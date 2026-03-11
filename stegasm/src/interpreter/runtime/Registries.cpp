//
// Created by Roumite on 21/02/2026.
//

#include "Registries.h"

#include "Logger.h"
#include "../exceptions.h"

void Registries::check_registry_valid(const uint16_t registry_number)
{
    if (registry_number > number_of_registries)
        throw RegistryError("Invalid registry number " + std::to_string(registry_number));
}

void Registries::write(const RegNames registry_name, const uint32_t value)
{
    this->registries[static_cast<uint16_t>(registry_name)] = value;
}

void Registries::write(const uint16_t registry_number, const uint32_t value)
{
    check_registry_valid(registry_number);
    registries[registry_number] = value;
}

uint32_t Registries::read(const RegNames registry_name) const
{
    return registries[static_cast<uint16_t>(registry_name)];
}

uint32_t Registries::read(uint16_t registry_number) const
{
    check_registry_valid(registry_number);
    return registries[registry_number];
}

void Registries::display() const
{
    for (uint16_t i = 0; i < number_of_registries; i++)
    {
        auto reg = static_cast<RegNames>(i);
        Logger::log(registryToString[reg] + " -> " + std::to_string(registries[i]), "Registries");
    }
}
