#include <curl/curl.h>

#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "auth.h"
#include "userCommands.h"
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

    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (!Authenticate(creds.first, creds.second))
    {
        return -5;
    }

    AuthToken token = GetAuthToken();
    UserDetails userDetails = GetUserDetails(token.accessToken);

    if (userDetails == UserDetails())
    {
        return -6;
    }

    // Map of file path and id
    std::unordered_map<std::string, std::string> files;

    while (true)
    {
        std::cout << "Checking for files" << std::endl;

        for (const auto &file : fs::directory_iterator(monitorPath))
        {
            if (fs::is_regular_file(file.status()))
            {
                if (files.find(file.path()) == files.end())
                {
                    if (TokenHasExpired())
                    {
                        if (!Authenticate(creds.first, creds.second))
                        {
                            return -5;
                        }

                        token = GetAuthToken();
                    }
                    else
                    {
                        if (ShouldRefreshToken())
                        {
                            if (!RefreshToken())
                            {
                                std::cerr << "Failed to refresh token" << std::endl;
                            }
                            else
                            {
                                token = GetAuthToken();
                            }
                        }
                    }
                    std::string fileId;
                    if (FileUpload(file.path(), userDetails.homeFolderID, token.accessToken, fileId))
                    {
                        files[file.path()] = fileId;
                    }
                    else
                        continue;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}