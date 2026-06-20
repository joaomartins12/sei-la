// Copyright (C) 2007 - 2018 Wellbia.com Co., Ltd.

#ifndef ZCWAVE_ERRORS
#define ZCWAVE_ERRORS

// Successful
#define ZCWAVE_OK									0

// An error occurred due to missing x3.xem in specified XIGNCODE directory
#define ZCWAVE_E_CANT_FIND_X3_DLL					0xE0FF007E

// An error occurred due to insufficient amount of system resources
#define ZCWAVE_E_INSUFFICIENT_RESOURCES				0xE0010003

// File Corruption

// An error occurred due to corrupted XIGNCODE3 DNA
#define ZCWAVE_E_DNA_CORRUPTED						0xE0190101

// An error occurred due to corrupted XIGNCODE3 system files
#define ZCWAVE_E_XCLIO_CORRUPTED					0xE0190102

// An error occurred due to corrupted XIGNCODE3 system files
#define ZCWAVE_E_XSG_CORRUPTED						0xE0190103

// An error occurred due to invalid license code
#define ZCWAVE_E_INVALID_LICENSE					0xE0190104

// An error occurred due to corrupted XIGNCODE3 system files
#define ZCWAVE_E_XEZ_CORRUPTED						0xE0190105

// An error occurred due to corrupted allowed program list in XIGNCODE3 Pattern system
#define ZCWAVE_E_WHITE_DB_MISSING					0xE0190106

// An error occurred due to corrupted blocked program list in XIGNCODE3 Pattern system
#define ZCWAVE_E_BLACK_DB_MISSING					0xE0190107

// An error occurred due to corrupted XIGNCODE3 system files
#define ZCWAVE_E_ENGINE_CORRUPTED					0xE0190108

// An error occurred due to corrupted XIGNCODE3 system files
#define ZCWAVE_E_PHANTOM_CORRUPTED					0xE0190109

// An error occurred due to corrupted XIGNCODE3 hash data file
#define ZCWAVE_E_HASH_DATA_CORRUPTED				0xE019010A

// An error occurred due to missing XIGNCODE3 hash data file
#define ZCWAVE_E_HASH_DATA_MISSING					0xE019010B

// An error occurred due to corrupted XIGNCODE3 system files
#define ZCWAVE_E_XDL_CORRUPTED						0xE019010C

// An error occurred due to corrupted XIGNCODE3 Pattern file
#define ZCWAVE_E_INVALID_SPO_MAGIC					0xE019010D

// An error occurred due to corrupted XIGNCODE3 Pattern file
#define ZCWAVE_E_INVALID_SPO_VERSION				0xE019010E

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_X3_EGG_QFN			0xE019010F

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_X3_EGG_SCVA			0xE0190110

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_QUERYFUNCTION			0xE0190111

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_INIT					0xE0190112

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_CLEANUP				0xE0190113

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_PROBE					0xE0190114

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_CREATECODEBOX			0xE0190115

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_CLOSECODEBOX			0xE0190116

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_PROBECODEBOX			0xE0190117

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_SENDCOMMANDVA			0xE0190118

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_SYSENTERW				0xE0190119

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_SYSEXIT				0xE019011A

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_REGISTERCALLBACK		0xE019011B

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_PROBECODEBOXEX		0xE019011C

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_PROBEEX				0xE019011D

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_ENCRYPT				0xE019011E

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_DECRYPT				0xE019011F

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSAGENERATEKEY		0xE0190120

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSASETPRIVATEKEY		0xE0190121

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSASETPUBLICKEY		0xE0190122

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSAPRIVATEENCRYPT		0xE0190123

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSAPRIVATEDESCRYPT	0xE0190124

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSAPUBLICENCRYPT		0xE0190125

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSAPUBLICDECRYPT		0xE0190126

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSACLOSE				0xE0190127

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSACREATE				0xE0190128

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_RSAFREEBUFFER			0xE0190129

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_SOLOMON_CORRUPTED					0xE019012A

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_X3_EGG_SYSENTERW		0xE019012B

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_LOAD_RAPHAEL_FROYO			0xE019012C

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_RAPHAEL_CORRUPTED					0xE019012D

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_UNPACK_XXD_FAIL					0xE019012E

// An error occurred due to corrupted x3.xem
#define ZCWAVE_E_CANT_RESOLVE_X3_EGG_REGISTERCALLBACK	0xE019012F

// An error occurred due to tampering of XIGNCODE3 system
#define ZCWAVE_E_MOD_XCLIO							0xE0190130

