========================================================================
    Dark GDK - Game : DXWindowsGrabbing Project Overview
========================================================================

Dark GDK - Game has created this DXWindowsGrabbing project for you as a starting point.

This file contains a summary of what you will find in each of the files that make up your project.

Dark GDK - Game.vcproj
    This is the main project file for projects generated using an Application Wizard. 
    It contains information about the version of the product that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

Dark GDK - The Game Creators - www.thegamecreators.com

/////////////////////////////////////////////////////////////////////////////
Other notes:

/////////////////////////////////////////////////////////////////////////////

Using http://msdn.microsoft.com/en-us/library/ms633497(VS.85).aspx
I can get the list of windows active on the desktop.
This can be useful for selecting which window to record the video from.

/////////////////////////////////////////////////////////////////////////////

Some declarations

+ Code Snippet

IDirect3DSurface9 *        screenGrabSourceSurface;
IDirect3DSurface9 *        screenGrabLockableSurface;

HBITMAP                    screenBitmap;
void *                     screenBitmapData;
int                        screenBitmapScanLineSize;
HDC                        captureDC;


Creating surface and render target

+ Code Snippet
   
if( FAILED(g_pD3DDevice->CreateRenderTarget(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0,false, &screenGrabSourceSurface, NULL)) )
      return E_UNEXPECTED;

if( FAILED(g_pD3DDevice->CreateOffscreenPlainSurface(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &screenGrabLockableSurface, NULL)) )
      return E_UNEXPECTED;


Grab Backbuffer

+ Code Snippet

IDirect3DDevice9* pd3dDevice = g_pD3DDevice;

IDirect3DSurface9* pRenderTargetSurface = NULL;
if( FAILED(pd3dDevice->GetRenderTarget(0, &pRenderTargetSurface)) )
      { assert( false ); return; }


if( FAILED(pd3dDevice->StretchRect(pRenderTargetSurface, NULL, screenGrabSourceSurface, NULL, D3DTEXF_NONE)) )
      { assert( false ); return; }
SAFE_RELEASE( pRenderTargetSurface );

if( FAILED(pd3dDevice->GetRenderTargetData(screenGrabSourceSurface, screenGrabLockableSurface)) )
      { assert( false ); return; }



Save the data from backbuffer to color array

+ Code Snippet

     //Create a lock on the DestinationTargetSurface
      D3DLOCKED_RECT lockedRC;
      if( FAILED( screenGrabLockableSurface->LockRect( &lockedRC, NULL, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY ) ) )
      { assert( false ); return 0; }

      LPBYTE lpSource = reinterpret_cast<LPBYTE>(lockedRC.pBits);

      LPBYTE lpDestination = (LPBYTE)screenBitmapData;
      LPBYTE lpDestinationTemp = lpDestination;

      // Copy the data
      lpDestinationTemp += (bbSurfDesc.Height-1) * screenBitmapScanLineSize;
      for ( unsigned int iY = 0; iY < bbSurfDesc.Height; ++iY )
      {
         for ( unsigned int iX = 0; iX < bbSurfDesc.Width; ++iX )
         {
            UINT pixel = ((UINT*)lpSource)[iX];
            lpDestinationTemp[iX*3 + 0] = (BYTE)((pixel >> 0) & 0xFF);
            lpDestinationTemp[iX*3 + 1] = (BYTE)((pixel >> 8) & 0xFF);
            lpDestinationTemp[iX*3 + 2] = (BYTE)((pixel >> 16) & 0xFF);
         }

         lpSource += lockedRC.Pitch;
         lpDestinationTemp -= screenBitmapScanLineSize;
      }


      //Unlock the rectangle
      if( FAILED(screenGrabLockableSurface->UnlockRect()) )
      { assert( false ); return 0; }



Save the data from color array to HBITMAP

+ Code Snippet

   HDC    hdcDesktop = GetDC( GetDesktopWindow() );
   captureDC = CreateCompatibleDC( hdcDesktop );
   ReleaseDC( GetDesktopWindow(), hdcDesktop );

   BITMAPINFO*    pbmpInfo = (BITMAPINFO*)new unsigned char[sizeof( BITMAPINFOHEADER ) + sizeof( unsigned short ) * 256];

   BITMAPINFO& bmpInfo = (*pbmpInfo );
   ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
   bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
   bmpInfo.bmiHeader.biBitCount=24;
   bmpInfo.bmiHeader.biCompression = BI_RGB;
   bmpInfo.bmiHeader.biWidth= bbSurfDesc.Width;
   bmpInfo.bmiHeader.biHeight= (int)bbSurfDesc.Height;
   bmpInfo.bmiHeader.biSizeImage = 0;
   bmpInfo.bmiHeader.biPlanes = 1;
   bmpInfo.bmiHeader.biClrUsed = 0;
   bmpInfo.bmiHeader.biClrImportant = 0;

   screenBitmapScanLineSize = (bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;

   this->screenBitmap = CreateDIBSection( captureDC, pbmpInfo, DIB_RGB_COLORS, (void**)&screenBitmapData, NULL, NULL );

   delete[] pbmpInfo;


This is how I grab the d3d device

+ Code Snippet

g_pGlob->pHoldBackBufferPtr->GetDevice(&g_pD3DDevice);