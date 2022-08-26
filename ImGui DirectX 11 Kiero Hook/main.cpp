#include "includes.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
static uintptr_t imageBase;

inline void safeJMP(injector::memory_pointer_tr at, injector::memory_pointer_raw dest, bool vp = true)
{
	MH_Initialize();
	MH_CreateHook((void*)at.as_int(), (void*)dest.as_int(), nullptr);
	MH_EnableHook((void*)at.as_int());
}

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("ImGui Window");
	if (ImGui::Button("Toggle Freeze Cam")) {
		if (*(byte*)(imageBase + 0xE9936) != (byte)1) {
			injector::WriteMemory<BYTE>(imageBase + 0xE9936, 0x01, true);
		}
		else {
			injector::WriteMemory<BYTE>(imageBase + 0xE9936, 0x00, true);
		}
	}
	ImGui::End();
	ImGui::Render();

	
	

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

static int returnTrue() {
	return 1;
}

static void hoodcamFunc() {
	uintptr_t node = imageBase + 0x1F52470; //base ptr for coords

	//calc pointers for rest of the coords
	float calcX = *(float*)(node);
	float calcY = *(float*)(node + 4);
	float calcZ = *(float*)(node + 8); 

	//offset values for hoodcam :>
	float offsetval = 0;
	float offsetval2 = 0;
	float offsetval3 = 10.5;

	//multilevel pointer base for player coords, spaghetti code
	uintptr_t node2 = *(uintptr_t*)(imageBase + 0x1F52270);
	node2 = *(uintptr_t*)(node2 + 0x8);
	node2 = *(uintptr_t*)(node2 + 0x8);
	node2 = *(uintptr_t*)(node2);

	float carx = *(float*)(node2 + 0x1D4);
	float cary = *(float*)(node2 + 0x1D8);
	float carz = *(float*)(node2 + 0x1DC);

	calcX = carx + offsetval;
	calcY = cary + offsetval3;
	calcZ = carz + offsetval2;


	injector::WriteMemory<FLOAT>(imageBase + 0x1F52470, calcX, true);
	injector::WriteMemory<FLOAT>(imageBase + 0x1F52474, calcY, true);
	injector::WriteMemory<FLOAT>(imageBase + 0x1F52478, calcZ, true);
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		imageBase = (uintptr_t)GetModuleHandleA(0);
		safeJMP(imageBase + 0xE995D, hoodcamFunc, false);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}