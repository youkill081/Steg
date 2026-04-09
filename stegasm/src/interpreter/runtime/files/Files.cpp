//
// Created by Roumite on 27/02/2026.
//

#include "Files.h"

#include "interpreter/exceptions.h"

descriptor Files::next_descriptor()
{
    return _current++;
}

descriptor Files::open_file(const std::string& path)
{
    descriptor desc = next_descriptor();
    _entries[desc] = File::open_file(path);
    return desc;
}

descriptor Files::create_file(const std::string& path)
{
    descriptor desc = next_descriptor();
    _entries[desc] = File::create_empty_file(path);
    return desc;
}

void Files::push_file(descriptor desc, std::shared_ptr<FileBase> entry)
{
    _entries[desc] = std::move(entry);
    if (desc >= _current)
        _current = desc + 1;
}

void Files::close_file(descriptor desc)
{
    _entries[desc] = nullptr;
}

void Files::delete_file(descriptor desc)
{
    get_file(desc)->delete_file();
    _entries[desc] = nullptr;
}

std::shared_ptr<FileBase> Files::operator[](descriptor desc)
{
    std::shared_ptr<FileBase> entry = _entries[desc];
    if (!entry)
        throw FileError("Descriptor " + std::to_string(desc) + " not found !");
    return entry;
}

std::shared_ptr<File> Files::get_file(descriptor desc)
{
    std::shared_ptr<FileBase> entry = (*this)[desc];

    auto file = std::dynamic_pointer_cast<File>(entry);
    if (!file)
        throw FileError("Descriptor " + std::to_string(desc) + " is not a plain file !");
    return file;
}