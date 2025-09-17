#pragma once
#include <Windows.h>
#include <charconv>
#include <fstream>
#include <streambuf>
#include <vector>

bool readFile(const char* path, std::vector<char>& content)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return true;
}

bool writeFile(const char* path, const std::vector<char>& content)
{
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) return false;
    file.write(content.data(), content.size());
    return true;
}

unsigned virtualAddress2RawOffset(char* image, unsigned address)
{
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)image;
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(&image[dosHeader->e_lfanew]);

    IMAGE_SECTION_HEADER* header = IMAGE_FIRST_SECTION(ntHeaders);
    for (size_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        DWORD begin = ntHeaders->OptionalHeader.ImageBase + header->VirtualAddress;
        DWORD end = begin + header->Misc.VirtualSize;
        if (address >= begin && address < end)
            return address - begin + header->PointerToRawData;
        header++;
    }

    return 0;
}

bool convHexString2ByteArray(const char* hex, std::vector<char>& result)
{
    size_t hexLen = strlen(hex);
    result.clear();
    result.reserve(hexLen / 2);
    for (size_t i = 0; (i + 1) < hexLen; i += 2) {
        int v;
        if (std::from_chars(&hex[i], &hex[i + 2], v, 0x10).ec == std::errc{})
            result.push_back(v & 0xFF);
    }
    return result.capacity() == result.size();
}