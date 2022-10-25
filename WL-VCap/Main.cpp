#include <windows.h>
#include <commctrl.h>

#include "AviFile.h"
#include "MainDialog.h"

#pragma comment (lib, "comctl32.lib")

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	InitCommonControls();

	// Lets get the dialog running
	dlMain.hInstance = hInstance;
	dlMain.run();

	return 0;
}