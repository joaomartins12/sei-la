#include "base.h"
#include "asio.hpp"

#include "cClient.h"

#include <thread>

#include "asio/system_error.hpp"
#include "../LibProj/CsFunc/CsMessageBox.h"

//#include "lib_netsockets/Include/json_message.hh"

using namespace asio::ip;

static asio::io_context io_ctx;

// Work guard keeps io_ctx.run() from returning when the current cClient
// instance's pending operations all drain (which happens on logout when the
// game cClient is destroyed and its socket is cancel()'d).  Without it:
//
//   1. Game session ends, ~cClient cancels its socket, all in-flight async
//      ops complete with operation_aborted, io_ctx goes idle.
//   2. The detached worker thread's io_ctx.run() returns; thread exits.
//   3. Server-select self-heal creates a new cCliAccount and calls Run() →
//      starts a new worker thread that calls io_ctx.run().
//   4. But io_ctx is in the "stopped/exhausted" state — run() returns
//      immediately without pumping any completion handlers.
//   5. async_connect's TCP handshake still finishes at the OS level (Windows
//      IOCP completes the kernel-side connect regardless), and async_write
//      bytes still reach the wire — but the connect-completion handler never
//      fires, so OnConnected → SendLogin is never called, and the server's
//      handshake/reply bytes pile up in the IOCP queue with nobody to dispatch
//      them.  Result: server log shows the post-logout TCP connection +
//      client-sent ClusterList request, but never sees a Login packet, and
//      the client never processes the reply → empty server-select list.
//
// The work guard holds a "fake" outstanding work item so io_ctx.run() blocks
// indefinitely.  We only release it at process exit.
static auto io_work = asio::make_work_guard(io_ctx);

static bool IsSocketCloseSpamError(const std::error_code& ec)
{
	if (!ec)
		return false;

	// ASIO cancel/destructor/logout.
	if (ec == asio::error::operation_aborted)
		return true;

	const int code = ec.value();

	// Windows socket errors:
	// 10054 = connection reset by peer
	// 10057 = socket is not connected
	// 10058 = cannot send after socket shutdown
	return code == 10054 || code == 10057 || code == 10058;
}

cClient::cClient(uint16_t) : enc(false), m_v(0), m_timets(0), m_sock(io_ctx), m_a(false), m_sb(false)
{

}

bool cClient::Connect(const char* ip, uint16_t port)
{
	//CsMessageBoxA(MB_OK, "\n connecting to %s::%d\n", ip, port);

	if (port == 7029)
	{
		DBG("\nConnecting to Auth Server on %s:%d\n", ip, port);
	}
	else if (port == 7050)
	{
		DBG("\nConnecting to Character Server on %s:%d\n", ip, port);
	}
	else
	{
		DBG("\nConnecting to Game Server on %s:%d\n", ip, port);
	}

	if (m_a)
	{
		m_sock.cancel();
		m_sock.close();
	}

	m_sock.async_connect(tcp::endpoint(address().from_string(trim(ip)), port), [this](const std::error_code& ec)
		{
			if (ec)
			{
				if (ec.value() == 10061)
				{
					DBG("\nError %d: Server not opened\n", ec);
					OnDisconnected((char*)"Disconnecting with error 10061");
				}
				else
				{
					DBG("\nError connecting to AuthServer: %d\n", ec);
					OnDisconnected((char*)"Disconnecting with error 10003");
				}

				m_a = false;
			}
			else
			{
				OnConnected();
				StartRead();
			}
		});

	m_a = true;
	return true;
}

void cClient::EnableEncryption(unsigned int version)
{
	// TODO
}

void cClient::DoSend(cPacket& p)
{
	if (m_sb) {

#ifdef DEBUG_NETWORK
		CsMessageBoxA(MB_OK, "Requests disabled for now");
#endif
		return;
	}

	// Se a socket já está fechada, não tenta enviar nem mostra popup.
	if (!m_a || !m_sock.is_open())
	{
		DBG("\nDoSend ignored: socket closed or inactive.\n");
		return;
	}

	// Original code captured `&p` by reference and called `p.m_buf.consume(bt)`
	// inside the async_write completion handler. Callers (SendXxx) build the
	// cPacket on their stack and return immediately after DoSend, so by the time
	// the io_context thread fires the handler, `p` has been destroyed — `m_buf`
	// is freed memory and `consume()` blows up at +0x13/+0x15 inside
	// asio::basic_streambuf::consume.  That's the recurring teardown crash.
	//
	// Fix: copy the wire bytes into a heap-owned vector that the lambda holds
	// via shared_ptr, and consume the caller's streambuf SYNCHRONOUSLY here.
	// The caller's cPacket can now safely go out of scope while the async write
	// is in flight.
	auto bytes = std::make_shared<std::vector<uint8_t>>(p.real_len);
	asio::buffer_copy(asio::buffer(*bytes), p.m_buf.data(), p.real_len);
	p.m_buf.consume(p.real_len);

	asio::async_write(m_sock, asio::buffer(*bytes), asio::transfer_exactly(bytes->size()),
		[this, bytes](const std::error_code& ec, size_t /*bt*/)
		{
			if (ec)
			{
				// operation_aborted / 10054 / 10057 / 10058 são normais quando:
				// - o servidor fechou a ligação;
				// - o client mudou de servidor;
				// - logout / cancel / close;
				// - socket já foi encerrada.
				//
				// Antes isto mostrava sempre:
				// "Error while transfering bytes 10058"
				//
				// Agora apenas ignora estes casos para não spammar popup.
				if (IsSocketCloseSpamError(ec))
				{
					DBG("\nDoSend ignored socket close error: %d\n", ec.value());
					m_a = false;
					return;
				}

#ifdef DEBUG_NETWORK
				CsMessageBoxA(MB_OK, "Error while transfering bytes %d", ec.value());
#endif

				OnDisconnected((char*)"10003");
				m_a = false;
			}
		});
}

