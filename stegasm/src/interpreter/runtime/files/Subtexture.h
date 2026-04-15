//
// Created by Roumite on 27/02/2026.
//

#pragma once

#include <memory>

#include "File.h"
#include "FileBase.h"

class SubTexture final : public FileBase
{
private:
    File *_origin;
    Crop _crop;

public:
    SubTexture(File *origin, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
        : _origin(origin), _crop{x, y, w, h}
    {
        _origin->add_callback([this]() { this->call_callbacks(); });
    }

    const std::string& get_path() override { return _origin->get_path(); }

    [[nodiscard]] std::optional<Crop> get_crop() const override { return _crop; }
    FileBaseType get_file_type() override { return SUBTEXTURE_TYPE; }
};
