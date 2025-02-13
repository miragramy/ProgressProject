#pragma once
#include <string>

const std::string BASE_URL = "https://testserver.moveitcloud.com/";

static size_t ResponseCb(void *contents, size_t size, size_t nmemb, void *userData)
{
    std::string *ud = static_cast<std::string *>(userData);
    ud->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}