void cClient::DoDisconnect()
{
	try
	{
		m_sock.shutdown(m_sock.shutdown_send);
	}
	catch (...) {}

	m_a = false;
	DBG("\n");
	OnDisconnected((char*)"Disconecting from server");
	DBG("\n");
}

void cClient::Stop()
{

}

bool cClient::Bind()
{
	return true;
}

cClient::~cClient()
{
	// Inverted condition fix: original had `if (!m_sock.is_open()) m_sock.close();`
	// — closing only when NOT open. With an OPEN socket left untouched, the
	// in-flight async_read/async_write callbacks (which capture `this` by
	// reference / shared_ptr) continued firing on freed memory after this
	// destructor returned.
	//
	// Cancel pending async ops first so their lambdas fire with
	// operation_aborted before we tear down. Use the error_code overloads to
	// avoid throwing inside a destructor.
	asio::error_code ec;
	m_sock.cancel(ec);
	if (m_sock.is_open())
	{
		m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
		m_sock.close(ec);
	}

	// `m_t` is the worker thread that runs `io_ctx.run()` (started by
	// cClient::Run). `io_ctx.run()` loops until the io_context is stopped or
	// runs out of work — it doesn't return on its own when this socket
	// closes. So the thread is still joinable when the cClient destructor
	// runs. `std::thread::~thread()` calls `std::terminate()` on a joinable
	// thread, which is the recurring TERMINATE seen on logout / app exit at
	// `cClient::~cClient` line 160 (end of destructor). Detach so the thread
	// outlives the destructor without terminating; the OS reclaims it on app
	// exit. Per-instance leak (one thread per cClient ever created) — the
	// proper fix is one global io_ctx + one global worker thread tied to the
	// app lifetime, but that's a deeper refactor.
	if (m_t.joinable())
		m_t.detach();
}

bool cClient::Run(int unk)
{
	if (m_t.joinable())
		return true;

	std::thread t([]()
		{
			io_ctx.run();
		});

	m_t = std::move(t);
	return true;
}

void cClient::DoExecute()
{

}

void cClient::StartRead()
{
	asio::async_read(m_sock, m_read.m_buf.prepare(2), asio::transfer_exactly(2), [this](asio::error_code ec, std::size_t)
		{
			if (ec)
			{
				// operation_aborted = teardown cancel from ~cClient. The cClient
				// instance may already be destroyed by the time this fires (the
				// io_ctx worker thread is detached); calling OnDisconnected (virtual)
				// would __purecall on a torn-down vtable. Quiet bail.
				if (ec == asio::error::operation_aborted)
					return;

				DBG("\nError Code: %d\n", ec);
				OnDisconnected((char*)"10003");
				m_a = false;
				return;
			}

			m_read.m_buf.commit(2);

			uint16_t len;
			m_read.pop(len);

			m_read.real_len = len;

			int len2 = len - 2;

			if (len2 > 0) {
				ReadAll(len2);
			}
			else {
				StartRead();
			}
		});
}

void cClient::ReadAll(int b)
{
	asio::async_read(m_sock, m_read.m_buf.prepare(b), asio::transfer_exactly(b), [this](asio::error_code ec, std::size_t bt)
		{
			if (ec)
			{
				// Same teardown-cancel guard as StartRead — see comment there.
				if (ec == asio::error::operation_aborted)
					return;

				DBG("\nError Code: %d\n", ec);
				OnDisconnected((char*)"10003");
				m_a = false;
				return;
			}

			m_read.m_buf.commit(bt);

			m_read.pop(m_sproto);

			OnExecute();

			m_read.m_buf.consume(bt); // force consume everything else

			StartRead();
		});
}


void cClient::push(void* data, size_t len, bool wlen)
{
	m_write.push(data, len, wlen);
}

void cClient::pop(void* data, size_t len, bool wlen)
{
	m_read.pop(data, len, wlen);
}

void cClient::mark(void* data, size_t len)
{
	m_write.mark(data, len); // TODO: what is this?
}

void cClient::endp(uint16_t id)
{
	m_write.endp(id);
}

void cClient::newp(uint16_t id)
{
	m_write.newp(id);
}