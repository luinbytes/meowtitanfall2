#pragma once
#include "../utils/math/color.h"
#include "../utils/math/vector.h"
#include "../utils/render/render.h"
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
#include <format>
#include <Windows.h>
#include <WinUser.h>
#include "../../../dependencies/imgui/imgui.h"
#include "../key_bind_t.hpp"

namespace fw {
    namespace window {
        namespace utils
        {
            inline bool opened = false;
            inline float alpha = 0.f;
            inline vector2f position = vector2f(125.f, 125.f);
            inline vector2f size = vector2f(560.f, 370.f);
            inline float header_height = 35.f;
            inline int active_tab = 0;
            inline vector2f offset = vector2f(0.f, 0.f);
            inline vector2f next_group_box_size = vector2f(0.f, 0.f);
            inline size_t hash = 0;
            inline bool skip = false;
            inline color copied_color = color::white();

            template <typename T>
            T map_number(const T& input, const T& input_min, const T& input_max, const T& output_min, const T& output_max) {
                return (input - input_min) / (input_max - input_min) * (output_max - output_min) + output_min;
            }

            std::string trim_name(std::string_view name) {
                if (name.find('#') > 0)
                    return std::string(name.substr(0, name.find('#')));
                else
                    return std::string(name);
            }
        }

        namespace colors {
            color accent = color(255, 200, 255);
            color background = color(15, 15, 15);
            color outline = color(30, 30, 30);
            color outerline = color(8, 8, 8);
            color disabled_text = color(120, 120, 120);
            color active_text = color(190, 190, 190);
            color hovered_disabled = color(150, 150, 150);
            color shadow = color(12, 12, 12);
            color group_box_background = color(18, 18, 18);
            color header = color(180, 180, 180);
        }

        struct drag_info_t {
            vector2f click_position;
            vector2f mouse_pos;
            bool is_dragging;
        };

        void handle_movement(vector2f& position, vector2f item_size, vector2f work_size, drag_info_t& drag_info, float clamp_offset) {
            if (ImGui::IsMouseHoveringRect({ position.x, position.y }, { position.x + item_size.x, position.y + item_size.y }, false)
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                drag_info.is_dragging = true;
                drag_info.click_position = position;
                drag_info.mouse_pos = vector2f(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
            }

            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                drag_info.is_dragging = false;
            }

            if (drag_info.is_dragging) {
                auto display_size = vector2f(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
                auto new_pos = drag_info.click_position + vector2f(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y) - drag_info.mouse_pos;
                new_pos.x = std::clamp<float>(new_pos.x, clamp_offset, display_size.x - work_size.x - clamp_offset);
                new_pos.y = std::clamp<float>(new_pos.y, clamp_offset, display_size.y - work_size.y - clamp_offset);
                position = new_pos;
            }
        }


        int tabs(std::vector<std::string> tabs) {
            int offset = 4;
            for (auto tab : tabs)
                offset += render::text_size(render::size_16, tab).x + 8;

            static std::map<int, float> hover_map;
            static std::map<int, float> active_map;

            int new_offset = 0;
            auto tab = [&](std::string text, vector2f pos, const vector2f& size, bool active) -> bool {
                auto text_pos = pos + vector2f(2.f, (size.y / 2.f) - render::text_size(render::size_16, text).y / 2.f);
                render::add_text(render::size_16, text, text_pos, colors::disabled_text.adjust_alpha(utils::alpha));

                auto h = hover_map.find(std::hash<std::string>()(text));
                if (h == hover_map.end()) {
                    hover_map.insert({ std::hash<std::string>()(text), static_cast<float>(active) });
                    h = hover_map.find(std::hash<std::string>()(text));
                }

                auto a = active_map.find(std::hash<std::string>()(text));
                if (a == active_map.end()) {
                    active_map.insert({ std::hash<std::string>()(text), static_cast<float>(active) });
                    a = active_map.find(std::hash<std::string>()(text));
                }

                a->second = std::lerp(a->second, active, 10.f * ImGui::GetIO().DeltaTime);

                bool hovering = ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, false);
                h->second = std::lerp(h->second, hovering, 10.f * ImGui::GetIO().DeltaTime);
                render::add_text(render::size_16, text, text_pos, colors::hovered_disabled.adjust_alpha(utils::alpha * h->second));
                render::add_text(render::size_16, text, text_pos, colors::accent.adjust_alpha(utils::alpha * a->second));

                new_offset += render::text_size(render::size_16, text).x + 8;

                return hovering && ImGui::IsMouseClicked(0);
                };

            for (int i = 0; i < tabs.size(); i++) {
                auto _p = utils::position + vector2f(utils::size.x, 5.f) - vector2f(offset, 0.f);

                if (tab(tabs.at(i), _p + vector2f(new_offset, 0.f), vector2f(render::text_size(render::size_16, tabs.at(i)).x + 4, utils::header_height - 10.f), utils::active_tab == i)) {
                    utils::active_tab = i;
                }
            }

            return utils::active_tab;
        }

