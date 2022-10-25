#pragma once

#define EXPORT __declspec(dllexport)
#define EXPORTC extern "C" __declspec(dllexport)

#include "DBPro.hpp"
#include "AviFile.h"

extern GlobStruct*	g_pGlob;