// An error occurred due to tampering of XIGNCODE3 system
#define ZCWAVE_E_MOD_XST							0xE0190131

// An error occurred while creating RPC server file
#define ZCWAVE_E_UNPACK_RPC_SERVER_X64_FAIL			0xE0190132

// An error occurred while creating RPC server thread
#define ZCWAVE_E_RPC_SERVER_THREAD_FAIL				0xE0190133

#define ZCWAVE_E_CANT_RESOLVE_SYSENTEREXW			0xE0190134

// Internal Function Failure

// An error occurred while processing XIGNCODE3 internal log
#define ZCWAVE_E_LOG_FAIL							0xE0190201

// An error occurred due to XIGNCODE3 internal IPC process failure
#define ZCWAVE_E_IPC_FAIL							0xE0190202

// An error occurred due to invalid operation during XIGNCODE3 operational routine
#define ZCWAVE_E_INVALID_OPERATION					0xE0190203

// An error occurred while creating XIGNCODE3 system ID.
// An error occurred due to lack of network card on the system
#define ZCWAVE_E_SYSTEM_ID_FAIL						0xE0190204

// An error occurred due to tampering of XIGNCODE3 internal status
#define ZCWAVE_E_INTERNAL_STATE_CHANGED				0xE0190205

// An error occurred during XIGNCODE3 internal IPC network process
#define ZCWAVE_E_DETECT_IPC_ERROR					0xE0190206

// An error occurred while checking XIGNCODE3 status
#define ZCWAVE_E_XVC_CHECK_FAIL						0xE0190207

// An error occurred due to terminated XIGNCODE3's watchdog process, xxd.xem
#define ZCWAVE_E_XXD_TERMINATED						0xE0190208

// An error occurred due to debugged XIGNCODE3 watchdog process
#define ZCWAVE_E_DETECT_XXD_DEBUGGING				0xE0190209

// An error occurred while creating XIGNCODE3 watchdog process file
#define ZCWAVE_E_CREATE_XXD_FAIL					0xE019020A

// An error occurred due to incorrect XIGNCODE3 API call point
#define ZCWAVE_E_CALLER_CORRUPTED					0xE019020B

// An error occurred due to corrupted XIGNCODE3 API callstack
#define ZCWAVE_E_CALLSTACK_CORRUPTED				0xE019020C

// An error occurred while loading XIGNCODE3 detection codes
#define ZCWAVE_E_CANT_LOAD_COOPER					0xE019020D

// An error occurred while downloading XIGNCODE3 detection codes
#define ZCWAVE_E_CANT_DOWNLOAD_COOPER				0xE019020E

// An error occurred while creating XIGNCODE3 policy file
#define ZCWAVE_E_CREATE_PLC_FAIL					0xE019020F

// An error occurred while initializing XIGNCODE3 scanner
#define ZCWAVE_E_ENGINE_INIT_FAIL					0xE0190210

// An error occurred while loading XIGNCODE3 allowed program database
#define ZCWAVE_E_WHITE_DB_READ_FAIL					0xE0190211

// An error occurred while loading XIGNCODE3 blocked program database
#define ZCWAVE_E_BLACK_DB_READ_FAIL					0xE0190212

// An error occurred while loading XIGNCODE3 scanner engine
#define ZCWAVE_E_ENGINE_LOAD_FAIL					0xE0190213

// An error occurred while checking XIGNCODE3 heartbeat
#define ZCWAVE_E_CHECK_HEART_BEAT					0xE0190214

// An error occurred while finding XIGNCODE3 module interface
#define ZCWAVE_E_CANT_FIND_INTERFACE				0xE0190215

// An error occurred while creating XIGNCODE3 wormhole path
#define ZCWAVE_E_CANT_SETUP_WORMHOLE_PATH			0xE0190216

// An error occurred while creating XIGNCODE3 wormhole file
#define ZCWAVE_E_CANT_CREATE_WORMHOLE				0xE0190217

// An error occurred while loading XIGNCODE3 wormhole
#define ZCWAVE_E_CANT_LOAD_WORMHOLE					0xE0190218

// An error occurred due to incorrect XIGNCODE3 PE file load information
#define ZCWAVE_E_PE_LDR_MISSING						0xE0190219

// An error occurred due to incorrect XIGNCODE3 XMO file load information
#define ZCWAVE_E_XMO_LDR_MISSING					0xE019021A

