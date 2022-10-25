#include "windows.h"
#include "commctrl.h"
#include "DBP-entry.h"

//
// see: http://forum.thegamecreators.com/?m=forum_view&t=190224&b=18
// for more refernces
//

#pragma comment (lib, "DBProInterface9.0.lib")
#pragma comment (lib, "comctl32.lib")

//GlobStruct*	g_pGlob;
//HMODULE comctr_lib;

void Constructor ( void ) { // Create memory here
	//comctr_lib = LoadLibrary("comctl32.dll");
	InitCommonControls();
	CoInitialize(NULL);
}

void Destructor ( void ) { // Free memory here
	//FreeLibrary(comctr_lib);
}

void ReceiveCoreDataPtr ( LPVOID pCore ) { // Get Core Data Pointer here
	g_pGlob = (GlobStruct*)pCore;
}

// List of DLL's that this plug-in depends on
const char* Dependencies[] =
{
    "DBProCore.dll",
	"DBProFileDebug.dll"
};


EXPORT int GetNumDependencies( void )
{
    return sizeof(Dependencies) / sizeof(const char*);
}


EXPORT char const* GetDependencyID(int ID)
{
    return Dependencies[ID];
}