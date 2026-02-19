//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <map>
#include <functional>

#include "../steganographer/constant.h"
#include "IHander.h"
#include "StringHandler.h"

inline std::map<DataType, std::function<std::unique_ptr<IHandler>()>> handlers = {
    {STRING, []{ return std::make_unique<StringHandler>(); }}
};

inline std::unique_ptr<IHandler> handler_factory(DataType type)
{
    const auto it = handlers.find(type);
    if (it == handlers.end())
        throw HandlerParameterError("Handler not found for given DataType");

    return it->second();
}