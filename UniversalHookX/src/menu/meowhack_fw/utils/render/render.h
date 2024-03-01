#pragma once
#include "fonts.h"
#include "../../utils/math/color.h"
#include "../../utils/math/vector.h"
#include <string>
#include "../../../../dependencies/imgui/imgui.h"
#include "../../../../dependencies/imgui/imgui_internal.h"
#include "../../../../dependencies/imgui/imgui_freetype.h"

namespace render {
    static bool _setup = false;
    inline ImDrawList* draw_list = nullptr;
    inline ImFont* _size_16;
    inline ImFont* _bold_16;
    inline ImFont* _size_24;
    inline ImFont* _small_10;

    enum font_sizes : int {
        size_16,
        bold_16,
        size_24,
        small_10
    };

    static ImFont* create_from_ttf(ImGuiIO& io, const uint8_t* ttf_data, int ttf_data_size, float size, int rasterizer_flags = 0,
        std::initializer_list<ImWchar> glyph_ranges = {}) {
        ImFontConfig font_config;

        font_config.FontData = std::malloc(ttf_data_size);
        font_config.FontDataSize = ttf_data_size;
        font_config.SizePixels = size;
        font_config.GlyphRanges = glyph_ranges.size() == 0 ? nullptr : glyph_ranges.begin();
        font_config.FontBuilderFlags = rasterizer_flags;

        std::memcpy(font_config.FontData, ttf_data, ttf_data_size);

        return io.Fonts->AddFont(&font_config);
    }

