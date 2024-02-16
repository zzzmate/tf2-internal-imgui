#pragma once

#include <thread>
#include <vector>
#include <map>

#include "Offsets/Offsets.h"
#include "Hook/Hook.h"
#include "VFunc/VFunc.h"

#include "../Util/ImGui/imgui_impl_dx9.h"
#include "../Util/ImGui/imgui_impl_win32.h"
#include "../Util/ImGui/imgui.h"

#include <d3d9.h>
#include <d3dx9.h>

#define _(x) x