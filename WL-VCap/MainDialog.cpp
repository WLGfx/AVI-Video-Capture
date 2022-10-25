#include "MainDialog.h"
#include "resource.h"

// this is global across all cpp files (ref'd in .h)
MainDialog dlMain;

// forward declare the callback function
INT_PTR CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK myTimerProc( HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);

using namespace std;



MainDialog::MainDialog(void)
{
	hWnd		= NULL;
	bRunning	= false;
	vApps.clear();
}

MainDialog::~MainDialog(void)
{
}

static BOOL CALLBACK myEnum(HWND hwnd, LPARAM lParam)
{
	// add the HWND if not the same as this apps
	if ( hwnd != dlMain.hWnd )
	{
		char buff[128];
		
		GetWindowText( hwnd, buff, 128 );
		
		if ( strlen( buff ) > 0  && IsWindowVisible( hwnd ) )
		{
			dlMain.sApps.push_back( buff );
			dlMain.vApps.push_back( hwnd );
		}
	}

	return true;
}

void MainDialog::enumerateApps()
{
	// if the HWND array is used then clear it out
	if ( vApps.size() > 0 )
	{
		vApps.clear();
		sApps.clear();
	}

	EnumWindows( myEnum, NULL );
}

void MainDialog::filllistbox()
{
	// empty the list box
	HWND hListBox = dlMain.hWnd_LB;
	SendMessage( hListBox, CB_RESETCONTENT, 0, 0 );

	// go through the HWND list and get the window titles
	for ( int iCurrent = 0; iCurrent < (int)vApps.size(); iCurrent++ )
	{
		//SendDlgItemMessage( hWnd, IDC_MAIN_APPLIST, LB_ADDSTRING, 0, (LPARAM)sApps[iCurrent].c_str() );
		int sel = SendMessage( hListBox, CB_ADDSTRING, NULL, (LPARAM)sApps[iCurrent].c_str() );
		SendMessage( hListBox, CB_SETITEMDATA, sel, (LPARAM)vApps[iCurrent] );
	}
}

void MainDialog::run()
{
	DialogBox( hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc );
}

