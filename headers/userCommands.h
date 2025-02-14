#pragma once
#include <string>

struct UserDetails
{
    std::string userId;
    unsigned long long homeFolderID;

    bool operator==(const UserDetails& other){
        return this->userId == other.userId && this->homeFolderID == other.homeFolderID;
    }
};

UserDetails GetUserDetails(const std::string &accessToken);