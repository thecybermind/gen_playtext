#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define LIB_PT_IPC 82385

int main(int argc, char* argv[]) {
	HWND hWinamp = FindWindow("Winamp v1.x", NULL);
	COPYDATASTRUCT cds;
	memset(&cds, 0, sizeof(COPYDATASTRUCT));

	if (!hWinamp) {
		printf("Unable to find Winamp window\n");
		return 1;
	}

	char input[256];
	memset(input, 0, sizeof(input));

	if (argc == 1) {
		printf("Enter search string: ");
		gets(input);
	} else {
		strncpy(input, argv[1], sizeof(input));
	}

	input[sizeof(input)-1] = '\0';

	COPYDATASTRUCT cds;
	cds.dwData = LIB_PT_IPC;
	cds.lpData = input;
	cds.cbData = strlen(input) + 1;
	printf("Searching for \"%s\"\n", input);

	SendMessage(hWinamp, WM_COPYDATA, (WPARAM)hWinamp, (LPARAM)&cds);

	printf("Done.\n");

	return 0;
}