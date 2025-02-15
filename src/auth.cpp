#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <chrono>

#include "Finally.h"
#include "auth.h"
#include "util.h"

static AuthToken authToken;

namespace
{
    unsigned long long GetTimeNowSeconds()
    {
        const auto now = std::chrono::system_clock::now();

        const auto epoch = now.time_since_epoch();

        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);

        return seconds.count();
    }

    /*
     * This is a helper utility function that sends a POST request using
     * user provided post arguments. It's meant to be used for authentication
     * thus not setting any headers.
     */
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

bool RefreshToken()
{
    if (authToken.refreshToken.empty())
        return false;

    std::string postArgs = "refresh_token=" + authToken.refreshToken + "&grant_type=refresh_token";
    return AuthHelper(postArgs);
}

bool TokenHasExpired()
{
    unsigned long long timeNow = GetTimeNowSeconds();
    return authToken.acquiredAt + authToken.expirationTime <= timeNow;
}

bool ShouldRefreshToken()
{
    // Token can only be refreshed before it has expired.
    if (TokenHasExpired())
    {
        return false;
    }

    /* If we check the token for refresh in the first half of it's lifetime, this function
     * will return false. If the check is done during the second half of it's lifetime, it will
     * return true.
     */

    unsigned long long timeNow = GetTimeNowSeconds();
    unsigned long long mid = authToken.acquiredAt + authToken.expirationTime / 2;
    unsigned long long end = authToken.acquiredAt + authToken.expirationTime;

    /*
     * end - 10 so we don't get into the case where timeNow is
     * end - 1 because the token may expire while we make a request
     */

    return mid <= timeNow && timeNow < end - 10;
}

AuthToken GetAuthToken()
{
    return authToken;
}