// An error occurred due to missing XIGNCODE3 debug writer
#define ZCWAVE_E_DEBUG_WRITER_MISSING				0xE019021B

// An error occurred due to missing XIGNCODE3 log printer
#define ZCWAVE_E_LOG_PRINTER_MISSING				0xE019021C

// An error occurred due to incorrect XIGNCODE3 version information
#define ZCWAVE_E_INVALID_VERSION_SIZE				0xE019021D

// An error occurred due to missing XIGNCODE3 Solomon system interface
#define ZCWAVE_E_SOLOMON_MISSING					0xE019021E

// An error occurred while creating XIGNCODE3 wormhole interface
#define ZCWAVE_E_CANT_CREATE_WORMHOLE_INTERFACE		0xE019021F

// An error occurred due to incorrect XIGNCODE3 secure packet index
#define ZCWAVE_E_PACKET_INDEX_MISMATCH				0xE0190220

// An error occurred due to incorrect XIGNCODE3 secure packet index
#define ZCWAVE_E_PACKET_INDEX_OVERFLOW				0xE0190221

// An error occurred due to incorrect secure packet offset
#define ZCWAVE_E_PACKET_OFFSET_OVERFLOW				0xE0190222

// An error occurred due to missing code buffer matching XIGNCODE3 secure packet
#define ZCWAVE_E_NO_CODE_BUFFERS					0xE0190223

// An error occurred while finding XIGNCODE3 internal system entry
#define ZCWAVE_E_CANT_FIND_XCLIO_ENTRY				0xE0190224

// An error occurred while binding XIGNCODE3 internal API entry
#define ZCWAVE_E_CANT_RESOLVE_XCLIO_FN				0xE0190225

// An error occurred while creating XIGNCODE3 service table
#define ZCWAVE_E_CANT_LOAD_XST						0xE0190226

// Don't declare constant !!!

// An error occurred during XIGNCODE3 internal IPC process
#define ZCWAVE_E_CANT_RESOLVE_XCLIO_FN_LAST			0xE0190255

// An error occurred due to missing XIGNCODE3 execution buffer manager interface
#define ZCWAVE_E_EBM_MISSING						0xE0190256

// An error occurred due to terminated XIGNCODE3 watchdog thread
#define ZCWAVE_E_XXD_THREAD_TERMINATED				0xE0190257

// An error occurred due to suspended XIGNCODE3 watchdog thread
#define ZCWAVE_E_XXD_THREAD_SUSPENDED				0xE0190258

// An error occurred due to terminated XIGNCODE3 host process (x64)
#define ZCWAVE_E_CORONA_HOST_TERMINATED				0xE0190259

// An error occurred due to low engine interface level
#define ZCWAVE_E_ENGINE_INTERFACE_LEVEL_IS_LOW		0xE019025A

// An error occurred due to suspended engine update thread
#define ZCWAVE_E_UPDATE_THREAD_IS_SUSPENDED			0xE0190265

// External Function Failure

// An error occurred due to invalid handle passed to XIGNCODE3 API
#define ZCWAVE_E_INVALID_HANDLE						0xE0190301

// An error occurred due to invalid packet passed to XIGNCODE3 API
#define ZCWAVE_E_INVALID_PACKET						0xE0190302

// An error occurred while initializing XIGNCODE3
#define ZCWAVE_E_NOT_INITIALIZED					0xE0190304

// An error occurred due to infinite processing of XIGNCODE3 secure packet
#define ZCWAVE_E_PROBE_CONTINUE						0xE0190305

// An error occurred while creating XIGNCODE3 policy manager interface
#define ZCWAVE_E_CANT_CREATE_PLC_MGR				0xE0190306

// An error occurred while loading XIGNCODE3 system
#define ZCWAVE_E_XSG_LOAD_FAIL						0xE0190307

// An error occurred due to missing XIGNCODE3 system functions
#define ZCWAVE_E_XSG_FUNCTION_MISSING				0xE0190308

// An error occurred due to XIGNCODE3 system initialization failure
#define ZCWAVE_E_XSG_INIT_FAIL						0xE0190309

// An error occurred while protecting XIGNCODE3 applied game process
#define ZCWAVE_E_XSG_PROTECT_FAIL					0xE019030A

// An error occurred while obtaining XIGNCODE3 descriptor
#define ZCWAVE_E_GET_XIGNCODE_DESC_FAIL				0xE019030B

// An error occurred while configuring XIGNCODE3 descriptor
#define ZCWAVE_E_SET_XIGNCODE_DESC_FAIL				0xE019030C

