#include "menu.hpp"

#include "../dependencies/imgui/imgui.h"
#include "../dependencies/imgui/imgui_impl_win32.h"
#include "meowhack_fw/utils/render/render.h"
#include "meowhack_fw/window/window.h"
#include "../features/settings.hpp"

namespace ig = ImGui;

namespace Menu {
    void InitializeContext(HWND hwnd) {
        if (ig::GetCurrentContext( ))
            return;

        ImGui::CreateContext( );
        ImGui_ImplWin32_Init(hwnd);

        render::setup( );

        ImGuiIO& io = ImGui::GetIO( );
        io.IniFilename = io.LogFilename = nullptr;
    }

    void Render( ) {
        fw::window::utils::opened = bShowMenu;

        render::start( );

        fw::window::begin( );
        {
            auto idx = fw::window::tabs({"aimbot", "visuals", "misc", "config"});
            switch (idx) {
                case 0: // aimbot
                    break;
                case 1: // visuals
                    break;
                case 2: // misc
                {
                    fw::window::begin_group_box("exploits", fw::window::top_left, fw::window::half);
                    {
                        fw::window::add_key_bind("speed hack key", &settings::exploits::speed_hack_key);
                        fw::window::add_checkbox("speed hack", &settings::exploits::speed_hack);
                    }
                }
                    break;
                case 3: // config
                    break;
            }
        }
        fw::window::end( );

        render::end( );
        
    }
} // namespace Menu
