#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <libloaderapi.h>
#include <cmath>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "kiero/minhook/include/MinHook.h"
#include "kiero/injector/injector.hpp"
#include "doomer/mathTools.h"
#include "wtypes.h"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;