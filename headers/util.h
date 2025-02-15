#pragma once
#include <string>
#include <fstream>

const std::string BASE_URL = "https://testserver.moveitcloud.com/";

// Max allowed size per chunk per the documentation.
const size_t CHUNK_SIZE = 4 * 1024 * 1024;

/*
 * This callback is used with libcurl's CURLOPT_WRITEFUNCTION option.
 * The main purpose of this callback is to get the response from the request
 * and write it to a user passed pointer.
 */
static size_t ResponseCb(void *contents, size_t size, size_t nmemb, void *userData)
{
    std::string *ud = static_cast<std::string *>(userData);
    ud->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

/*
 * This callback is used with libcurl's CURLOPT_READFUNCTION option.
 * The main purpose of this callback is to pass a stream of data to libcurl,
 * when it's uploading data.
 */
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

/*
 * Function that reads a file and gets the credentials from it.
 */
static std::pair<std::string, std::string> GetCreds(std::ifstream &file)
{
    std::string password, username, text;

    getline(file, text);

    int index = 0;
    while (index < text.size() && text[index] != ':')
    {
        username += text[index++];
    }

    if (index >= text.size())
    {
        return {};
    }

    index += 1;

    while (index < text.size())
    {
        password += text[index++];
    }

    if (password.empty())
    {
        return {};
    }

    return {username, password};
}