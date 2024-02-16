#include "Entry.h"
#define _VALIDATE(x) if (!x) { Error(#x); m_bShouldUnload = true; return; }
CModuleEntry::EndScene EndScene_o;
CModuleEntry::Reset Reset_o;
void* d3d9Device[119];

bool CModuleEntry::GetD3D9Device(void** pTable, size_t size)
{
	if (!pTable) return false;

	g_pD3D = Direct3DCreate9(32); // d3d_sdk or some shit
	if (!g_pD3D) return false;

	CModuleEntry::g_pD3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	CModuleEntry::g_pD3DParams.hDeviceWindow = Hooks::WndProc::hwGame;
	CModuleEntry::g_pD3DParams.Windowed = TRUE;

	g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_pD3DParams.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_pD3DParams, &g_pD3Device);
	//g_pD3D->CreateDevice(
		//D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_pD3DParams.hDeviceWindow,
		//D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_pD3DParams, &g_pD3Device);

	if (!g_pD3Device) { g_pD3D->Release();  return false; }

	memcpy(pTable, *reinterpret_cast<void***>(g_pD3Device), size);

	return true;
}

void CModuleEntry::LoadImGui(LPDIRECT3DDEVICE9 device, HWND window)
{
	//HRESULT hr = pDevice->QueryInterface(IID_IDirect3DDevice9, (void**)&m_pDirectPointer); // no error check cus swag

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void CModuleEntry::CreateImGuiMenu()
{
	//MessageBox(0, L"ImGui", L"sex", 0);
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hello world!", &Hooks::WndProc::m_bShowMenu);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CModuleEntry::UnloadImGui()
{
	ImGui::EndFrame();
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

HRESULT __stdcall HookEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	static bool init = false;

	if (GetAsyncKeyState(VK_INSERT) & 1)
		Hooks::WndProc::m_bShowMenu = !Hooks::WndProc::m_bShowMenu;

	if (!init)
	{
		//MessageBox(0, L"EndScene", L"sex", 0);

		g_ModuleEntry.LoadImGui(pDevice, Hooks::WndProc::hwGame);
		init = true;
	}

	if (init)
		if(Hooks::WndProc::m_bShowMenu)
			g_ModuleEntry.CreateImGuiMenu();

	return EndScene_o(pDevice);
}

HRESULT __stdcall HookReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS params)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();
	return Reset_o(pDevice, pDevice, params);
}

void CModuleEntry::Load()
{
	while (!GetModuleHandleW(L"mss32.dll"))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	MathLib_Init();

	_VALIDATE(g_Offsets.Initialize());
	_VALIDATE(g_Interfaces.Initialize());

	_VALIDATE(G::ConVars.Initialize());
	_VALIDATE(G::Draw.Initialize());

	G::Draw.ReloadFonts();
	G::Draw.ReloadScreenSize();

	if (I::EngineClient->IsInGame())
		g_Globals.m_nLocalIndex = I::EngineClient->GetLocalPlayer();

	_VALIDATE(Hooks::Initialize());

	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
	{
		MH_CreateHook(d3d9Device[EndSceneHook], &HookEndScene, (LPVOID*)reinterpret_cast<EndScene*>(&EndScene_o));
		MH_EnableHook(d3d9Device[EndSceneHook]);

		MH_CreateHook(d3d9Device[ResetHook], &HookReset, (LPVOID*)reinterpret_cast<Reset*>(&Reset_o));
		MH_EnableHook(d3d9Device[ResetHook]);
	}

	//I::EngineClient->ClientCmd_Unrestricted("r_drawtracers_firstperson 1");
}

void CModuleEntry::ConsoleLoad(FILE* f)
{
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
}

void CModuleEntry::ConsoleUnload(FILE* f)
{
	if (f) fclose(f);
	FreeConsole();
}

void CModuleEntry::Unload()
{
	CModuleEntry::DisableHooks();

	_VALIDATE(Hooks::UnInitialize());

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	G::Draw.UnInitialize();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	CModuleEntry::UnloadImGui();
	CModuleEntry::UnloadDevice();
}

void CModuleEntry::UnloadDevice()
{
	if (g_pD3Device)
	{
		g_pD3Device->Release();
		g_pD3Device = nullptr;
	}

	if (g_pD3D)
		g_pD3D->Release(); // d3d already nullptr if the device is nullptr
}

void CModuleEntry::DisableHooks()
{
	// endscene hook
	MH_DisableHook(d3d9Device[EndSceneHook]);
	MH_RemoveHook(d3d9Device[EndSceneHook]);
	// reset hook
	MH_DisableHook(d3d9Device[ResetHook]);
	MH_RemoveHook(d3d9Device[ResetHook]);
}

bool CModuleEntry::ShouldUnload()
{
	if (GetAsyncKeyState(VK_F11))
		m_bShouldUnload = true;

	return m_bShouldUnload;
}