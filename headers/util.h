#pragma once
#include <string>
#include <fstream>

const std::string BASE_URL = "https://testserver.moveitcloud.com/";
const size_t CHUNK_SIZE = 4 * 1024 * 1024;

static size_t ResponseCb(void *contents, size_t size, size_t nmemb, void *userData)
{
    std::string *ud = static_cast<std::string *>(userData);
    ud->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

static size_t ReadCb(void *contents, size_t size, size_t nmemb, void *userData)
{
    std::ifstream *file = static_cast<std::ifstream *>(userData);

    if (file->eof())
    {
        return 0;
    }
    size_t readSize = std::min(CHUNK_SIZE, size * nmemb);

    file->read(static_cast<char *>(contents), readSize);

    return file->gcount();
}