#pragma once

#define NOMINMAX

#if _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

//
#include <Windows.h>
#include <assert.h>
//
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <map>
#include<set>

//다이렉트 추가.
#pragma comment(lib, "assimp.lib")


#include <d3dcompiler.h>
#include <d3d11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Setting.h"

#include "Utility/keyManager.h"
#include "singletonBox.h"

#include <windows.h>
#include <shobjidl.h>

using namespace std;
//라이브러리 추가

#define KEYMANAGER keyManager::getSingleton()
#define BOX singletonBox::getSingleton()

//
#define SAFE_DELETE(p)		{ if(p) { delete (p);		(p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);		(p) = nullptr; } }
#define SAFE_RELEASE(p)		{ if(p) { (p)->Release();	(p) = nullptr; } }
//



#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
//#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate )

static const float identityMatrix[16] =
{ 
    1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f 
};

static float cameraView[16] =
{
    1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  0.f, 0.f, 0.f, 1.f 
};

static float cameraProjection[16];

static float objectMatrix[16] =
{1.f, 0.f, 0.f, 0.f,
 0.f, 1.f, 0.f, 0.f,
 0.f, 0.f, 1.f, 0.f,
 0.f, 0.f, 0.f, 1.f };


static float dog[16] =
{ 0.1f, 0.f, 0.f, 0.f,
 0.f, 0.1f, 0.f, 0.f,
 0.f, 0.f, 0.1f, 0.f,
 0.f, 0.f, 0.f, 0.1f };


