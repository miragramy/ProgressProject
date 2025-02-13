#pragma once
#include <string>

struct UserDetails
{
    std::string userId;
    unsigned long long homeFolderID;
};

UserDetails GetUserDetails(const std::string &accessToken);