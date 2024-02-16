#pragma once
#include "../Hooks/Hooks.h"

class CModuleEntry
{
public:
	void Load();
	void Unload();
public:
	bool GetD3D9Device(void** pTable, size_t size);
	typedef HRESULT(__stdcall* EndScene)(LPDIRECT3DDEVICE9 pDevice);
	typedef HRESULT(__thiscall* Reset)(void*, LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS params);
	void UnloadDevice();
	void DisableHooks();
public:
	bool ShouldUnload();
public:
	void ConsoleLoad(FILE* f);
	void ConsoleUnload(FILE* f);
public:
	void LoadImGui(LPDIRECT3DDEVICE9 device, HWND window);
	void CreateImGuiMenu();
	void UnloadImGui();
public:
	int EndSceneHook = 42;
	int ResetHook = 16;
private:
	bool m_bShouldUnload = false;
private:
	IDirect3D9* g_pD3D = nullptr;
	LPDIRECT3DDEVICE9 g_pD3Device = nullptr;
	D3DPRESENT_PARAMETERS g_pD3DParams{};
};

inline CModuleEntry g_ModuleEntry;