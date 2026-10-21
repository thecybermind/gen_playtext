// Winamp general purpose plug-in mini-SDK
// Copyright (C) 1997, Justin Frankel/Nullsoft

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\winamp\wa_ipc.h"

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

#define LIB_PT2_IPC 82386

void config();
void quit();
int init();
void runquery(char* text);
void runsearch(char* text);
void makelcase(char* str);
LRESULT CALLBACK myWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WNDPROC oldWndProc = 0;

plugininfo_t plugin = {
	0x10,
	"cybermind's PlayText Interface v2",
	init,
	config,
	quit,
};

#define MAX_SONG_LEN 128
typedef struct {
	char name[MAX_SONG_LEN];
} song_t;
song_t* songlist = NULL;
int numsongs = 0;

//will make string lowercase in-place
void makelcase(char* str) {
	while (*str) {
		*str = tolower(*str);
		++str;
	}
}

void runquery(char* text) {
	int i;

	//get length of playlist
	numsongs = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GETLISTLENGTH);

	//allocate space for it
	songlist = (song_t*)malloc(numsongs * sizeof(song_t));

	//store each song name
	for (i = 0; i < numsongs; i++) {
		//get song name
		char* name = (char*)SendMessage(plugin.hwndParent, WM_WA_IPC, i, IPC_GETPLAYLISTTITLE);
		if (!name) continue;

		//save in allocated list
		strncpy(songlist[i].name, name, MAX_SONG_LEN);
		songlist[i].name[MAX_SONG_LEN-1] = '\0';	//null terminate
		makelcase(songlist[i].name);			//tolower
	}

	runsearch(text);

	free(songlist);
	songlist = NULL;
}

void runsearch(char* text) {
	int i = 0;		//iterator for song array
	int j = 0;		//iterator for word array
	int numwords = 1;	//include last word already
	char* p = text;		//iterator
	char** words = NULL;	//small array pointing to start of words in 'text'
	int foundsong = 0;	//flag

	makelcase(text);	//tolower

	//count spaces to allocate small array
	while (*p) {
		if (*p == ' ')
			++numwords;
		++p;
	}

	//allocate array
	words = (char**)malloc(sizeof(char*) * numwords);
	
	//store first
	words[j++] = text;

	//begin storing rest of words
	p = text;
	while (*p) {
		if (*p == ' ') {
			*p = '\0';
			words[j++] = p+1;
		}
		++p;
	}

	//so now words is a list of individual words in the query string
	//begin searching each song for the first word
	//if it exists, search for each subsequent word
	//if one word does not exist, skip to next song
	for (i = 0; i < numsongs; i++) {
		foundsong = 1;	//assume we find match

		//search for each word in the song name
		for (j = 0; j < numwords; j++) {
			if (!strstr(songlist[i].name, words[j])) {
				foundsong = 0;	//didn't find match
				break;
			}
		}
		//found all words
		if (foundsong) {
			SendMessage(plugin.hwndParent, WM_WA_IPC, i, IPC_SETPLAYLISTPOS);

			SendMessage(plugin.hwndParent, WM_COMMAND, MAKEWPARAM(WINAMP_BUTTON2,0), 0);
			break;
		}
	}

	free(words);
	words = NULL;
}

void config() {
	MessageBox(plugin.hwndParent, "To use:\n\nSend a WM_COPYDATA message to Winamp's hWnd:\nwParam = Winamp's hWnd\nlParam = COPYDATASTRUCT:\n\tdwData = 82386\n\tlpData = null-terminated search string\n\tcbData = length of lpData (including terminator)", plugin.description, 0);
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
		if (cds && cds->dwData == LIB_PT2_IPC && cds->lpData && *(char*)cds->lpData) {
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