static INT_PTR CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			//
			// INIT DIALOG
			//

			dlMain.hWnd = hwnd;

			dlMain.hWnd_LB = GetDlgItem( hwnd, IDC_MAIN_APPLIST );
			dlMain.hWnd_FN = GetDlgItem( hwnd, IDC_MAIN_FILENAME );

			dlMain.enumerateApps();			// store all open app windows HWNDs
			dlMain.filllistbox();

			// setup the fileselector dialog
			memset( &dlMain.ofn, 0, sizeof(OPENFILENAME) );
			dlMain.ofn.lStructSize		= sizeof(OPENFILENAME);
			dlMain.ofn.hwndOwner		= hwnd;
			dlMain.ofn.lpstrFilter		= "AVI Files\0*.avi\0All Files\0*.*\0\0";
			dlMain.ofn.lpstrTitle		= "Save as Avi file";
			dlMain.ofn.lpstrFile		= dlMain.szFile;
			dlMain.ofn.nMaxFile			= sizeof(dlMain.szFile);
			dlMain.ofn.Flags			= OFN_PATHMUSTEXIST;

			SetDlgItemInt( hwnd, IDC_MAIN_MILLISECONDS, dlMain.iFrameTime = 25, 0 );

			return true;
		}

	case WM_COMMAND:

		switch( LOWORD(wParam) )
		{

		case IDC_MAIN_BUTTON_START:
			{
				// Grab the Apps HWND
				int sel = SendMessage( dlMain.hWnd_LB, CB_GETCURSEL, NULL, NULL );
				//HWND hApp = (HWND)SendMessage( dlMain.hWnd_LB, CB_GETITEMDATA, sel, NULL );
				HWND hApp = dlMain.vApps[sel];
				dlMain.hApp = hApp;

				// Test if everything is okay for the next stage
				if ( hApp && strlen( dlMain.szFile ) )
				{
					FILE *fp = fopen( dlMain.szFile, "rb" );
					if ( fp )
					{
						fclose( fp );
						remove( dlMain.szFile );
					}

					dlMain.bAviFirstTime	= true;
					dlMain.bRunning			= true;
					dlMain.bMinimised		= true;

					dlMain.iFrameTime = GetDlgItemInt( dlMain.hWnd, IDC_MAIN_MILLISECONDS, NULL, NULL );

					dlMain.hAvi = CreateAvi( dlMain.szFile, dlMain.iFrameTime, NULL );

					// Set the callback timer to start recording the frames
					dlMain.hTProc = SetTimer( NULL, NULL, dlMain.iFrameTime, (TIMERPROC)myTimerProc );

					// Set the focus on the window that's being recorded
					SetFocus( hApp );
					ShowWindow( hApp, SW_SHOW );
					SetForegroundWindow( hApp );
				}

				return true;
			}

		case IDC_MAIN_BUTTON_CONTINUE:
			{
				ShowWindow( dlMain.hApp, SW_SHOW );
				SetForegroundWindow( dlMain.hApp );
				return true;
			}

		case IDC_MAIN_BUTTON_STOP:
			{
				KillTimer( NULL, dlMain.hTProc );
				CloseAvi( dlMain.hAvi );

				dlMain.bRunning = NULL;
				dlMain.hApp		= NULL;

				return true;
			}

		case IDC_MAIN_BUTTON_REFRESH:
			{
				// refresh the applications list

				dlMain.enumerateApps();
				dlMain.filllistbox();
				
				return true;
			}

		case IDC_MAIN_BUTTON_SELECTFILE:
			{
				GetSaveFileName( &dlMain.ofn );
				FILE *fp = fopen( dlMain.szFile, "rb" );
				if ( fp )
				{
					fclose( fp );
					if ( MessageBox( dlMain.hWnd, "Overwrite existing file?", 
						"File Exists!", MB_OKCANCEL | MB_ICONQUESTION ) == IDOK )
					{
						SetDlgItemText( dlMain.hWnd, IDC_MAIN_FILENAME, dlMain.szFile );
					}
					else
					{
						SetDlgItemText( dlMain.hWnd, IDC_MAIN_FILENAME, "\0" );
						dlMain.szFile[0] = '\0';
					}
				}
				else
				{
					SetDlgItemText( dlMain.hWnd, IDC_MAIN_FILENAME, dlMain.szFile );
					//dlMain.szFile[0] = '\0';
				}

				return true;
			}

		}
		break;

	case WM_CLOSE:
		{
			EndDialog( hwnd, 0 );

			return true;
		}

	case WM_SYSCOMMAND:
		{
			switch ( wParam )
			{
			case SC_MINIMIZE:
				{
				}
			}
		}

	}

	return false;
}

INT_PTR CALLBACK myTimerProc( HWND hwnd, UINT message, UINT idTimer, DWORD dwTime )
{
	if ( dlMain.bRunning && dlMain.bMinimised )
	{
		if ( dlMain.hApp == GetForegroundWindow() )
		{
			HWND hDesktop = GetDesktopWindow();
			HDC hDesktopDC = GetDC( hDesktop );

			RECT rAppRect;
			GetWindowRect( dlMain.hApp, &rAppRect );
			int iAppWid = rAppRect.right - rAppRect.left;
			int iAppHgt = rAppRect.bottom - rAppRect.top;

			HDC MemDC = CreateCompatibleDC( hDesktopDC );

			HBITMAP MemBM;
			
			BITMAPINFO bmh;
			ZeroMemory(&bmh, sizeof(BITMAPINFO));
			bmh.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
			bmh.bmiHeader.biWidth			= iAppWid;
			bmh.bmiHeader.biHeight			= iAppHgt;
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

			BitBlt( MemDC, 0, 0, iAppWid, iAppHgt, hDesktopDC, rAppRect.left, rAppRect.top, SRCCOPY );

			ReleaseDC( hDesktop, hDesktopDC );

			if ( dlMain.bAviFirstTime )
			{
				dlMain.bAviFirstTime = false;

				memset(&dlMain.opts, 0, sizeof(AVICOMPRESSOPTIONS));
				dlMain.opts.fccHandler = mmioFOURCC('m','s','s','2');

				SetAviVideoCompression( dlMain.hAvi, MemBM, &dlMain.opts, true, dlMain.hApp );
			}

			// add the frame to the file
			AddAviFrame(dlMain.hAvi, MemBM);

			DeleteObject( MemBM );
			DeleteDC( MemDC );
			//ReleaseDC( hDesktop, hDesktopDC );
		}
	}
	return true;
}