#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <chrono>

#include "Finally.h"
#include "auth.h"

const std::string BASE_URL = "https://testserver.moveitcloud.com/";

static AuthToken authToken;

namespace
{
    size_t ResponseCb(void *contents, size_t size, size_t nmemb, void *userData)
    {
        std::string *ud = static_cast<std::string *>(userData);
        ud->append(static_cast<char *>(contents), size * nmemb);
        return size * nmemb;
    }

    unsigned long long GetTimeNowSeconds()
    {
        const auto now = std::chrono::system_clock::now();

        const auto epoch = now.time_since_epoch();

        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);

        return seconds.count();
    }

    bool AuthHelper(const std::string &postArgs)
    {
        // Http Response code
        long responseCode;

        // Response code
        CURLcode result;

        // Response
        std::string response;

        CURL *curl = curl_easy_init();

        Finally finally([curl]()
                        {
        if (curl) {
            curl_easy_cleanup(curl);
        } });

        std::string url = BASE_URL + "api/v1/token";

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postArgs.c_str());

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
                        authToken.accessToken = parsedResponse["access_token"];
                        authToken.refreshToken = parsedResponse["refresh_token"];
                        authToken.expirationTime = parsedResponse["expires_in"];
                        authToken.acquiredAt = GetTimeNowSeconds();

                        RefreshToken(authToken.refreshToken);
                    }
                    catch (const nlohmann::json::parse_error &exc)
                    {
                        std::cerr << "Failed to parse response from authentication: " << exc.what() << std::endl;
                        return false;
                    }

                    return true;
                }
                else
                {
                    std::cerr << "Authentication response status: " << responseCode << std::endl;
                    return false;
                }
            }
            else
            {
                std::cerr << "Curl request failed: " << curl_easy_strerror(result) << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Failed to create a curl authenticate request" << std::endl;
            return false;
        }
    }
}

bool Authenticate(const std::string &username, const std::string &password)
{
    std::string postArgs = "username=" + username + "&password=" + password + "&grant_type=password";
    return AuthHelper(postArgs);
}

bool RefreshToken(const std::string &refreshToken)
{
    std::string postArgs = "refresh_token=" + refreshToken + "&grant_type=refresh_token";
    return AuthHelper(postArgs);
}

AuthToken GetAuthToken()
{
    return authToken;
}