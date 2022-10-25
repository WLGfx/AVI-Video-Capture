#include "DarkGDK.h"
#include "globstruct.h"

#include <windows.h>
#include <vfw.h>
#include <d3d9.h>
#include <stdio.h>

#include "AviFile.h"

int		VR_INIT();
void	VR_WRITE();
void	VR_CLOSE();
void	VR_WRITE_NEW();

extern GlobStruct* g_pGlob;

#pragma comment(lib, "vfw32.lib")

void DarkGDK ( void )
{
	dbAutoCamOff();
	dbSyncOn();
	dbSyncRate(60);
	//dbSetDisplayMode(640, 480, 32);

	dbMakeObjectCube(1, 10);
	dbMakeObjectCylinder(2, -15);
	dbColorObject(2, 0xff0000);
	dbPositionCamera(0,0,-14);

	dbSync();

	if (VR_INIT()) return;
	int count = 0;

	while ( LoopGDK() && !dbSpaceKey() )
	{
		dbYRotateObject(1, fmod(dbObjectAngleY(1) + 0.7f, 360.0f));
		dbXRotateObject(1, fmod(dbObjectAngleX(1) + 0.9f, 360.0f));
		dbYRotateObject(2, fmod(dbObjectAngleY(1) + 0.9f, 360.0f));
		dbXRotateObject(2, fmod(dbObjectAngleX(1) + 0.7f, 360.0f));
		dbSync();

		count = ( count + 1 ) % 3;
		if ( !count )
			VR_WRITE_NEW();
	}

	VR_CLOSE();

	return;
}

//
// Video Recorder
//

HAVI hAviFile;
AVICOMPRESSOPTIONS opts;
bool bAviFirstTime;
char cAviFileName[] = "test.avi";

int VR_INIT()
{
	FILE *fp = fopen(cAviFileName, "rb");

	if ( !fp )
	{
		remove(cAviFileName);
	}
	else
		fclose(fp);

	bAviFirstTime = true;
	hAviFile = CreateAvi(cAviFileName,25,NULL);

	return 0;
}

void VR_WRITE()
{
	// First of all grab the image from the screen
	IDirect3DSurface9 *SrcSurface = 0; 
	D3DLOCKED_RECT SrcRect;          
	D3DSURFACE_DESC SrcDesc;

	int wid = dbScreenWidth();
	int hgt = dbScreenHeight();
	int dep = 32; //dbGetBackbufferDepth();

	
	IDirect3DDevice9 *screen;
	//screen = dbGetDirect3DDevice();
	g_pGlob->pHoldBackBufferPtr->GetDevice(&screen);
	screen->CreateOffscreenPlainSurface(wid, hgt, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &SrcSurface, NULL);
	//screen->GetFrontBufferData(0, SrcSurface);

	SrcSurface->GetDesc(&SrcDesc);
	SrcSurface->LockRect(&SrcRect, 0, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY);

	int pit = SrcRect.Pitch;

	unsigned char *SrcAddr = (unsigned char*)SrcRect.pBits;

	HDC		hScrDC, hMemDC;
	HBITMAP	hBitmap, hBitmapOld;

	//SrcSurface->GetDC( &hScrDC );
	hScrDC = GetDC( g_pGlob->hWnd );
	hMemDC = CreateCompatibleDC( hScrDC );

	// CreateCompatibleBitmap() just doesn't work at all!
	//hBitmap = CreateCompatibleBitmap(hScrDC, SrcDesc.Width, SrcDesc.Height);
	//SelectObject(hMemDC, hBitmap);

	// TRYING DIB
	BITMAPINFO bmh;
	ZeroMemory(&bmh, sizeof(BITMAPINFO));
	bmh.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmh.bmiHeader.biWidth			= wid;
	bmh.bmiHeader.biHeight			= hgt;
	bmh.bmiHeader.biPlanes			= 1;
	bmh.bmiHeader.biBitCount		= 32;
	bmh.bmiHeader.biCompression		= BI_RGB;
	bmh.bmiHeader.biSizeImage		= wid*hgt*4;
	bmh.bmiHeader.biXPelsPerMeter	= 1000;
	bmh.bmiHeader.biYPelsPerMeter	= 1000;
	bmh.bmiHeader.biClrUsed			= 0;
	bmh.bmiHeader.biClrImportant	= 0;

	void *pbits;
	//char *pbits = new char[wid*hgt*4];
	hBitmap = CreateDIBSection(hMemDC, &bmh, DIB_RGB_COLORS, &pbits, 0, 0);
	//hBitmap = CreateDIBitmap(hMemDC,&bmh.bmiHeader,CBM_INIT,pbits,&bmh,DIB_RGB_COLORS);
	hBitmapOld = (HBITMAP)SelectObject( hMemDC, hBitmap );

	//BitBlt(hMemDC, 0, 0, wid, hgt, hScrDC, 0, 0, SRCCOPY);
	__asm {
		mov		edx, [pbits]	// destination
		mov		ecx, [SrcAddr]	// source
		mov		eax, [hgt]		// height of bitmap
_lhgt:	push	eax				// save hgt counter
		push	ecx				// save source address
		mov		ebx, [wid]		// pixel count across
_lwid:	mov		eax, [ecx]		// grab pixel
		mov		[edx], eax		// store in new bitmap
		add		ecx, 4			// next pixel across
		add		edx, 4
		dec		ebx
		jne		_lwid			// continue until end
		pop		ecx				// restore source address
		add		ecx, [pit]		// add pitch
		pop		eax
		dec		eax
		jne		_lhgt			// next line
	}

	hBitmap = (HBITMAP)SelectObject( hMemDC, hBitmapOld );

	if ( bAviFirstTime )
	{
		bAviFirstTime = false;

		memset(&opts, 0, sizeof(AVICOMPRESSOPTIONS));
		opts.fccHandler		= mmioFOURCC('d','i','v','x');

		SetAviVideoCompression( hAviFile, hBitmap, &opts, true, g_pGlob->hWnd );
	}

	// add the frame to the file
	AddAviFrame(hAviFile, hBitmap);

	DeleteObject( hBitmap );

	//SrcSurface->ReleaseDC(hScrDC);
	ReleaseDC( g_pGlob->hWnd, hScrDC );
	DeleteDC( hMemDC );

	// release the surface (give it freedom again)
	SrcSurface->UnlockRect();
	SrcSurface->Release();
	SrcSurface=0;

	// delete the buffer now we're done with it
	//delete pbits;
}

void VR_CLOSE()
{
	CloseAvi( hAviFile );
}

void VR_WRITE_NEW()
{
	HWND DesktopHWND = GetDesktopWindow();
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

		memset(&opts, 0, sizeof(AVICOMPRESSOPTIONS));
		opts.fccHandler		= mmioFOURCC('d','i','v','x');

		SetAviVideoCompression( hAviFile, MemBM, &opts, true, g_pGlob->hWnd );
	}

	// add the frame to the file
	AddAviFrame(hAviFile, MemBM);

	DeleteObject( MemBM );
	DeleteDC( MemDC );
	ReleaseDC( DesktopHWND, DesktopDC );
}