// An error occurred while loading XIGNCODE3 packet processing module
#define ZCWAVE_E_LOAD_XPL_FAIL						0xE019030D

// An error occurred due to missing XIGNCODE3 packet processing module functions
#define ZCWAVE_E_XPL_FUNCTION_MISSING				0xE019030E

// An error occurred while creating XIGNCODE3 packet processing interface
#define ZCWAVE_E_CANT_CREATE_PACKET_PROVIDER		0xE019030F

// An error occurred while creating XIGNCODE3 firewall driver (x86)
#define ZCWAVE_E_CANT_EXTRACT_XSPIRIT32				0xE0190310

// An error occurred while creating XIGNCODE3 firewall driver (x64)
#define ZCWAVE_E_CANT_EXTRACT_XSPIRIT64				0xE0190311

// An error occurred while loading XIGNCODE3 firewall driver
#define ZCWAVE_E_CANT_LOAD_XSPIRIT					0xE0190312

// An error occurred due to invalid XIGNCODE3 secure packet version
#define ZCWAVE_E_PACKET_VERSION_ERROR				0xE0190313

// An error occurred while XIGNCODE3 secure packet CRC check
#define ZCWAVE_E_PACKET_CRC_ERROR					0xE0190314

// An error occurred while initializing XIGNCODE3 firewall module
#define ZCWAVE_E_PHANTOM_INIT_FAIL					0xE0190315

// An error occurred while initializing XIGNCODE3 detection logic module
#define ZCWAVE_E_XEZ_INIT_FAIL						0xE1090316

// An error occurred while loading XIGNCODE3 detection logic module
#define ZCWAVE_E_XEZ_LOAD_FAIL						0xE1090317

// An error occurred while initializing XIGNCODE3 resource reporter interface
#define ZCWAVE_E_XJOON_INIT_FAIL					0xE1090318

// An error occurred while loading XIGNCODE3 resource reporter interface
#define ZCWAVE_E_XJOON_LOAD_FAIL					0xE1090319

// An error occurred due to incorrect XIGNCODE3 resource report module channel
#define ZCWAVE_E_XJOON_CHANNEL_FAIL					0xE109031A

// An error occurred while loading XIGNCODE3 driver with administrative privilege
#define ZCWAVE_E_XSG_INIT_FAIL_ADMIN_A				0xE109031B

// An error occurred while loading XIGNCODE3 driver with administrative privilege
#define ZCWAVE_E_XSG_INIT_FAIL_ADMIN_B				0xE109031C

// An error occurred while loading XIGNCODE3 driver with administrative privilege
#define ZCWAVE_E_XSG_INIT_FAIL_ADMIN_C				0xE109031D

// An error occurred while loading XIGNCODE3 driver with administrative privilege
#define ZCWAVE_E_XSG_INIT_FAIL_ADMIN_D				0xE109031E

//

// An error occurred due to kernel system function called from incorrect path
#define ZCWAVE_E_INVALID_CALL						0xE0190401

// An error occurred due to corrupted kernel SDT
#define ZCWAVE_E_INVALID_SDT						0xE0190402

// An error occurred due to tampering of XIGNCODE3 kernel code
#define ZCWAVE_E_KERNEL_CODE_CHANGED				0xE0190403

// An error occurred due to kernel mode debugging detection
#define ZCWAVE_E_DETECT_DEBUGGER					0xE0190404

// An error occurred due to modification to IDT
#define ZCWAVE_E_DETECT_IDT_MODIFIED				0xE0190405

// An error occurred due to malicious driver detection
#define ZCWAVE_E_DETECT_MAL_DRIVER					0xE0190406

// An error occurred due to DirectX hook detection
#define ZCWAVE_E_DETECT_DX_HOOK						0xE0190407

// An error occurred due to malicious settings detection
#define ZCWAVE_E_DETECT_MAL_INI						0xE0190408

// An error occurred due to malicious network I/O detection
#define ZCWAVE_E_DETECT_MAL_NET_IO					0xE0190409

// An error occurred due to "Speed Hack" detection
#define ZCWAVE_E_DETECT_SPEED_HACK					0xE019040A



//

// An error occurred due to unprepared user information
#define ZCWAVE_E_USER_INFO_IS_NOT_READY				0xE0190501

// An error occurred due to user information inaccessibility
#define ZCWAVE_E_USER_INFO_ACCESS_DENIED			0xE0190502

// User Environment Problem

