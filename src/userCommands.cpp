#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>

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
    struct curl_slist* headers = nullptr;

    Finally finally([curl, headers]()
                    {
        if (curl) {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } });

    std::string url = BASE_URL + "api/v1/users/self";

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string authHeader = "Authorization: Bearer "+ accessToken;
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
                    std::cout<<ud.homeFolderID<<" "<<ud.userId<<std::endl;
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
