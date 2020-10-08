// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// add headers that you want to pre-compile here
#include <string>
#include <vector>
#include <stdexcept>
#include <system_error>


#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11_1.h>
#pragma comment(lib, "d3d11")
#include <d2d1effects.h>
#include <d2d1effecthelpers.h>
#include <dwrite_1.h>
#pragma comment(lib, "dwrite")

#include <wrl/client.h>
#include <wincodec.h>
#include <comdef.h>
#include <shobjidl.h> 
#include <atlbase.h> 

using Microsoft::WRL::ComPtr;

#endif //PCH_H