// An error occurred due to the operating system running XIGNCODE3 is unsupported
#define ZCWAVE_E_DETECT_UNKNOWN_OS					0xE0191001

// An error occurred due to an incompatible program is running with XIGNCODE3
#define ZCWAVE_E_ALREADY_HOOKED						0xE0191002

// An error occurred due to VM environment detection
#define ZCWAVE_E_DETECT_VM							0xE0191003

// An error occurred due to compatibility check for Windows 98 and/or prior operating system has been set
#define ZCWAVE_E_DETECT_COMPAT_9X					0xE0191004

// An error occurred due to running XIGNCODE3 process exists
#define ZCWAVE_E_ALREADY_RUNNING					0xE0191005

// An error occurred while loading XIGNCODE3 driver
#define ZCWAVE_E_CANT_LOAD_DRIVER					0xE0191006

// An error occurred due to tampering of XIGNCODE3 client code
#define ZCWAVE_E_CLIENT_CHANGED						0xE0191007

// An error occurred due to XIGNCODE3 file creation failure
#define ZCWAVE_E_CANT_CREATE_FILE					0xE0191008

// An error occurred due to XIGNCODE3 update failure
#define ZCWAVE_E_UPDATE_FAIL						0xE0191009

// An error occurred due to another XIGNCODE3 update in progress
#define ZCWAVE_E_ALREADY_UPDATING					0xE019100A

// An error due to malicious software(s) detection
#define ZCWAVE_E_BLACKCODE							0xE019100B

// An error occurred due to the user system being blocked by Solomon system
#define ZCWAVE_E_BLOCKED_SYSTEM						0xE019100C

// An error occurred due to the user account being blocked by Solomon system
#define ZCWAVE_E_BLOCKED_ACCOUNT					0xE019100D

// An error occurred due to tampering of game resources
#define ZCWAVE_E_RESOURCE_CORRUPTED					0xE019100E

// An error occurred due to malicious hardware device detection
#define ZCWAVE_E_HID_DETECTED						0xE019100F

// An error occurred due to malicious software device detection
#define ZCWAVE_E_HID_SW_DETECTED					0xE0191010

// An error occurred due to initialization of XIGNCODE3 which already has taken place
#define ZCWAVE_E_ALREADY_INITIALIZED				0xE0191011

// An error occurred due to the game launch from unsupported region
#define ZCWAVE_E_BLOCKED_COUNTRY					0xE0191012

// An error occurred due to the detection of remote control software
#define ZCWAVE_E_REMOTE_CONTROL						0xE0191013

// An error occurred due to simultaneous launch of game clients. (Up to specified number)
#define ZCWAVE_E_MULTI_CLIENT						0xE0191014

// An error occurred due to root level privilege process detection
#define ZCWAVE_E_ROOT								0xE0191015

// An error occurred due to tamping of APK file
#define ZCWAVE_E_REPACK								0xE0191016

// An error occurred due to application launch on a custom ROM
#define ZCWAVE_E_CUSTOM_ENVIRONMENT					0xE0191017

// An error occurred due to the tampering of system environment
#define ZCWAVE_E_ENVIRONMENT_MODIFICATION			0xE0191018

// An error occurred due to malicious package detection
#define ZCWAVE_E_BLACK_PACKAGE						0xE0191019

// An error occurred due to the detection of malicious behavior
#define ZCWAVE_E_BLACK_ACTIVITY						0xE019101A

// An error occurred due to the detection of a designated server that unwillingly unintegrated XIGNCODE3
#define ZCWAVE_E_BLACK_SERVER						0xE019101B

// An error occurred due to XIGNCODE3 launch with erroneously distributed XIGNCODE3 files
#define ZCWAVE_E_INVALID_INTERFACE_FILE				0xE019101C

// xtrinity error

// An error occurred while protector finding cipher decryption descriptor
#define XTRINITY_E_NO_DESCRIPTOR					0xE0181001

// An error occurred due to protector failing to find XIGNCODE3 directory
#define XTRINITY_E_MISMATCH_XIGNCODE_DIRECTORY		0xE0181002

// An error occurred due to invalid protector license code
#define XTRINITY_E_MISMATCH_LICENSE					0xE0181003

// xsg

// An error occurred due to insufficient resources for XIGNCODE3 driver (x64)
#define XSG_E_NO_64BIT_RESOURCE						0xE0110001

// An error occurred due to insufficient resources for XIGNCODE3 driver (x86)
#define XSG_E_NO_32BIT_RESOURCE						0xE0110002

