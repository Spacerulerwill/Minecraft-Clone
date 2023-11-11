/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef IO_HPP
#define IO_HPP

#include <string>
#include <fstream>
#include <filesystem>

namespace engine {
    /*
    Write a struct's data to a binary file on secondary storage.
    NOTE: The struct must be ISO C complaint and not contain 
    pointers. The full data a pointer points too must be saved instead
    otherwise the pointer will be potentially dangling when you next load it
    */
    template<typename T>
    bool WriteStructToDisk(const std::string& file_name, T& data)
    {
        std::ofstream out;
        out.open(file_name,std::ios::binary | std::ios::trunc);
        if (out.fail())
            return false;
        out.write(reinterpret_cast<char*>(&data), sizeof(T));
        out.close();
        return true;
    };

    /*
    Read a binary file from secondary storage into a struct
    */
    template<typename T>
    bool ReadStructFromDisk(const std::string& file_name, T& data)
    {
        std::ifstream in;
        in.open(file_name,std::ios::binary);
        if (in.fail())
            return false;
        in.read(reinterpret_cast<char*>(&data), sizeof(T));
        in.close();
        return true;
    };

    // Count number of folders in directory
    inline size_t NumberOfFoldersInDirectory(std::filesystem::path path)
    {
        using std::filesystem::directory_iterator;
        using fp = bool (*)( const std::filesystem::path&);
        return std::count_if(directory_iterator(path), directory_iterator{}, (fp)std::filesystem::is_directory);
    }
}

#endif

/*
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/