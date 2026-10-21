#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#pragma comment(linker, "/EXPORT:playwinamp=_playwinamp@24")
#pragma comment(linker, "/EXPORT:playwinamp2=_playwinamp2@24")

typedef unsigned char byte;

#define LIB_PT_IPC 82385
#define LIB_PT2_IPC 82386

extern "C" int __stdcall playwinamp(HWND mWnd, HWND aWnd, char* data, char* parms, BOOL show, BOOL nopause) {

	HWND hWinamp = FindWindow("Winamp v1.x", NULL);
	if (!hWinamp) return 1;

	data[900-1] = '\0';	//just to be safe?

	COPYDATASTRUCT cds;
	memset(&cds, 0, sizeof(COPYDATASTRUCT));
	cds.dwData = LIB_PT_IPC;
	cds.lpData = data;
	cds.cbData = strlen(data) + 1;

	SendMessage(hWinamp, WM_COPYDATA, (WPARAM)mWnd, (LPARAM)&cds);

	return 1;
}

extern "C" int __stdcall playwinamp2(HWND mWnd, HWND aWnd, char* data, char* parms, BOOL show, BOOL nopause) {

	HWND hWinamp = FindWindow("Winamp v1.x", NULL);
	if (!hWinamp) return 1;

	data[900-1] = '\0';	//just to be safe?

	COPYDATASTRUCT cds;
	memset(&cds, 0, sizeof(COPYDATASTRUCT));
	cds.dwData = LIB_PT2_IPC;
	cds.lpData = data;
	cds.cbData = strlen(data) + 1;

	SendMessage(hWinamp, WM_COPYDATA, (WPARAM)mWnd, (LPARAM)&cds);

	return 1;
}