// An error occurred while loading XIGNCODE3 game protection driver
#define XSG_E_DRIVER_LOAD_FAIL						0xE0110003

// An error occurred due to insufficient resources for XIGNCODE3 compatible driver on Windows Vista (x86) and above
#define XSG_E_NO_32BIT_8_RESOURCE					0xE0110004

// An error occurred due to XIGNCODE3 game protection driver incompatibility
#define XSG_E_DRIVER_IS_NOT_SUPPORTED				0xE0110005

// An error occurred due to disabled XIGNCODE3 game protection driver service
#define XSG_E_SERVICE_DISABLED						0xE0110006

// An error occurred due to revoked permission for XIGNCODE3 game protection driver service
#define XSG_E_ACCESS_DENIED							0xE0110007

// xmorpheus

// An error occurred while binding XIGNCODE3 early protection function
#define XMORPHEUS_E_CANT_RESOLVE_SCVA				0xE0120001

// An error occurred while finding XIGNCODE3 early protection entry function
#define XMORPHEUS_E_CANT_RESOLVE_QFN				0xE0120002

// base

// An error occurred due to general reasons
#define X_E_UNSUCCESSFUL							0xE0010001

// An error occurred due to invalid argument passed to XIGNCODE3 function
#define X_E_INVALID_PARAMETER						0xE0010002

// An error occurred due to insufficient amount of system resources
#define X_E_INSUFFICIENT_RESOURCES					0xE0010003

// An error occurred while opening a file
#define X_E_CANT_OPEN_FILE							0xE0010004

// An error occurred while reading a file
#define X_E_CANT_READ_FILE							0xE0010005

// An error occurred while creating a file
#define X_E_CANT_CREATE_FILE						0xE0010006

// An error occurred while locking
#define X_E_LOCK_FAIL								0xE0010007

// An error occurred due to buffer overflow
#define X_E_BUFFER_OVERFLOW							0xE0010008

// An error occurred due to invalid operation
#define X_E_INVALID_OPERATION						0xE0010009

// An error occurred while converting Unicode
#define X_E_UNICODE_ERROR							0xE001000A

// An error occurred due to insufficient amount of local buffer resources
#define X_E_INSUFFICIENT_LOCAL_BUFFER				0xE001000B

// An error occurred due to missing XIGNCODE3 interface
#define X_E_INTERFACE_MISSING						0xE001000C

// An error occurred while writing to a file
#define X_E_CANT_WRITE_FILE							0xE001000D

// An error occurred while creating XIGNCODE3 object
#define X_E_CANT_CREATE_OBJECT						0xE001000E

// An error occurred due to halt request from XIGNCODE3 callback function
#define X_E_CALLBACK_WANT_TO_STOP					0xE001000F

#define X_E_NOT_IMPLMENTED							0xE0010010

#define XM_BASE 1

// An error occurred due to corrupted XIGNCODE3 Pattern data
#define X_E_SPO_NO_DATA								MAKE_E_XSTATUS(XM_BASE, 51)

// An error occurred due to corrupted XIGNCODE3 Pattern data
#define X_E_SPO_INVALID_MAGIC						MAKE_E_XSTATUS(XM_BASE, 52)

// An error occurred due to corrupted XIGNCODE3 Pattern data
#define X_E_SPO_INVALID_VERSION						MAKE_E_XSTATUS(XM_BASE, 53)

// An error occurred due to corrupted XIGNCODE3 Pattern data
#define X_E_SPO_ALREADY_LOADED						MAKE_E_XSTATUS(XM_BASE, 54)

// An error occurred due to missing execution image inspection module
#define X_E_IMC_NO_MODULE							MAKE_E_XSTATUS(XM_BASE, 61)

// An error occurred due to failed interpretation of execution image
#define X_E_XST_NTH_INVALID_SIZE					MAKE_E_XSTATUS(XM_BASE, 71)

// An error occurred due to failed interpretation of execution image
#define X_E_XST_NTH_INVALID_DOS_SIG					MAKE_E_XSTATUS(XM_BASE, 72)

// An error occurred due to failed interpretation of execution image
#define X_E_XST_NTH_INVALID_HDR_RANGE				MAKE_E_XSTATUS(XM_BASE, 73)

