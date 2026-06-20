{
	
	Copyright (c) 2007 - 2018 Wellbia.com Co., Ltd.

}

unit zwave_sdk_client;

interface

uses
	Windows;

const
	ZCWAVE_OK = 0;
	ZCWAVE_E_USER_INFO_IS_NOT_READY = $E0190501;
	ZCWAVE_E_USER_INFO_ACCESS_DENIED = $E0190502;

	XUID_USER_ACCOUNT   = 1;
	XUID_USER_NAME      = 2;
	XUID_USER_MAP_POINT = 3;
	XUID_USER_EXTRA     = 4;

	XclioFidSysEnterA = 0;
	XclioFidSysEnterW = 1;
	XclioFidSysExit = 2;
	XclioFidInit = 3;
	XclioFidCleanup = 4;
	XclioFidProbe = 5;
	XclioFidProbeEx = 6;
	XclioFidCreateCodeBox = 7;
	XclioFidCloseCodeBox = 8;
	XclioFidProbeCodeBox = 9;
	XclioFidProbeCodeBoxEx = 10;
	XclioFidEncrypt = 11;
	XclioFidDecrypt = 12;
	XclioFidRsaCreate = 13;
	XclioFidRsaClose = 14;
	XclioFidRsaSetPublicKey = 15;
	XclioFidRsaSetPrivateKey = 16;
	XclioFidRsaPublicEncrypt = 17;
	XclioFidRsaPublicDecrypt = 18;
	XclioFidRsaPrivateEncrypt = 19;
	XclioFidRsaPrivateDecrypt = 20;
	XclioFidCheck = 21;
	XclioFidRegisterCallback = 22;
	XclioFidRsaGenerateKey = 23;
	XclioFidRsaFreeBuffer = 24;
	XclioFidSetup = 25;
	XclioFidSendCommandVa = 26;

	ZCMD_SET_USER_INFO_CALLBACKA = 100;

type
	TGetUserInfoCallbackA = function (iid: cardinal; buffer: PChar; size: cardinal; context: Pointer) : cardinal stdcall;
	TProbeCallback = function (cbx: Pointer; pkt: Pointer; buffer: Pointer; size: cardinal; context: Pointer) : cardinal stdcall;

	TQueryFunction = function (ptr: PPointer; idx: cardinal) : cardinal stdcall;
	TSysEnterW = function (license: PChar; dir: PChar; flag: cardinal) : cardinal stdcall;
	TSysExit = function () : cardinal stdcall;
	TInit = function () : cardinal stdcall;
	TCleanup = function () : cardinal stdcall;
	TSendCommandVa = function (cid: cardinal; va_list: Pointer) : cardinal stdcall;
	TProbeEx = function (buffer: Pointer; size: cardinal; OnProbeComplete: TProbeCallback; context: Pointer) : cardinal stdcall;


	function ZCWAVE_SysExit() : boolean;
	function ZCWAVE_SysEnter(license: String; dir: String; cb: TGetUserInfoCallbackA) : boolean;
	function ZCWAVE_ProbeEx(buffer: Pointer; size: cardinal; OnProbeComplete: TProbeCallback; context: Pointer) : boolean;


var
	g_xigncode_module: cardinal;
	QueryFunction: TQueryFunction;
	SysExit: TSysExit;
	SendCommandVa: TSendCommandVa;
	ProbeEx: TProbeEx;
	SysEnterW: TSysEnterW;
	Cleanup: TCleanup;
	Init: TInit;

implementation

function xstrrscan(str:PAnsiChar; ch:Char):PAnsiChar;
var
	i:cardinal;
begin
	for i:=Length(str) downto 0 do
	begin
		if str[i] = ch 
		then begin
			Result := str+i;
			Exit;
		end;
	end;
	Result:= nil;
end;

function GetXigncodeDirectoryW(input_dir:string) : String;
var
	path : array[0..MAX_PATH-1] of Char;
	drive_mark : PChar;
	dir_mark : PChar;