    void setup() {
        if (_setup) return;
        ImFontConfig font_config;
        font_config.FontDataOwnedByAtlas = false;
        font_config.OversampleH = 1;
        font_config.OversampleV = 1;
        font_config.FontBuilderFlags = 1;
        font_config.FontDataOwnedByAtlas = false;

        static const ImWchar ranges[] = {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0xE000, 0xE226, // icons
            0,
        };

        _size_16 = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.f, &font_config, ranges);
        _bold_16 = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 16.f, &font_config, ranges);
        _size_24 = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 24.f, &font_config, ranges);
        _small_10 = create_from_ttf(ImGui::GetIO(), pixelmix_ttf, sizeof(pixelmix_ttf), 10.f, ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome);
        ImGui::GetIO().FontDefault = _size_16;


        ImGuiFreeType::GetBuilderForFreeType()->FontBuilder_Build(ImGui::GetIO().Fonts);
        
        _setup = true;
    }

    void start() {
        draw_list = ImGui::GetBackgroundDrawList();
    }

    void set_to_foreground() { draw_list = ImGui::GetForegroundDrawList(); }

    void set_to_background() { draw_list = ImGui::GetBackgroundDrawList(); }

    void end() { draw_list = ImGui::GetBackgroundDrawList(); }

    void add_rect_fill(const vector2f& position, const vector2f& size, const color& color, float rounding = 0.f, int flags = 0) {
        draw_list->AddRectFilled({ position.x, position.y },
            { position.x + size.x, position.y + size.y }, ImColor(color.r, color.g, color.b, color.a), rounding, (ImDrawFlags)flags);
    }

    void add_rect(const vector2f& position, const vector2f& size, const color& color, float rounding = 0.f, float thick = 0.f, int flags = 0) {
        draw_list->AddRect({ position.x, position.y },
            { position.x + size.x, position.y + size.y }, ImColor(color.r, color.g, color.b, color.a), rounding, (ImDrawFlags)flags, thick);
    }

    void add_shadow_rect(const vector2f& position, const vector2f& size, const color& color, float thick, float rounding = 0.f, const vector2f& offset = vector2f(0.f, 0.f)) {
        draw_list->AddShadowRect({ position.x, position.y },
            { position.x + size.x, position.y + size.y }, ImColor(color.r, color.g, color.b, color.a), thick,
            { offset.x, offset.y }, ImDrawFlags_ShadowCutOutShapeBackground, rounding);
    }

    void radial_gradient(vector2f center, float radius, color col_in, color col_out) {

        if (((col_in.imgui() | col_out.imgui()) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
            return;


        draw_list->_PathArcToFastEx(center.imgui(), radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
        const int count = draw_list->_Path.Size - 1;

        unsigned int vtx_base = draw_list->_VtxCurrentIdx;
        draw_list->PrimReserve(count * 3, count + 1);

        const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
        draw_list->PrimWriteVtx(center.imgui(), uv, col_in.imgui());

        for (int n = 0; n < count; n++)
            draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out.imgui());

        for (int n = 0; n < count; n++) {
            draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
            draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
            draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
        }
        draw_list->_Path.Size = 0;
    }

    enum e_gradient_type {
        horizontal,
        vertical
    };

    void gradient_rect(vector2f pos, vector2f size, color color1, color color2, e_gradient_type type, float rounding, int flags, float thickness) {
        auto shade_verts_linear_color_gradient = [&](int vert_start_idx, int vert_end_idx, vector2f start_pos, vector2f to_size, color col0, color col1) {
            constexpr int r_shift = 0;
            constexpr int g_shift = 8;
            constexpr int b_shift = 16;
            constexpr int a_shift = 24;

            vector2f gradient_extent = to_size - start_pos;
            float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent.imgui());
            auto vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
            auto vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
            const int col0_r = (int)(col0.imgui() >> r_shift) & 0xFF;
            const int col0_g = (int)(col0.imgui() >> g_shift) & 0xFF;
            const int col0_b = (int)(col0.imgui() >> b_shift) & 0xFF;
            const int col0_a = (int)(col0.imgui() >> a_shift) & 0xFF;
            const int col_delta_r = ((int)(col1.imgui() >> r_shift) & 0xFF) - col0_r;
            const int col_delta_g = ((int)(col1.imgui() >> g_shift) & 0xFF) - col0_g;
            const int col_delta_b = ((int)(col1.imgui() >> b_shift) & 0xFF) - col0_b;
            const int col_delta_a = ((int)(col1.imgui() >> a_shift) & 0xFF) - col0_a;

            for (auto vert = vert_start; vert < vert_end; vert++) {
                float d = ImDot(vector2f(vert->pos.x - start_pos.x, vert->pos.y - start_pos.y).imgui(), gradient_extent.imgui());
                float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
                int r = (int)(col0_r + col_delta_r * t);
                int g = (int)(col0_g + col_delta_g * t);
                int b = (int)(col0_b + col_delta_b * t);
                int a = (int)(col0_a + col_delta_a * t);
                vert->col = (r << r_shift) | (g << g_shift) | (b << b_shift) | (a << a_shift);
            }
            };

        const int vtx_idx_0 = draw_list->VtxBuffer.Size;
        draw_list->AddRect(pos.imgui(), (pos + size).imgui(), ImColor(1.f, 1.f, 1.f, color1.imgui().Value.w), rounding, flags, thickness);
        const int vtx_idx_1 = draw_list->VtxBuffer.Size;

        // todo: fix color stop pls
        vector2f stop = { pos + (size * 0.5f) };

        switch (type) {
        case e_gradient_type::horizontal:
            shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, vector2f(pos.x, pos.y + stop.y), vector2f(pos.x + size.x, pos.y + stop.y), color1, color2);
            break;
        case e_gradient_type::vertical:
            shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, { pos.x + stop.x, pos.y }, { pos.x + stop.x, pos.y + size.y }, color1, color2);
            break;
        }
    }

    void gradient_rect_filled(vector2f pos, vector2f size, color color1, color color2, e_gradient_type type, float rounding, int flags) {
        auto shade_verts_linear_color_gradient = [&](int vert_start_idx, int vert_end_idx, vector2f start_pos, vector2f to_size, color col0, color col1) {
            constexpr int r_shift = 0;
            constexpr int g_shift = 8;
            constexpr int b_shift = 16;
            constexpr int a_shift = 24;

            vector2f gradient_extent = to_size - start_pos;
            float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent.imgui());
            auto vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
            auto vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
            const int col0_r = (int)(col0.imgui() >> r_shift) & 0xFF;
            const int col0_g = (int)(col0.imgui() >> g_shift) & 0xFF;
            const int col0_b = (int)(col0.imgui() >> b_shift) & 0xFF;
            const int col0_a = (int)(col0.imgui() >> a_shift) & 0xFF;
            const int col_delta_r = ((int)(col1.imgui() >> r_shift) & 0xFF) - col0_r;
            const int col_delta_g = ((int)(col1.imgui() >> g_shift) & 0xFF) - col0_g;
            const int col_delta_b = ((int)(col1.imgui() >> b_shift) & 0xFF) - col0_b;
            const int col_delta_a = ((int)(col1.imgui() >> a_shift) & 0xFF) - col0_a;

            for (auto vert = vert_start; vert < vert_end; vert++) {
                float d = ImDot(vector2f(vert->pos.x - start_pos.x, vert->pos.y - start_pos.y).imgui(), gradient_extent.imgui());
                float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
                int r = (int)(col0_r + col_delta_r * t);
                int g = (int)(col0_g + col_delta_g * t);
                int b = (int)(col0_b + col_delta_b * t);
                int a = (int)(col0_a + col_delta_a * t);
                vert->col = (r << r_shift) | (g << g_shift) | (b << b_shift) | (a << a_shift);
            }
            };

        const int vtx_idx_0 = draw_list->VtxBuffer.Size;
        draw_list->AddRectFilled(pos.imgui(), (pos + size).imgui(), ImColor(1.f, 1.f, 1.f, color2.imgui().Value.w), rounding, flags);
        const int vtx_idx_1 = draw_list->VtxBuffer.Size;

        // todo: fix color stop pls
        vector2f stop = { pos + (size * 0.5f) };

        switch (type) {
        case e_gradient_type::horizontal:
            shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, vector2f(pos.x, pos.y + stop.y), vector2f(pos.x + size.x, pos.y + stop.y), color1, color2);
            break;
        case e_gradient_type::vertical:
            shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, { pos.x + stop.x, pos.y }, { pos.x + stop.x, pos.y + size.y }, color1, color2);
            break;
        }
    }

    void add_text(font_sizes font_size, const std::string& label, const vector2f& position, const color& color) {
        switch (font_size)
        {
        case font_sizes::size_16:
            ImGui::PushFont(_size_16);
            draw_list->AddText({ position.x, position.y }, ImColor(color.r, color.g, color.b, color.a), label.data());
            break;
        case font_sizes::bold_16:
            ImGui::PushFont(_bold_16);
            draw_list->AddText({ position.x, position.y }, ImColor(color.r, color.g, color.b, color.a), label.data());
            break;
        case font_sizes::size_24:
            ImGui::PushFont(_size_24);
            draw_list->AddText({ position.x, position.y }, ImColor(color.r, color.g, color.b, color.a), label.data());
            break;
        case font_sizes::small_10:
            ImGui::PushFont(_small_10);
            draw_list->AddText({ position.x, position.y }, ImColor(color.r, color.g, color.b, color.a), label.data());
            break;
        default:
            ImGui::PushFont(_size_16);
            draw_list->AddText({ position.x, position.y }, ImColor(color.r, color.g, color.b, color.a), label.data());
        }

        ImGui::PopFont();
    }

    vector2f text_size(font_sizes font_size, const std::string& label) {
        ImVec2 size = ImVec2(0.f, 0.f);

        switch (font_size)
        {
        case font_sizes::size_16:
            ImGui::PushFont(_size_16);
            size = ImGui::CalcTextSize(label.data());
            break;
        case font_sizes::bold_16:
            ImGui::PushFont(_bold_16);
            size = ImGui::CalcTextSize(label.data());
            break;
        case font_sizes::size_24:
            ImGui::PushFont(_size_24);
            size = ImGui::CalcTextSize(label.data());
            break;
        case font_sizes::small_10:
            ImGui::PushFont(_small_10);
            size = ImGui::CalcTextSize(label.data());
            break;
        default:
            ImGui::PushFont(_size_16);
            size = ImGui::CalcTextSize(label.data());
        }

        ImGui::PopFont();

        return vector2f(size.x, size.y);
    }

    void begin_clip_rect(const vector2f& position, const vector2f& size) {
        draw_list->PushClipRect({ position.x, position.y }, { position.x + size.x, position.y + size.y });
    }

    void end_clip_rect() {
        draw_list->PopClipRect();
    }
}