// An error occurred due to failed interpretation of execution image
#define X_E_XST_NTH_INVALID_NT_SIG					MAKE_E_XSTATUS(XM_BASE, 74)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_NO_SUCH_FILE						MAKE_E_XSTATUS(XM_BASE, 1001)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_FILE_SIZE					MAKE_E_XSTATUS(XM_BASE, 1002)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_BLOCK_COUNT					MAKE_E_XSTATUS(XM_BASE, 1003)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_NULL_BLOCK_REFERENCE				MAKE_E_XSTATUS(XM_BASE, 1004)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_DUP_BLOCK_REFERENCE					MAKE_E_XSTATUS(XM_BASE, 1005)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_MAGIC						MAKE_E_XSTATUS(XM_BASE, 1006)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_USED_COUNT					MAKE_E_XSTATUS(XM_BASE, 1007)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_FREE_COUNT					MAKE_E_XSTATUS(XM_BASE, 1008)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_MD5							MAKE_E_XSTATUS(XM_BASE, 1009)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_INVALID_CHECKSUM					MAKE_E_XSTATUS(XM_BASE, 1010)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_RELEASE_BLOCK_FAIL					MAKE_E_XSTATUS(XM_BASE, 1011)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_FILE_ALREADY_EXISTS					MAKE_E_XSTATUS(XM_BASE, 1012)

// An error occurred due to corrupted MFS file system
#define X_E_MFS_NO_TAG								MAKE_E_XSTATUS(XM_BASE, 1013)

// An error occurred due to failed interpretation of the operating system
#define X_E_SH_CANT_FIND_SSEH_1						MAKE_E_XSTATUS(XM_BASE, 2001)

// An error occurred due to failed interpretation of the operating system
#define X_E_SH_CANT_FIND_SSEH_2						MAKE_E_XSTATUS(XM_BASE, 2002)

// An error occurred due to failed interpretation of the operating system
#define X_E_SH_CANT_FIND_SSEH_3						MAKE_E_XSTATUS(XM_BASE, 2003)

// An error occurred while opening a compressed file
#define X_E_MUNZ_CANT_OPEN_FILE						0xE0013001

#define X_E_WINSOCK_BASE							0xE0300000

#define X_E_PETOOL_TOO_SMALL_FILE					0xE0400001

#define X_E_LUA_BASE								0xE0301000
#define X_E_LUA_BASE_END							0xE0302000
#define X_E_LUA_CANT_FIND_FUNCTION					0xE0302001

#define X_E_CANT_FIND_PE_HEADER						0xE0230001
#define X_E_XSG_DNA_IS_CORRUPTED					0xE0230002
#define X_E_NOA_DNA_IS_CORRUPTED					0xE0230003
#define X_E_KAGA_DNA_IS_CORRUPTED					0xE0230004
#define X_E_EZ_DNA_IS_CORRUPTED						0xE0230005
#define X_E_MISSING_RPT_THREAD_PTR					0xE0230006

#define X_E_MISSING_LOG_THREAD_PTR					0xE0230007
#define X_E_MISSING_PROBE_THREAD_PTR				0xE0230008
#define X_E_MISSING_ENVIRONMENT_MANAGER				0xE0230009
#define X_E_MISSING_XSG_PTR							0xE023000A
#define X_E_MISSING_EZ_PTR							0xE023000B
#define X_E_EZ_IS_DEAD								0xE023000C
#define X_E_MISSING_NOA_PTR							0xE023000D
#define X_E_NOA_IS_DEAD								0xE023000E
#define X_E_MISSING_LOGGER_PTR						0xE023000F

#define X_E_LOG_THREAD_IS_DEAD						0xE0230010
#define X_E_LOG_THREAD_IS_SUSPENDED					0xE0230011
#define X_E_RPT_THREAD_IS_DEAD						0xE0230012
#define X_E_RPT_THREAD_IS_SUSPENDED					0xE0230013
#define X_E_PROBE_THREAD_IS_DEAD					0xE0230014
#define X_E_PROBE_THREAD_IS_SUSPENDED				0xE0230015
#define X_E_RPT_TICK_IS_CORRUPTED					0xE0230016
#define X_E_XDL_MISSING_XDL_PTR						0xE0230017

#define X_E_XDL_IS_DEAD								0xE0230018
#define X_E_XCLIO_IS_CORRUPTED						0xE0230019
#define X_E_PROCESS_INTEGRITY_IS_CORRUPTED			0xE023001A
#define X_E_XSG_IS_DEAD								0xE023001B
#define X_E_XSG_IS_CORRUPTED						0xE023001C
#define X_E_DISPATCHER_IS_DEAD						0xE023001D



