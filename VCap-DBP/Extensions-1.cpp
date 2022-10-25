#include "windows.h"
#include "commctrl.h"
#include "DBP-entry.h"
#include "DBPro.hpp"
#include "d3dx9.h"
#include "stdio.h"

#pragma comment(lib, "vfw32.lib")

// Basic Video Capture plugin for DBP.

// This plugin will allow the user to capture the screen and add frames to an
// encoded compressed video file.

// No audio enabled for this first sample release.

// Misc defines
#define FOURCC(a,b,c,d) ( (DWORD) (((d)<<24) | ((c)<<16) | ((b)<<8) | (a)) )

// Global variables for the basic plugin
GlobStruct*	g_pGlob;

HAVI	hAviFile		= 0;
bool	bAviFirstTime	= 0;
int		iAviFrameTime;

AVICOMPRESSOPTIONS opts = {
	streamtypeVIDEO,				// fccType
	mmioFOURCC('M','S','V','C'),	// fccHandler
	4,								// keyframe rate
	7500,							// compression quality 0-10000
	170 * 1024,						// dwBytesPerSecond 170Kb
	AVICOMPRESSF_VALID,				// dwFlags
	0,0,0,0,0 };					// unused as of yet

EXPORTC DWORD VCap_Init(const char *sFilename, int iMSeconds, int iCompression, DWORD iOverwrite)
{
	if ( !sFilename )
	{
		return 1;			// invalid filename
	}

	FILE* fp;
	if ( fp = fopen( sFilename, "rb" ) )
	{
		fclose( fp );
		if ( iOverwrite )	// allow deletion of old file?
		{
			if ( remove( sFilename ) )
			{
				return 2;	// couldn't delete original file
			}
		}
		else
		{
			return 3;		// file already exists
		}
	}

	if ( iMSeconds < 25 )
	{
		return 4;			// incorrect value for iMSeconds
	}

	if ( iCompression < 0 || iCompression > 10000 )
	{
		return 5;			// incorrect compression value
	}

	opts.dwQuality	= iCompression;
	iAviFrameTime	= iMSeconds;

	hAviFile = CreateAvi( sFilename, iMSeconds, NULL );
	bAviFirstTime = true;

	//DBPro::DeleteString( sFilename );

	return 0;				// successful
}

EXPORTC void VCap_AddWindowFrame()
{
	HWND DesktopHWND;

	DesktopHWND = GetDesktopWindow();
	//DesktopHWND = g_pGlob->hWnd;

	HDC DesktopDC = GetDC( DesktopHWND );

	RECT WindowRect;
	RECT WindowSize;
	GetWindowRect( g_pGlob->hWnd, &WindowRect );
	GetClientRect( g_pGlob->hWnd, &WindowSize );
	int WindowWid = WindowRect.right - WindowRect.left;
	int WindowHgt = WindowRect.bottom - WindowRect.top;
	
	HDC MemDC = CreateCompatibleDC( DesktopDC );

	HBITMAP MemBM;
	
	//MemBM = CreateCompatibleBitmap( MemDC, WindowSize.right, WindowSize.bottom );

	BITMAPINFO bmh;
	ZeroMemory(&bmh, sizeof(BITMAPINFO));
	bmh.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmh.bmiHeader.biWidth			= WindowWid;
	bmh.bmiHeader.biHeight			= WindowHgt;
	bmh.bmiHeader.biPlanes			= 1;
	bmh.bmiHeader.biBitCount		= 32;
	bmh.bmiHeader.biCompression		= BI_RGB;
	bmh.bmiHeader.biSizeImage		= 0;
	bmh.bmiHeader.biXPelsPerMeter	= 1000;
	bmh.bmiHeader.biYPelsPerMeter	= 1000;
	bmh.bmiHeader.biClrUsed			= 0;
	bmh.bmiHeader.biClrImportant	= 0;
	void *pbits;
	MemBM = CreateDIBSection(MemDC, &bmh, DIB_RGB_COLORS, &pbits, 0, 0);

	SelectObject( MemDC, MemBM );

	BitBlt( MemDC, 0, 0, WindowWid, WindowHgt, DesktopDC, WindowRect.left, WindowRect.top, SRCCOPY );

	if ( bAviFirstTime )
	{
		bAviFirstTime = false;

		/*memset(&opts, 0, sizeof(AVICOMPRESSOPTIONS));
		opts.fccHandler		= mmioFOURCC('m','s','v','c');*/

		SetAviVideoCompression( hAviFile, MemBM, &opts, false, g_pGlob->hWnd );
	}

	// add the frame to the file
	AddAviFrame(hAviFile, MemBM);

	DeleteObject( MemBM );
	DeleteDC( MemDC );
	ReleaseDC( DesktopHWND, DesktopDC );
}

EXPORTC void VCap_Close()
{
	CloseAvi( hAviFile );
}
