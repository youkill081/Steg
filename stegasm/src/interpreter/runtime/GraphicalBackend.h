//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <cstdint>
#include <raylib.h>

class GraphicalBackend
{
private:
    bool check_inited(bool throw_if_not_inited = false);
public:
    // Windows Management

    void create_window();
    void close_window();
    void poll_events();
    bool should_close();

    void set_target_fps(uint16_t fps);

    // Drawing management

    void clear_window(const Color &color);
    void present_window();

    // Input management
    bool key_down(uint16_t key);
    bool key_pressed(uint16_t key);
};