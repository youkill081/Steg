//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include "files/File.h"

#include <string>
#include <cstdint>
#include <raylib.h>
#include <functional>
#include <memory>
#include <map>

#define FONT_BASE_SIZE 256
#define FONT_TEXT_SPACING 1.0f
#define DEFAULT_TEXTURE_COLOR_MASK WHITE

class Runtime;

struct Framebuffer {
    RenderTexture2D renderTex;
    uint32_t memoryAddress; // Address in runtime memory
    uint32_t width;
    uint32_t height;
};

class GraphicalBackend
{
private:
    static bool check_inited(bool throw_if_not_inited = false);

    uint16_t _text_size = 8;
    Color _text_color = WHITE;

    std::map<std::shared_ptr<FileBase>, Texture2D, std::owner_less<>> _textures;
    std::map<uint32_t, Framebuffer> _framebuffers;  // Id -> Framebuffer

    Color _texture_color_mask = DEFAULT_TEXTURE_COLOR_MASK;

    void load_texture(const std::shared_ptr<FileBase> &file);
    void unload_texture(const std::shared_ptr<FileBase> &file);

    uint16_t _viewport_width = 0;
    uint16_t _viewport_height = 0;
    bool _viewport_active = false;
    RenderTexture2D _target{}; // Used by viewport

    Font _current_font{};
    bool _font_loaded = false;
    std::string _last_font_path;

    Color last_clear_color = BLACK;
public:
    // Windows Management

    void create_window(uint16_t width, uint16_t height, const std::string &title);
    void close_window();
    void poll_events();
    bool should_close();

    void set_target_fps(uint16_t fps);
    void set_window_icon(const std::shared_ptr<FileBase> &file);

    // Viewport Management
    void set_viewport_size(uint16_t width, uint16_t height);
    void disable_viewport();

    // Drawing management

    void clear_window(const Color &color);
    void present_window() const;

    void set_text_size(const uint16_t size);
    void set_text_color(const Color &color) { _text_color = color; }
    void draw_text(const std::string &text, int x, int y);
    void set_font(const std::shared_ptr<FileBase> &file);

    void set_texture_color_mask(const Color &color);
    void reset_texture_color_mask() { _texture_color_mask = DEFAULT_TEXTURE_COLOR_MASK; }
    void draw_texture(const std::shared_ptr<FileBase> &file, int x, int y);

    // Input management
    bool key_down(uint16_t key);
    bool key_pressed(uint16_t key);

    // Mouse management
    [[nodiscard]] int32_t mouse_x() const;
    [[nodiscard]] int32_t mouse_y() const;
    [[nodiscard]] int32_t mouse_delta_x() const;
    [[nodiscard]] int32_t mouse_delta_y() const;
    [[nodiscard]] bool mouse_button_pressed(int button) const;
    [[nodiscard]] bool mouse_button_down(int button) const;
    [[nodiscard]] bool mouse_button_released(int button) const;
    [[nodiscard]] int32_t mouse_wheel_delta() const;
    void hide_cursor() const;
    void show_cursor() const;

    // Framebuffer management
    uint32_t create_framebuffer(Runtime &runtime, uint32_t width, uint32_t height);
    void sync_framebuffer(Runtime &runtime, uint32_t framebuffer_id);
    uint32_t framebuffer_get_address(uint32_t framebuffer_id);
    void draw_framebuffer(uint32_t framebuffer_id, int x, int y);

    // Utils

    /*
     *  Convert coordinate from screen to viewport (if viewport is active)
     */
    [[nodiscard]] Vector2 screen_to_viewport(const Vector2 &screenPos) const;
};