#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <filesystem>

#include "Finally.h"
#include "userCommands.h"
#include "util.h"

UserDetails GetUserDetails(const std::string &accessToken)
{
    // Http Response code
    long responseCode;

    // Response code
    CURLcode result;

    // Response
    std::string response;

    CURL *curl = curl_easy_init();
    struct curl_slist *headers = nullptr;

    Finally finally([curl, headers]()
                    {
        if (curl)
        {
            if (headers)
            {
                curl_slist_free_all(headers);
            }
            curl_easy_cleanup(curl);
        } });

    std::string url = BASE_URL + "api/v1/users/self";

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ResponseCb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        result = curl_easy_perform(curl);

        if (result == CURLE_OK)
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
            if (responseCode == 200)
            {
                try
                {
                    nlohmann::json parsedResponse = nlohmann::json::parse(response);
                    UserDetails ud;
                    ud.homeFolderID = parsedResponse["homeFolderID"];
                    ud.userId = parsedResponse["id"];
                    return ud;
                }
                catch (const nlohmann::json::parse_error &exc)
                {
                    std::cerr << "Failed to parse response from getting user details: " << exc.what() << std::endl;
                    return {};
                }
            }
            else
            {
                std::cerr << "Getting user details response status: " << responseCode << std::endl;
                return {};
            }
        }
        else
        {
            std::cerr << "Curl request failed: " << curl_easy_strerror(result) << std::endl;
            return {};
        }
    }
    else
    {
        std::cerr << "Failed to create a curl request for getting user details" << std::endl;
        return {};
    }
}

bool FileUpload(const std::string &path, unsigned long long folderId, const std::string &accessToken, std::string &fileId)
{
    // Http Response code
    long responseCode;

    // Response code
    CURLcode result;

    // Response
    std::string response;

    CURL *curl = curl_easy_init();
    struct curl_slist *headers = nullptr;
    curl_mime *mime = nullptr;

    Finally finally([curl, headers, mime]()
                    {
        if (curl)
        {
            if (headers)
            {
                curl_slist_free_all(headers);
            }

            if (mime) {
                curl_mime_free(mime);
            }
            curl_easy_cleanup(curl);
        } });

    std::string url = BASE_URL + "/api/v1/folders/" + std::to_string(folderId) + "/files";

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        std::string authHeader = "Authorization: Bearer " + accessToken;

        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        mime = curl_mime_init(curl);
        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filename(part, std::filesystem::path(path).filename().string().c_str());
        curl_mime_filedata(part, path.c_str());

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
        {
            std::cerr << "Failed to open file " << path << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCb);
        curl_easy_setopt(curl, CURLOPT_READDATA, &file);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ResponseCb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        result = curl_easy_perform(curl);

        if (result == CURLE_OK)
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
            if (responseCode == 201 || responseCode == 200)
            {
                try
                {
                    nlohmann::json parsedResponse = nlohmann::json::parse(response);
                    fileId = parsedResponse["id"];
                    return true;
                }
                catch (const nlohmann::json::parse_error &exc)
                {
                    std::cerr << "Failed to parse response from uploading file: " << exc.what() << std::endl;
                    return false;
                }
            }
            else if (responseCode == 409)
            {
                std::cout << path << " has already been uploaded to MOVEit" << std::endl;

                /*
                 *  Return true here, so we don't try to upload it again. We know that this file
                 *  has already been uploaded so there's no point in trying further.
                 *
                 *  A possible improvement for the future is to create a GetFile function, that will get the file id,
                 *  so we can fill it in here, but for now, we don't need it.
                 */
                return true;
            }
            else
            {
                std::cerr << "Uploading file response status: " << responseCode << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Curl upload failed: " << curl_easy_strerror(result) << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "Failed to upload file" << std::endl;
        return false;
    }
    return false;
}