        void begin() {

            utils::alpha = std::lerp(utils::alpha, utils::opened, 20.f * ImGui::GetIO().DeltaTime);

            if (utils::alpha <= 0.f)
                return;

            float rounding = 2.f;

            static drag_info_t d_info;

            vector2f header_size = vector2f(utils::size.x, utils::header_height);
            handle_movement(utils::position, vector2f(header_size.x - 200.f, utils::header_height), utils::size + vector2f(0.f, 15.f + utils::header_height), d_info, 10.f);

            vector2f header_position = utils::position;
            float offset = header_size.y / 2.f - render::text_size(render::bold_16, "meow hack").y / 2.f;
            render::add_shadow_rect(header_position, header_size, colors::accent.adjust_alpha(utils::alpha), 65.f * utils::alpha, rounding);
            auto new_pos = utils::position + vector2f(0.f, 15.f + utils::header_height);

            render::add_shadow_rect(new_pos, utils::size, colors::accent.adjust_alpha(utils::alpha), 65.f * utils::alpha, rounding);
            
            render::add_rect_fill(header_position, header_size, colors::background.adjust_alpha(utils::alpha), rounding);
            render::begin_clip_rect(header_position + vector2f(4.f, 0.f), vector2f(120.f, header_size.y));
            render::add_shadow_rect(header_position, vector2f(render::text_size(render::bold_16, "meow hack").x + (offset * 2.f),
                header_size.y), colors::shadow.adjust_alpha(utils::alpha), 60.f);
            render::end_clip_rect();
            render::add_rect_fill(header_position, vector2f(render::text_size(render::bold_16, "meow hack").x + (offset * 2.f),
                header_size.y), colors::outline.adjust_alpha(utils::alpha), rounding, ImDrawFlags_RoundCornersLeft);
            render::add_rect(header_position, header_size, colors::outline.adjust_alpha(utils::alpha), rounding);
            render::add_text(render::bold_16, "meow hack", header_position + offset, colors::accent.adjust_alpha(utils::alpha));

            render::add_rect_fill(new_pos, utils::size, colors::background.adjust_alpha(utils::alpha), rounding);
            render::add_rect(new_pos - 1.f, utils::size + 2.f, colors::outline.adjust_alpha(utils::alpha), rounding);

        }

        void end() {
            utils::skip = false;
        }

        enum group_box_positions : int {
            top_left, top_right,
            bot_left, bot_right
        };

        enum group_box_sizes : int {
            full, half
        };

        void begin_group_box(const std::string& header, group_box_positions pos, group_box_sizes size) {
            auto new_pos = utils::position + vector2f(0.f, 15.f + utils::header_height);
            vector2f position = new_pos + 10.f;
            vector2f gb_size = utils::size / 2.f;

            switch (pos) {
            case top_left:
                break;
            case top_right: position.x += utils::size.x / 2.f - 5.f;
                break;
            case bot_left: position.y += utils::size.y / 2.f - 5.f;
                break;
            case bot_right: position.x += utils::size.x / 2.f - 5.f; position.y += utils::size.y / 2.f - 5.f;
                break;
            }

            switch (size) {
            case full: gb_size.y = utils::size.y - 20.f;
                break;
            case half:
                gb_size.y = (utils::size.y - 30.f) / 2.f;
                break;
            }

            gb_size.x = (utils::size.x - 30.f) / 2.f;

            render::add_shadow_rect(position, gb_size, colors::shadow.adjust_alpha(utils::alpha), 35.f, 2.f, vector2f(1.f, 1.f));
            render::add_rect_fill(position, gb_size, colors::group_box_background.adjust_alpha(utils::alpha), 2.f);
            render::add_rect_fill(position - 1.f, vector2f(gb_size.x, 30.f) + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f, ImDrawFlags_RoundCornersTop);
            render::add_rect(position - 1.f, gb_size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);
            float offset = 15.f - render::text_size(render::bold_16, header).y / 2.f;
            render::add_text(render::bold_16, header, position + offset, colors::header.adjust_alpha(utils::alpha));

            utils::offset = position + vector2f(10.f, 40.f);
            utils::next_group_box_size = gb_size;
        }

        void end_group_box();

        void add_checkbox(const std::string& label, bool* var) {
            auto pos = utils::offset;
            auto size = vector2f(12.f);

            static std::map<int, float> hover_map;
            static std::map<int, float> active_map;

            auto h = hover_map.find(std::hash<std::string>()(label));
            if (h == hover_map.end()) {
                hover_map.insert({ std::hash<std::string>()(label), 0.f });
                h = hover_map.find(std::hash<std::string>()(label));
            }

            auto a = active_map.find(std::hash<std::string>()(label));
            if (a == active_map.end()) {
                active_map.insert({ std::hash<std::string>()(label), static_cast<float>(*var) });
                a = active_map.find(std::hash<std::string>()(label));
            }

            a->second = std::lerp(a->second, *var, 10.f * ImGui::GetIO().DeltaTime);

            bool hovered = !utils::skip && !utils::hash && ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x + 8.f + render::text_size(render::size_16, label).x, pos.y + size.y }, false);
            if (hovered && ImGui::IsMouseClicked(0)) {
                *var = !*var;
            }
            h->second = std::lerp(h->second, hovered or *var, 10.f * ImGui::GetIO().DeltaTime);

            render::add_shadow_rect(pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(pos, size, colors::accent.adjust_alpha(utils::alpha * 0.6f * h->second), 15.f, 2.f);
            render::add_rect_fill(pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);
            render::add_rect_fill(pos, size, colors::accent.adjust_alpha(utils::alpha * a->second), 2.f);

            render::add_rect(pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

            render::add_text(render::size_16, utils::trim_name(label), pos + vector2f(size.x + 8.f, -2.f), colors::disabled_text.adjust_alpha(utils::alpha));
            render::add_text(render::size_16, utils::trim_name(label), pos + vector2f(size.x + 8.f, -2.f), colors::hovered_disabled.adjust_alpha(utils::alpha * h->second));

            utils::offset.y += size.y + 10.f;
        }

        bool add_button(const std::string& label, bool skip_hash_checks = false) {
            auto pos = utils::offset;
            auto size = vector2f(utils::next_group_box_size.x - 20.f, 22.f);

            static std::map<int, float> hover_map;
            static std::map<int, float> active_map;

            auto h = hover_map.find(std::hash<std::string>()(label));
            if (h == hover_map.end()) {
                hover_map.insert({ std::hash<std::string>()(label), 0.f });
                h = hover_map.find(std::hash<std::string>()(label));
            }

            auto a = active_map.find(std::hash<std::string>()(label));
            if (a == active_map.end()) {
                active_map.insert({ std::hash<std::string>()(label), 0.f });
                a = active_map.find(std::hash<std::string>()(label));
            }

            a->second = std::lerp(a->second, 0.f, 5.f * ImGui::GetIO().DeltaTime);

            bool hovered = !utils::skip && !utils::hash && ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, false);
            if (skip_hash_checks)
                hovered = ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, false);

