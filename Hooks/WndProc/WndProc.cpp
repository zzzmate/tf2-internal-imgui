#include "WndProc.h"

using namespace Hooks;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc::Detour(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bShowMenu && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcW(oWndProc, hwnd, uMsg, wParam, lParam);
}

void WndProc::Initialize()
{
	while (!hwGame) 
	{
		hwGame = FindWindowW(L"Valve001", nullptr);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongW(hwGame, GWL_WNDPROC, reinterpret_cast<LONG>(Detour)));
}

void WndProc::Uninitialize()
{
	if (oWndProc)
		SetWindowLongW(hwGame, GWL_WNDPROC, reinterpret_cast<LONG>(oWndProc));
}