#define X_E_CMD_CHECK_SEED_IS_CORRUPTED				0xE023001D
#define X_E_WRONG_CMD_SEED							0xE023001E
#define X_E_NO_MORPHEUS_PTR							0xE023001F
#define X_E_RELOC_SIZE_IS_CORRUPTED					0xE0230020
#define X_E_NO_ADDRESS_OF_ENTRY_POINT				0xE0230021
#define X_E_NO_RELOC_BLOCK							0xE0230022
#define X_E_TRINITY_CHECKSUM_IS_CORRUPTED			0xE0230023
#define X_E_NO_NT32_HEADERS							0xE0230024
#define X_E_NO_USER_CALLBACK_A						0xE0230025
#define X_E_NO_USER_CALLBACK_W						0xE0230026
#define X_E_DNA_USER_INFO_IS_OFF					0xE0230027
#define X_E_CANT_GET_USER_INFO						0xE0230028

#define X_E_CANT_LOAD_XMO_FROM_PATH					0xE0230029
#define X_E_CANT_LOAD_XMO_FROM_NAME					0xE023002A
#define X_E_CANT_CREATE_XMO_INTERFACE				0xE023002B
#define X_E_CANT_EXECUTE_FROM_MFS					0xE023002C
#define X_E_CANT_GET_ADAPTERS_INFO					0xE023002D
#define X_E_CANT_GET_PAREANT_PROCESS_ID				0xE023002E
#define X_E_CANT_CREATE_FILE_FROM_MFS				0xE023002F
#define X_E_CANT_WRITE_FILE_FROM_MFS				0xE0230030
#define X_E_CANT_MAKE_FILE_FROM_MFS					0xE0230031

#define X_E_INVALID_READ_SIZE						0xE0230032
#define X_E_FILE_CONTENTS_IS_NOT_VALID				0xE0230033
#define X_E_INVALID_FILE_SIZE						0xE0230034
#define X_E_XSG_CHECKSUM_FAIL						0xE0230035
#define X_E_CANT_LOAD_XMO							0xE0230036
#define X_E_CANT_LOAD_DATA_FROM_MFS					0xE0230037
#define X_E_INVALID_WRITE_SIZE						0xE0230038
#define X_E_INVALID_SYSTEM_ID_MAGIC					0xE0230039
#define X_E_NO_ANTI_VIRUS							0xE023003A
#define X_E_CANT_DOWNLOAD_CODE						0xE023003B
#define X_E_INVALID_DOWNLOAD_SIZE					0xE023003C
#define X_E_INVALID_DOWNLOAD_MD5					0xE023003D
#define X_E_DP_DATA_IS_CORRUPTED					0xE023003E
#define X_E_MISSING_RPT_PTR							0xE023003F
#define X_E_DLW_RPT_ITEM_BUFFER_IS_FULL				0xE0230040
#define X_E_CANT_FIND_DLW_RPT_ITEM					0xE0230041
#define X_E_NO_DLW_RPT_ITEM							0xE0230042
#define X_E_ROSWELL_NO_BLACK_FILE					0xE0230043

#define X_E_NO_WORMHOLE_INTERFACE_FUNCTION			0xE0230044
#define X_E_CANT_LAUNCH_XXD							0xE0230045

#define X_E_ELK_LOG_IS_NOT_BLACK					0xE0230046
#define X_E_ELK_INVALID_SOCKET						0xE0230047

#define X_E_INFO_LENGTH_MISMATCH					0xE0230048


#define X_E_INVALID_LUA_BUFFER						0xE0230049
#define X_E_INVALID_LUA_MODULE						0xE023004A
#define X_E_CANT_FIND_LUA_FUNCTION					0xE023004B
#define X_E_CANT_FIND_ALTITUDE						0xE023004C
#define X_E_CANT_FIND_RESOURCE						0xE023004D

#define	X_E_NOT_IMPLEMENTED_SYSCALL_1				0xE0230051
#define	X_E_NOT_IMPLEMENTED_SYSCALL_2				0xE0230052
#define	X_E_NOT_IMPLEMENTED_SYSCALL_3				0xE0230053
#define	X_E_NOT_IMPLEMENTED_SYSCALL_4				0xE0230054

#define X_E_IAT_BINDING_INCOMPLETE					0xE0230100

#define X_E_TOMCRYPT_BASE							0xE0238000

// DO NOT DEFINE ANYTHING

#define X_E_TOMCRYPT_END							0xE0238FFF



#endif
