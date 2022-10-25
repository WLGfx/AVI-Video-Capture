#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <stdio.h>

#include "AviFile.h"

class MainDialog;

extern MainDialog dlMain;			// global reference to the main dialog

class MainDialog
{
public:
	HWND		hWnd;				// main dialogs HWND
	HINSTANCE	hInstance;			// apps hinstance

	HWND		hWnd_LB;			// ListBox HWND
	HWND		hWnd_FN;			// File name HWND

	HAVI		hAvi;				// Avi file handle
	bool		bAviFirstTime;		// First frame added?
	bool		bRunning;			// true if dialog active
	bool		bMinimised;			// true if minimised
	int			iFrameTime;			// Avi frame period
	HWND		hApp;				// Recording apps HWND
	AVICOMPRESSOPTIONS opts;		// compression options
	UINT_PTR	hTProc;				// Timer proc HWND

	OPENFILENAME ofn;				// used by the file selector
	char		szFile[260];		// used to store the file selected

	std::vector<HWND> vApps;		// array of active windows on desktop
	std::vector<std::string> sApps;	// array of active windows titles

	MainDialog(void);
	~MainDialog(void);

	void		enumerateApps();
	void		filllistbox();
	void		run();
};
