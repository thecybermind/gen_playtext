// Winamp general purpose plug-in mini-SDK
// Copyright (C) 1997, Justin Frankel/Nullsoft

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include "..\winamp\wa_ipc.h"
#include "..\gen_ml\ml.h"

typedef struct {
	int version;
	char *description;
	int (*init)();
	void (*config)();
	void (*quit)();
	HWND hwndParent;
	HINSTANCE hDllInstance;
} plugininfo_t;

BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

#define LIB_PT_IPC 82385

long libmlipc = 0;
HWND mlHwnd = 0;

void config();
void quit();
int init();
LRESULT CALLBACK myWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WNDPROC oldWndProc = 0;

plugininfo_t plugin = {
	0x10,
	"cybermind's PlayText Interface",
	init,
	config,
	quit,
};

void runquery(char* text) {
	mlQueryStruct mlQ;
	int i;
	FILE* fdm3u;
	COPYDATASTRUCT cds = { 
			IPC_ENQUEUEFILE,
			13,
			"playtext.m3u"
		};

	if (!libmlipc)
		libmlipc = SendMessage(plugin.hwndParent, WM_WA_IPC, (WPARAM)"LibraryGetWnd", IPC_REGISTER_WINAMP_IPCMESSAGE); 

	if (!libmlipc)
		return;

	if (!mlHwnd)
		mlHwnd = (HWND)SendMessage(plugin.hwndParent, WM_WA_IPC, -1, libmlipc);

	if (!mlHwnd)
		return;

	memset(&mlQ, 0, sizeof(mlQueryStruct));
	mlQ.query = text;

	//run query on string
	if (SendMessage(mlHwnd, WM_ML_IPC, (WPARAM)&mlQ, ML_IPC_DB_RUNQUERY_SEARCH) == 1) {
		//if no results, cancel
		if (!mlQ.results.Size)
			return;

		//clear playlist
		SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_DELETE);

		//create an .m3u and load it
		fdm3u = fopen("playtext.m3u", "w");
		for (i = mlQ.results.Size - 1; i >= 0 ; i--)
		{
			fputs(mlQ.results.Items[i].filename, fdm3u);
			fputc('\n', fdm3u);
		}
		fclose(fdm3u);

		//free up ml query data
		SendMessage(mlHwnd, WM_ML_IPC, (WPARAM)&mlQ, ML_IPC_DB_FREEQUERYRESULTS);

		//enqueue files
		SendMessage(plugin.hwndParent, WM_COPYDATA, (WPARAM)plugin.hwndParent, (LPARAM)&cds);

		//select first song
		SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_SETPLAYLISTPOS);

		SendMessage(plugin.hwndParent,WM_COMMAND,MAKEWPARAM(WINAMP_BUTTON2,0),0);


		//play
		//SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_STARTPLAY);
	}
}

void config() {
	MessageBox(plugin.hwndParent, "To use:\n\nSend a WM_COPYDATA message to Winamp's hWnd:\nwParam = Winamp's hWnd\nlParam = COPYDATASTRUCT:\n\tdwData = 82385\n\tlpData = null-terminated search string\n\tcbData = length of lpData (including terminator)", plugin.description, 0);
}

void quit() {
	SetWindowLong(plugin.hwndParent, GWL_WNDPROC, (long)oldWndProc);
}

int init() {
	oldWndProc = (WNDPROC)SetWindowLong(plugin.hwndParent, GWL_WNDPROC, (long)myWndProc);

	return 0;
}

LRESULT CALLBACK myWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (hwnd == plugin.hwndParent && message == WM_COPYDATA) {
		COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
		if (cds && cds->dwData == LIB_PT_IPC && cds->lpData) {
			runquery((char*)cds->lpData);
			return 0;
		}
	}

	return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
}

__declspec(dllexport) plugininfo_t* winampGetGeneralPurposePlugin() {
	return &plugin;
}

extern plugininfo_t* gen_plugins[256];
typedef plugininfo_t* (*winampGeneralPurposePluginGetter)();