begin
	if input_dir = ''
	then begin
		GetModuleFileNameA(0, path, MAX_PATH);
		dir_mark := xstrrscan(path, '\');
		if dir_mark <> nil 
		then begin
			dir_mark[0]:=Char(0);
		end;
	end
	else begin
		Move(input_dir[1], path[0], (length(input_dir)+1) * sizeof(char));
		drive_mark := xstrrscan(path, ':');
		if drive_mark <> nil
		then begin
			Move(path[0], input_dir[1], (length(input_dir)+1) * sizeof(char));
		end
		else begin
			GetModuleFileNameA(0, path, MAX_PATH);
			dir_mark := xstrrscan(path, '\');
			if dir_mark <> nil
			then begin
				if input_dir[1] = '\'
				then begin
					dir_mark[0] := Char(0);
				end
				else begin
					Move(input_dir[1], dir_mark[1], (length(input_dir)+1) * sizeof(char));
				end;
			end;
	end;
	Result := path;
	if Result[Length(Result)] = '\'
	then
		path[Length(Result)-1] := #0;
		Result := path;
	end;
end;


function ZCWAVE_SetUserInfoCallbackA(sva: TSendCommandVa; cb: TGetUserInfoCallbackA): cardinal;
asm
	push ebp
	mov ebp, esp

	push cb

	push esp
	push ZCMD_SET_USER_INFO_CALLBACKA
	call sva

	pop ebp
	pop ebp
end;

function ZCWAVE_ProbeEx(buffer: Pointer; size: cardinal; OnProbeComplete: TProbeCallback; context: Pointer) : boolean;
begin
	Result := False;

	if g_xigncode_module = 0 
	then begin
		Exit;
	end;

	if ProbeEx(buffer, size, OnProbeComplete, context) = 0
	then begin
		Exit;
	end;

	Result := True;
end;

function ZCWAVE_SysExit() : boolean;
begin
	Result := False;

	if g_xigncode_module = 0
	then begin
		Exit;
	end;

	Cleanup();
	SysExit();
	FreeLibrary(g_xigncode_module);

	g_xigncode_module := 0;
	Result := True
end;

function ZCWAVE_SysEnter(license: String; dir: String; cb: TGetUserInfoCallbackA) : boolean;
var
	res: cardinal;
	buffer: Array[0..80] of Word;
	path: Array[0..256] of Word;
	x3_path:String;
begin
	Result := False;

	if g_xigncode_module <> 0
	then begin
		Exit;
	end;

	x3_path := GetXigncodeDirectoryW(dir) + '\x3.xem';

	g_xigncode_module := LoadLibraryA(PAnsiChar(AnsiString(x3_path)));

	QueryFunction := GetProcAddress(g_xigncode_module, PChar(1));

	QueryFunction(@@SysEnterW, XclioFidSysEnterW);

	MultiByteToWideChar(CP_ACP, 0, PChar(license), -1, @buffer, 80);
	MultiByteToWideChar(CP_ACP, 0, PChar(dir), -1, @path, 256);

	res := SysEnterW(@buffer, @path, 0);
	if res = 0
	then begin
		FreeLibrary(g_xigncode_module);
		g_xigncode_module := 0;
		Exit;
	end;

	QueryFunction(@@SysExit, XclioFidSysExit);
	QueryFunction(@@SendCommandVa, XclioFidSendCommandVa);
	QueryFunction(@@ProbeEx, XclioFidProbeEx);
	QueryFunction(@@Init, XclioFidInit);
	QueryFunction(@@Cleanup, XclioFidCleanup);

	Init();

	res := ZCWAVE_SetUserInfoCallbackA(SendCommandVa, cb);
	if res = 0
	then begin
		ZCWAVE_SysExit();
		Exit;
	end;

	Result := True;
end;

end.