            const bool clicked = hovered && ImGui::IsMouseClicked(0);

            if (clicked) a->second = 1.f;


            render::add_shadow_rect(pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(pos, size, colors::accent.adjust_alpha(utils::alpha * 0.6f * a->second), 15.f, 2.f);
            render::add_rect_fill(pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);
            render::add_rect(pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);
            render::add_text(render::size_16, label, pos + size / 2.f - render::text_size(render::size_16, label) / 2.f, colors::disabled_text.adjust_alpha(utils::alpha));
            render::add_text(render::size_16, label, pos + size / 2.f - render::text_size(render::size_16, label) / 2.f, colors::accent.adjust_alpha(utils::alpha * a->second));

            utils::offset.y += size.y + 10.f;

            return clicked;
        }

        void add_slider(const std::string& label, float* var, float min, float max, std::string fmt = "{:.2f}") {
            auto pos = utils::offset;
            auto size = vector2f(utils::next_group_box_size.x - 20.f, 12.f);
            const auto display_str = std::vformat(fmt, std::make_format_args(*var));

            static std::map<int, float> hover_map;
            static std::map<int, float> active_map;
            static std::map<int, float> drag_map;

            auto h = hover_map.find(std::hash<std::string>()(label));
            if (h == hover_map.end()) {
                hover_map.insert({ std::hash<std::string>()(label), 0.f });
                h = hover_map.find(std::hash<std::string>()(label));
            }

            auto a = active_map.find(std::hash<std::string>()(label));
            if (a == active_map.end()) {
                active_map.insert({ std::hash<std::string>()(label), static_cast<float>(*var) });
                a = active_map.find(std::hash<std::string>()(label));
            }

            auto d = drag_map.find(std::hash<std::string>()(label));
            if (d == drag_map.end()) {
                drag_map.insert({ std::hash<std::string>()(label), static_cast<float>(0.f) });
                d = drag_map.find(std::hash<std::string>()(label));
            }

            a->second = std::lerp(a->second, *var, 10.f * ImGui::GetIO().DeltaTime);
            d->second = std::lerp(d->second, utils::hash == std::hash<std::string>()(label), 10.f * ImGui::GetIO().DeltaTime);

            auto slider_pos = pos + vector2f(0.f, 16.f);

            const bool hovered = !utils::skip && !utils::hash && ImGui::IsMouseHoveringRect({ slider_pos.x, slider_pos.y }, { slider_pos.x + size.x, slider_pos.y + size.y }, false);
            const bool clicked = hovered && ImGui::IsMouseClicked(0);

            h->second = std::lerp(h->second, hovered or utils::hash == std::hash<std::string>()(label), 20.f * ImGui::GetIO().DeltaTime);

            if (!utils::hash && clicked)
            {
                utils::hash = std::hash<std::string>()(label);
            }

            if (utils::hash == std::hash<std::string>()(label)) {
                if (!ImGui::IsMouseDown(0)) {
                    utils::hash = 0;
                }

                float offset = std::clamp<float>(ImGui::GetIO().MousePos.x - slider_pos.x, 0.f, size.x);
                *var = std::clamp(static_cast<float>(utils::map_number<float>(offset, 0, size.x, min, max)), min, max);
            }

            render::add_shadow_rect(slider_pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(slider_pos, size, colors::accent.adjust_alpha(utils::alpha * 0.6f * d->second), 15.f, 2.f);
            render::add_rect_fill(slider_pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);

            const auto slider_width = std::clamp<float>(utils::map_number<float>(a->second, min, max, 0.f, size.x), 0.f, size.x);
            if (slider_width > 1)
                render::add_rect_fill(slider_pos, { slider_width, size.y }, colors::accent.adjust_alpha(utils::alpha), 2.f);

            render::add_rect(slider_pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

            render::set_to_foreground();

            auto value_box_size = render::text_size(render::size_16, display_str) + vector2f(4.f, 2.f);
            auto value_box_pos = vector2f(std::clamp<float>(slider_pos.x + slider_width, slider_pos.x, slider_pos.x + size.x - value_box_size.x), slider_pos.y - 4.f - value_box_size.y);

            render::add_shadow_rect(value_box_pos, value_box_size, colors::shadow.adjust_alpha(utils::alpha * h->second), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(value_box_pos, value_box_size, colors::accent.adjust_alpha(utils::alpha * 0.6f * h->second), 15.f, 2.f);
            render::add_rect_fill(value_box_pos, value_box_size, colors::background.adjust_alpha(utils::alpha * h->second), 2.f);
            render::add_rect(value_box_pos - 1.f, value_box_size + 2.f, colors::outline.adjust_alpha(utils::alpha * h->second), 2.f);
            render::add_text(render::size_16, display_str, value_box_pos + value_box_size / 2.f - render::text_size(render::size_16, display_str) / 2.f + vector2f(1.f, 0.f), colors::active_text.adjust_alpha(utils::alpha * h->second));
            render::set_to_background();

            render::add_text(render::size_16, label, pos - vector2f(0.f, -5.f + render::text_size(render::size_16, label).y / 2.f), colors::disabled_text.adjust_alpha(utils::alpha));
            render::add_text(render::size_16, label, pos - vector2f(0.f, -5.f + render::text_size(render::size_16, label).y / 2.f), colors::hovered_disabled.adjust_alpha(utils::alpha * h->second));

            utils::offset.y += size.y + 16.f + 10.f;
        }

        void add_slider_int(const std::string& label, int* var, int min, int max, std::string fmt = "{}") {
            auto pos = utils::offset;
            auto size = vector2f(utils::next_group_box_size.x - 20.f, 12.f);
            const auto display_str = std::vformat(fmt, std::make_format_args(*var));

            static std::map<int, float> hover_map;
            static std::map<int, float> active_map;
            static std::map<int, bool> drag_map;

            auto h = hover_map.find(std::hash<std::string>()(label));
            if (h == hover_map.end()) {
                hover_map.insert({ std::hash<std::string>()(label), 0.f });
                h = hover_map.find(std::hash<std::string>()(label));
            }

            auto a = active_map.find(std::hash<std::string>()(label));
            if (a == active_map.end()) {
                active_map.insert({ std::hash<std::string>()(label), static_cast<float>(*var) });
                a = active_map.find(std::hash<std::string>()(label));
            }

            auto d = drag_map.find(std::hash<std::string>()(label));
            if (d == drag_map.end()) {
                drag_map.insert({ std::hash<std::string>()(label), false });
                d = drag_map.find(std::hash<std::string>()(label));
            }

            a->second = std::lerp(a->second, static_cast<float>(*var), 10.f * ImGui::GetIO().DeltaTime);
            d->second = utils::hash == std::hash<std::string>()(label);

            auto slider_pos = pos + vector2f(0.f, 16.f);

            const bool hovered = !utils::skip && !utils::hash && ImGui::IsMouseHoveringRect({ slider_pos.x, slider_pos.y }, { slider_pos.x + size.x, slider_pos.y + size.y }, false);
            const bool clicked = hovered && ImGui::IsMouseClicked(0);

            h->second = std::lerp(h->second, hovered or utils::hash == std::hash<std::string>()(label), 20.f * ImGui::GetIO().DeltaTime);

            if (!utils::hash && clicked)
            {
                utils::hash = std::hash<std::string>()(label);
            }

            if (utils::hash == std::hash<std::string>()(label)) {
                if (!ImGui::IsMouseDown(0)) {
                    utils::hash = 0;
                }

                float offset = std::clamp<float>(ImGui::GetIO().MousePos.x - slider_pos.x, 0.f, size.x);
                *var = std::clamp(static_cast<int>(utils::map_number<float>(offset, 0.f, size.x, static_cast<float>(min), static_cast<float>(max))), min, max);
            }

            render::add_shadow_rect(slider_pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(slider_pos, size, colors::accent.adjust_alpha(utils::alpha * 0.6f * d->second), 15.f, 2.f);
            render::add_rect_fill(slider_pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);

            const auto slider_width = std::clamp<float>(utils::map_number<float>(a->second, min, max, 0.f, size.x), 0.f, size.x);
            if (slider_width > 1)
                render::add_rect_fill(slider_pos, { slider_width, size.y }, colors::accent.adjust_alpha(utils::alpha), 2.f);

            render::add_rect(slider_pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

            render::set_to_foreground();

            auto value_box_size = render::text_size(render::size_16, display_str) + vector2f(4.f, 2.f);
            auto value_box_pos = vector2f(std::clamp<float>(slider_pos.x + slider_width, slider_pos.x, slider_pos.x + size.x - value_box_size.x), slider_pos.y - 4.f - value_box_size.y);

            render::add_shadow_rect(value_box_pos, value_box_size, colors::shadow.adjust_alpha(utils::alpha * h->second), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(value_box_pos, value_box_size, colors::accent.adjust_alpha(utils::alpha * 0.6f * h->second), 15.f, 2.f);
            render::add_rect_fill(value_box_pos, value_box_size, colors::background.adjust_alpha(utils::alpha * h->second), 2.f);
            render::add_rect(value_box_pos - 1.f, value_box_size + 2.f, colors::outline.adjust_alpha(utils::alpha * h->second), 2.f);
            render::add_text(render::size_16, display_str, value_box_pos + value_box_size / 2.f - render::text_size(render::size_16, display_str) / 2.f + vector2f(1.f, 0.f), colors::active_text.adjust_alpha(utils::alpha * h->second));
            render::set_to_background();

            render::add_text(render::size_16, label, pos - vector2f(0.f, -5.f + render::text_size(render::size_16, label).y / 2.f), colors::disabled_text.adjust_alpha(utils::alpha));
            render::add_text(render::size_16, label, pos - vector2f(0.f, -5.f + render::text_size(render::size_16, label).y / 2.f), colors::hovered_disabled.adjust_alpha(utils::alpha * h->second));

            utils::offset.y += size.y + 16.f + 10.f;
        }

        void add_drop_down(const std::string& label, int* var, std::vector<std::string> items) {
            auto pos = utils::offset;
            auto size = vector2f(utils::next_group_box_size.x - 20.f, 22.f);

            static std::map<int, float> hover_map;
            static std::map<int, float> active_map;

            auto h = hover_map.find(std::hash<std::string>()(label));
            if (h == hover_map.end()) {
                hover_map.insert({ std::hash<std::string>()(label), 0.f });
                h = hover_map.find(std::hash<std::string>()(label));
            }

            auto a = active_map.find(std::hash<std::string>()(label));
            if (a == active_map.end()) {
                active_map.insert({ std::hash<std::string>()(label), static_cast<float>(*var) });
                a = active_map.find(std::hash<std::string>()(label));
            }


            a->second = std::lerp(a->second, utils::hash == std::hash<std::string>()(label), 10.f * ImGui::GetIO().DeltaTime);

            auto drop_down_pos = pos + vector2f(0.f, 16.f);

            const bool hovered = !utils::skip && !utils::hash && ImGui::IsMouseHoveringRect({ drop_down_pos.x, drop_down_pos.y }, { drop_down_pos.x + size.x, drop_down_pos.y + size.y }, false);
            const bool clicked = hovered && ImGui::IsMouseClicked(0);

            h->second = std::lerp(h->second, hovered or utils::hash == std::hash<std::string>()(label), 20.f * ImGui::GetIO().DeltaTime);

            if (!utils::hash && clicked && !utils::skip)
            {
                utils::hash = std::hash<std::string>()(label);
                utils::skip = true;
            }

            if (utils::hash == std::hash<std::string>()(label)) {
                // :trole:
                if (ImGui::IsMouseHoveringRect({ drop_down_pos.x, drop_down_pos.y }, { drop_down_pos.x + size.x, drop_down_pos.y + size.y },
                    false) && ImGui::IsMouseClicked(0) && !utils::skip) {
                    utils::hash = 0;
                }

                static std::map<int, float> item_map;

                auto item = [&](std::string title, vector2f p, vector2f s, bool active, int _i) -> bool {

                    const bool hovered = ImGui::IsMouseHoveringRect({ p.x, p.y }, { p.x + s.x, p.y + s.y }, false);
                    const bool clicked = hovered && ImGui::IsMouseReleased(0);

                    render::add_rect_fill(p, s, colors::background.adjust_alpha(utils::alpha * h->second), _i == 0 or _i == items.size() - 1 ? 2.f : 0.f, _i == 0 ? ImDrawFlags_RoundCornersTop : _i == items.size() - 1 ? ImDrawFlags_RoundCornersBottom : 0);
                    if (_i != items.size() - 1) {
                        render::add_rect_fill(p + vector2f(5.f, s.y - 1.f), vector2f(s.x - 10.f, 1.f), colors::outline.adjust_alpha(utils::alpha * 0.5f * h->second));
                    }
                    auto _item = item_map.find(std::hash<std::string>()(label + title));
                    if (_item == item_map.end()) {
                        item_map.insert({ std::hash<std::string>()(label + title) , 0.f });
                        _item = item_map.find(std::hash<std::string>()(label + title));
                    }
                    _item->second = std::lerp(_item->second, hovered, 10.f * ImGui::GetIO().DeltaTime);
                    render::add_text(active ? render::bold_16 : render::size_16, title, p + vector2f(active ? 10.f : 5.f, 3.f), colors::disabled_text.adjust_alpha(utils::alpha * h->second));
                    render::add_text(active ? render::bold_16 : render::size_16, title, p + vector2f(active ? 10.f : 5.f, 3.f), colors::hovered_disabled.adjust_alpha(utils::alpha * _item->second * h->second));
                    if (active)
                        render::add_text(active ? render::bold_16 : render::size_16, title, p + vector2f(active ? 10.f : 5.f, 3.f), colors::accent.adjust_alpha(utils::alpha * h->second));
                    return clicked;
                    };

                render::set_to_foreground();

                render::add_shadow_rect(drop_down_pos + vector2f(0.f, 5.f + size.y), vector2f(size.x, size.y * items.size()), colors::accent.adjust_alpha(utils::alpha * 0.6f * h->second), 15.f, 2.f);

                for (int i = 0; i < items.size(); i++) {
                    auto _p = drop_down_pos + vector2f(0.f, 5.f + size.y + (size.y * i));

                    if (item(items.at(i), _p, size, *var == i, i)) {
                        *var = i;
                        utils::hash = 0;
                        utils::skip = true;
                    }
                }

                render::add_rect(drop_down_pos + vector2f(-1.f, 4.f + size.y), vector2f(size.x + 2.f, (size.y * items.size()) + 2.f), colors::outline.adjust_alpha(utils::alpha * h->second), 2.f);

                render::set_to_background();
            }

            render::add_shadow_rect(drop_down_pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(drop_down_pos, size, colors::accent.adjust_alpha(utils::alpha * 0.6f * a->second), 15.f, 2.f);
            render::add_rect_fill(drop_down_pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);

            float rad_size = 6.f;
            render::radial_gradient(drop_down_pos + vector2f(size.x - 5.f - rad_size, size.y / 2.f),
                rad_size, colors::disabled_text.adjust_alpha(utils::alpha - a->second), colors::disabled_text.adjust_alpha(0.f));
            render::radial_gradient(drop_down_pos + vector2f(size.x - 5.f - rad_size, size.y / 2.f),
                rad_size, colors::accent.adjust_alpha(utils::alpha * a->second), colors::accent.adjust_alpha(0.f));

            render::add_rect(drop_down_pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

            render::add_text(render::size_16, label, pos - vector2f(0.f, -5.f + render::text_size(render::size_16, label).y / 2.f), colors::disabled_text.adjust_alpha(utils::alpha));
            render::add_text(render::size_16, label, pos - vector2f(0.f, -5.f + render::text_size(render::size_16, label).y / 2.f), colors::hovered_disabled.adjust_alpha(utils::alpha * h->second));

            render::add_text(render::size_16, items.at(*var), drop_down_pos + vector2f(5.f, 2.f), colors::disabled_text.adjust_alpha(utils::alpha));
            render::add_text(render::size_16, items.at(*var), drop_down_pos + vector2f(5.f, 2.f), colors::hovered_disabled.adjust_alpha(utils::alpha * h->second));

            utils::offset.y += size.y + 16.f + 10.f;
        }

        void add_color_picker(const std::string& hash, color* col, bool show_alpha_bar = false) {
            auto size = vector2f(22.f, 12.f);
            auto pos = utils::offset + vector2f(utils::next_group_box_size.x - 20.f - size.x, 0.f);

            render::add_shadow_rect(pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_rect_fill(pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);
            render::add_rect_fill(pos, size, col->adjust_alpha(utils::alpha), 2.f);
            render::add_rect(pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

            const bool hovered = !utils::hash && !utils::skip && ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, false);
            const bool clicked = hovered && ImGui::IsMouseClicked(0);

            if (clicked) {
                utils::hash = std::hash<std::string>()(hash);
                utils::skip = true;
            }

            if (utils::hash == std::hash<std::string>()(hash))
            {

                render::set_to_foreground();

                auto p = pos + vector2f(0.f, size.y + 5.f);
                auto s = vector2f(160.f, 160.f + 18.f + (show_alpha_bar ? 18.f : 0.f) + 22.f + 10.f);

                if (!utils::skip && ImGui::IsMouseClicked(0) && !ImGui::IsMouseHoveringRect({ p.x, p.y }, { p.x + s.x, p.y + s.y }, false)) {
                    utils::hash = 0;
                }

                render::add_shadow_rect(p, s, colors::shadow.adjust_alpha(utils::alpha), 30.f, 2.f, vector2f(1.f));
                render::add_shadow_rect(p, s, colors::accent.adjust_alpha(utils::alpha * 0.6f), 15.f, 2.f);
                render::add_rect_fill(p, s, colors::background.adjust_alpha(utils::alpha), 2.f);
                render::add_rect(p - 1.f, s + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

                const color col_hues[6 + 1] = {
                    color(255, 0, 0).adjust_alpha(utils::alpha),color(255, 255, 0).adjust_alpha(utils::alpha),
                    color(0, 255, 0).adjust_alpha(utils::alpha),color(0, 255, 255).adjust_alpha(utils::alpha),
                    color(0, 0, 255).adjust_alpha(utils::alpha),color(255, 0, 255).adjust_alpha(utils::alpha),
                    color(255, 0, 0).adjust_alpha(utils::alpha)
                };

                float hue, sat, val, r, g, b, a, _r, _g, _b;
                ImGui::ColorConvertRGBtoHSV(col->r / 255.f, col->g / 255.f, col->b / 255.f, hue, sat, val);
                ImGui::ColorConvertHSVtoRGB(hue, 1.f, 1.f, _r, _g, _b);
                auto display_col = color(_r * 255, _g * 255, _b * 255);
                a = ImSaturate(col->a / 255.f);

                render::gradient_rect_filled(p + vector2f(5.f), vector2f(150.f), color::white().adjust_alpha(utils::alpha), display_col.adjust_alpha(utils::alpha), render::e_gradient_type::horizontal, 2.f, ImDrawFlags_RoundCornersAll);
                render::gradient_rect_filled(p + vector2f(5.f), vector2f(150.f), color::black().adjust_alpha(0.f), color::black().adjust_alpha(utils::alpha), render::e_gradient_type::vertical, 2.f, ImDrawFlags_RoundCornersAll);
                render::add_shadow_rect(p + vector2f(5.f), vector2f(150.f), colors::accent.adjust_alpha(utils::alpha * 0.6f), 25.f, 2.f);

                for (int i = 0; i < 6; i++)
                {
                    render::gradient_rect_filled(p + vector2f(5.f + (150.f / 6.f) * i, 160.f), vector2f(150.f / 6.f, 16.f), col_hues[i], col_hues[i + 1], render::e_gradient_type::horizontal, 0.f, 0);
                }

                render::add_rect_fill(p + vector2f(5.f + (147.f * hue), 160.f), vector2f(3.f, 16.f), color::white().adjust_alpha(utils::alpha), 2.f);

                render::add_rect(p + vector2f(5.f, 160.f), vector2f(150.f, 16.f), colors::outline.adjust_alpha(utils::alpha));

                render::radial_gradient(p + vector2f(5.f + (150.f * sat), 5.f + (150.f * (1.f - val))), 5.f, color::white().adjust_alpha(utils::alpha), color::white().adjust_alpha(0.f));

                static size_t _bl = 0;

                if (!_bl) {
                    // saturation & value
                    if (ImGui::IsMouseHoveringRect({ p.x + 5.f, p.y + 5.f }, { p.x + 155.f, p.y + 155.f }, false) && ImGui::IsMouseClicked(0))
                        _bl = 1;

                    // hue
                    if (ImGui::IsMouseHoveringRect({ p.x + 5.f, p.y + 155.f }, { p.x + 155.f, p.y + 155.f + 16.f }, false) && ImGui::IsMouseClicked(0))
                        _bl = 2;
                }
                if (_bl == 1) {
                    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        _bl = 0;

                    sat = ImSaturate(std::clamp<float>((ImGui::GetIO().MousePos.x - (p.x + 5.f)) / 150.f, 0.f, 1.f));
                    val = ImSaturate(1.f - std::clamp<float>((ImGui::GetIO().MousePos.y - (p.y + 5.f)) / 150.f, 0.f, 1.f));
                }

                if (_bl == 2) {
                    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        _bl = 0;

                    hue = ImSaturate(std::clamp<float>((ImGui::GetIO().MousePos.x - (p.x + 5.f)) / 150.f, 0.f, 1.f));
                }

                // THIS IS CRAZYYYYYY
                static auto original_pos = utils::offset;
                static auto original_size = utils::next_group_box_size;
                original_pos = utils::offset;
                original_size = utils::next_group_box_size;

                if (ImGui::IsMouseDown(0))
                {
                    ImGui::ColorConvertHSVtoRGB(hue, sat, val, r, g, b);
                    col->r = r * 255;
                    col->g = g * 255;
                    col->b = b * 255;
                }

                utils::offset = p + vector2f(5.f, 160.f + 6.f + 16.f + (show_alpha_bar ? +16.f + 5.f : 0.f));
                utils::next_group_box_size = vector2f((150 / 2.f) + 17.f, 0.f);

                if (add_button("copy", true)) {
                    utils::copied_color.r = col->r;
                    utils::copied_color.g = col->g;
                    utils::copied_color.b = col->b;
                    utils::copied_color.a = col->a;
                    printf("copied color");
                }

                utils::offset.y -= 22.f + 10.f;
                utils::offset.x += utils::next_group_box_size.x - 14.f;

                if (add_button("paste", true)) {
                    col->r = utils::copied_color.r;
                    col->g = utils::copied_color.g;
                    col->b = utils::copied_color.b;
                    col->a = utils::copied_color.a;
                    printf("pasted color");
                }

                utils::offset = original_pos;
                utils::next_group_box_size = original_size;

                render::set_to_background();
            }
        }

        auto get_key_name = [&](const int vk_key, char* buffer, bool capitalize_first_char = false) {
            if (vk_key == 0) {
                strcpy(buffer, "none");
                return;
            }

            switch (vk_key) {
            case 0x05:
                strcpy(buffer, "m4");
                return;
            case 0x06:
                strcpy(buffer, "m5");
                return;
            case 0x01:
                strcpy(buffer, "m1");
                return;
            case 0x04:
                strcpy(buffer, "m3");
                return;
            case 0x02:
                strcpy(buffer, "m2");
                return;
            }

            auto scan_code = MapVirtualKeyA(vk_key, MAPVK_VK_TO_VSC);

            switch (vk_key) {
            case VK_LEFT:
            case VK_UP:
            case VK_RIGHT:
            case VK_DOWN:
            case VK_RCONTROL:
            case VK_RMENU:
            case VK_LWIN:
            case VK_RWIN:
            case VK_APPS:
            case VK_PRIOR:
            case VK_NEXT:
            case VK_END:
            case VK_HOME:
            case VK_INSERT:
            case VK_DELETE:
            case VK_DIVIDE:
            case VK_NUMLOCK:
                scan_code |= KF_EXTENDED;
            }

            if (auto result = GetKeyNameTextA(scan_code << 16, buffer, 128); result == 0) {
                strcpy(buffer, "none");
                return;
            }

            for (size_t i = 0; i < strlen(buffer); i++)
                buffer[i] = tolower(buffer[i]);

            if (capitalize_first_char)
                buffer[0] = toupper(buffer[0]);
            };

        void add_key_bind(const std::string& hash, key_bind_t* key) {
            struct changing_value_t {
                std::string last_value;
                std::string current_value;

                float main_alpha = 0.f;
                float switch_time = 0.f;
                bool should_switch = false;
            };

            static std::map<int, changing_value_t> change_anim_map;
            static std::map<int, float> size_anim_map;
            static std::map<int, float> rotate_anim_map;

            char buf[128] = {};
            get_key_name(utils::hash == std::hash<std::string>()(hash) ? -1 : key->key, buf);
            const char* value_key; value_key = buf;

            auto value = change_anim_map.find(std::hash<std::string>()(hash + "#anim"));
            if (value == change_anim_map.end()) {
                changing_value_t temp_str;
                temp_str.last_value = value_key;
                temp_str.current_value = value_key;
                change_anim_map.insert({ std::hash<std::string>()(hash + "#anim"), temp_str });
                value = change_anim_map.find(std::hash<std::string>()(hash + "#anim"));
            }

            auto rotate = rotate_anim_map.find(std::hash<std::string>()(hash + "#anim"));
            if (rotate == rotate_anim_map.end()) {
                rotate_anim_map.insert({ std::hash<std::string>()(hash + "#anim"), 0.f });
                rotate = rotate_anim_map.find(std::hash<std::string>()(hash + "#anim"));
            }

            auto static_value_size = render::text_size(render::size_16, value->second.last_value);

            auto size_anim = size_anim_map.find(std::hash<std::string>()(hash + "#anim"));
            if (size_anim == size_anim_map.end())
            {
                size_anim_map.insert({ std::hash<std::string>()(hash + "#anim"), static_value_size.x });
                size_anim = size_anim_map.find(std::hash<std::string>()(hash + "#anim"));
            }

            size_anim->second = std::lerp(size_anim->second, static_value_size.x, 20.f * ImGui::GetIO().DeltaTime);

            value->second.current_value = value_key;
            if (value->second.current_value != value->second.last_value && !value->second.should_switch) {
                value->second.should_switch = true;
            }

            if (value->second.should_switch) {
                if (value->second.last_value != value->second.current_value && value->second.main_alpha <= 1.f) {
                    value->second.main_alpha += (3.f * ImGui::GetIO().DeltaTime);
                }

                if (value->second.main_alpha >= 1.f) {
                    value->second.last_value = value->second.current_value;
                }

                if (value->second.last_value == value->second.current_value) {
                    value->second.main_alpha -= (3.f * ImGui::GetIO().DeltaTime);
                    if (value->second.main_alpha <= 0.f)
                        value->second.should_switch = false;
                }
            }

            auto size = vector2f(4.f + size_anim->second, 14.f);
            auto pos = utils::offset + vector2f(utils::next_group_box_size.x - 20.f - size.x, 0.f);

            const bool hovered = !utils::hash && !utils::skip && ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, false);
            const bool clicked = hovered && ImGui::IsMouseClicked(0);
            const bool right_clicked = hovered && GetAsyncKeyState(VK_RBUTTON) & 1;

            if (clicked) {
                utils::hash = std::hash<std::string>()(hash);
                utils::skip = true;
            }

            if (right_clicked) {
                utils::hash = std::hash<std::string>()(hash) + 0x4;
            }

            if (utils::hash == std::hash<std::string>()(hash)) {
                auto vk_to_mouse = [&](int vk)
                    {
                        switch (vk) {
                        case VK_LBUTTON:
                            return 0;
                        case VK_RBUTTON:
                            return 1;
                        case VK_MBUTTON:
                            return 2;
                        case VK_XBUTTON1:
                            return 3;
                        case VK_XBUTTON2:
                            return 4;
                        }

                        return 0;
                };

                auto is_key_pressed = [&](int key, bool repeat) -> bool
                    {
                        if (key > VK_XBUTTON2) {
                            return ImGui::IsKeyPressed((ImGuiKey)key, repeat);
                        }
                        else {
                            return ImGui::IsMouseClicked(vk_to_mouse(key), repeat);
                        }
                };

                // main input
                if (!utils::skip)
                {
                    for (int i = 1; i < 0xff; i++) {
                        if (i == VK_CANCEL)
                            continue;

                        if (is_key_pressed(i, false)) {
                            key->key = i;
                            utils::hash = 0;
                            utils::skip = true;

                            if (i == VK_ESCAPE)
                                key->key = 0;
                        }
                    }
                }
            }

            if (utils::hash == std::hash<std::string>()(hash) + 0x4) {

                render::set_to_foreground();

                auto p = pos + vector2f(size.x + 5.f, 0.f);
                // this is STUPID
                auto s = vector2f(render::text_size(render::size_16, "toggled").x + 4.f, size.y * 3.f);

                render::add_shadow_rect(p, s, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
                render::add_shadow_rect(p, s, colors::accent.adjust_alpha(utils::alpha * 0.5f), 15.f, 2.f);
                render::add_rect_fill(p, s, colors::background.adjust_alpha(utils::alpha), 2.f);
                render::add_rect(p - 1.f, s + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

                render::add_text(render::size_16, "always", p + vector2f(s.x / 2.f, size.y / 2.f) - render::text_size(render::size_16, "always") / 2.f, key->mode == 0 ? colors::accent.adjust_alpha(utils::alpha) : colors::disabled_text.adjust_alpha(utils::alpha));
                render::add_text(render::size_16, "toggled", p + vector2f(0.f, size.y) + vector2f(s.x / 2.f, size.y / 2.f) - render::text_size(render::size_16, "toggled") / 2.f, key->mode == 1 ? colors::accent.adjust_alpha(utils::alpha) : colors::disabled_text.adjust_alpha(utils::alpha));
                render::add_text(render::size_16, "hold", p + vector2f(0.f, size.y * 2.f) + vector2f(s.x / 2.f, size.y / 2.f) - render::text_size(render::size_16, "hold") / 2.f, key->mode == 2 ? colors::accent.adjust_alpha(utils::alpha) : colors::disabled_text.adjust_alpha(utils::alpha));

                if (!utils::skip && ImGui::IsMouseHoveringRect({ p.x, p.y }, { p.x + s.x, p.y + size.y }, false) && ImGui::IsMouseClicked(0)) {
                    key->mode = 0;
                    utils::skip = true;
                    utils::hash = 0;
                }

                if (!utils::skip && ImGui::IsMouseHoveringRect({ p.x, p.y + size.y }, { p.x + s.x, p.y + size.y * 2.f }, false) && ImGui::IsMouseClicked(0)) {
                    key->mode = 1;
                    utils::skip = true;
                    utils::hash = 0;
                }

                if (!utils::skip && ImGui::IsMouseHoveringRect({ p.x, p.y + size.y * 2.f }, { p.x + s.x, p.y + size.y * 3.f }, false) && ImGui::IsMouseClicked(0)) {
                    key->mode = 2;
                    utils::skip = true;
                    utils::hash = 0;
                }

                if (!utils::skip && !ImGui::IsMouseHoveringRect({ p.x, p.y }, { p.x + s.x, p.y + s.y }, false) && ImGui::IsMouseClicked(0)) {
                    utils::skip = true;
                    utils::hash = 0;
                }
                render::set_to_background();
            }

            rotate->second = std::lerp(rotate->second, utils::hash == std::hash<std::string>()(hash) or utils::hash == std::hash<std::string>()(hash) + 0x4, 10.f * ImGui::GetIO().DeltaTime);

            render::add_shadow_rect(pos, size, colors::shadow.adjust_alpha(utils::alpha), 15.f, 2.f, vector2f(1.f));
            render::add_shadow_rect(pos, size, colors::accent.adjust_alpha(utils::alpha * 0.5f * rotate->second), 15.f, 2.f);
            render::add_rect_fill(pos, size, colors::background.adjust_alpha(utils::alpha), 2.f);
            render::add_rect(pos - 1.f, size + 2.f, colors::outline.adjust_alpha(utils::alpha), 2.f);

            auto col = colors::disabled_text.lerp(colors::accent, rotate->second);
            render::add_text(render::size_16, value_key, pos + size / 2.f - render::text_size(render::size_16, value_key) / 2.f - vector2f(0.f, 1.f), col.adjust_alpha(utils::alpha - value->second.main_alpha));
            render::add_text(render::size_16, value_key, pos + size / 2.f - render::text_size(render::size_16, value_key) / 2.f - vector2f(0.f, 1.f), col.adjust_alpha(utils::alpha * value->second.main_alpha));
        }
    }
}
