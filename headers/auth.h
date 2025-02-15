#pragma once
#include <string>
/*
 * Struct that holds information regarding authentication info.
 */
struct AuthToken
{
    std::string accessToken;
    std::string refreshToken;
    unsigned int expirationTime;
    unsigned long long acquiredAt;
};

bool Authenticate(const std::string &username, const std::string &password);
bool RefreshToken();

bool TokenHasExpired();
bool ShouldRefreshToken();

AuthToken GetAuthToken();