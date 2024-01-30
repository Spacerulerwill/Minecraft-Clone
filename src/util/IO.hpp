/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef IO_H
#define IO_H

#include <fstream>
#include <string>
#include <world/World.hpp>
#include <fmt/format.h>

template<typename T>
void WriteStructToDisk(const std::string& file_name, const T& data){
	std::ofstream out;
	out.open(file_name,std::ios::binary | std::ios::trunc);
	out.write(reinterpret_cast<const char*>(&data), sizeof(T));
};

template<typename T>
void ReadStructFromDisk(const std::string& file_name, T& data)
{
	std::ifstream in;
	in.open(file_name,std::ios::binary);
	if (in.fail()) {
		throw WorldCorruptionException(fmt::format("Failed to read data from {}", file_name));
	}
	in.read(reinterpret_cast<char*>(&data), sizeof(T));
};

#endif // !IO_H

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
