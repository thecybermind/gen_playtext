gen_playtext - by CyberMind (cybermind@gmail.com)

When this plugin receives a specially-crafted IPC, it will clear the playlist and load only files that match the search query.
This plugin requires the Media Library and the search behaves the same as the ML search or the JTFE search.

To send data to this plugin, send a WM_COPYDATA message to the Winamp hWnd:

	#define LIB_PT_IPC 82385

	COPYDATASTRUCT cds;
	cds.dwData = LIB_PT_IPC;
	cds.lpData = text;
	cds.cbData = strlen(text) + 1;

	SendMessage(hWinamp, WM_COPYDATA, (WPARAM)hWinamp, (LPARAM)&cds);
