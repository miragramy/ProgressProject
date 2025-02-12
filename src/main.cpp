#include <curl/curl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::pair<std::string, std::string> getCreds(std::ifstream &file)
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

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "You need to pass 2 arguments: path to credential file and path to directory to monitor" << std::endl;
        return -1;
    }

    std::string credsPath = argv[1];
    std::string monitorPath = argv[2];

    std::ifstream credentialsFile(credsPath);

    if (!credentialsFile.is_open())
    {
        std::cerr << "Failed to open credentials file" << std::endl;
        return -2;
    }

    std::pair<std::string, std::string> creds = getCreds(credentialsFile);

    if (creds == std::pair<std::string, std::string>{})
    {
        std::cerr << "Invalid credentials file format. It should contain one line in the format <user>:<password>" << std::endl;
        return -3;
    }

    if (!fs::exists(monitorPath) || !fs::is_directory(monitorPath))
    {
        std::cerr << "Invalid directory path for monitoring" << std::endl;
        return -4;
    }

    std::cout << creds.first << " " << creds.second << std::endl;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_global_cleanup();
    return 0;
}