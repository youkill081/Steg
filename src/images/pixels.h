//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include <iostream>
#include <vector>

#include "pixel.h"

class pixels : public std::vector<pixel>
{
public:
    void display()
    {
        for (auto &pix: *this)
        {
            std::cout << std::to_string(pix) << std::endl;
        }
    };
};
