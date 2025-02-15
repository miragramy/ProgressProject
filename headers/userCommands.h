#pragma once
#include <string>

/*
 * Struct that holds information regarding the current user.
 */
struct UserDetails
{
    std::string userId;
    unsigned long long homeFolderID;

    bool operator==(const UserDetails &other)
    {
        return this->userId == other.userId && this->homeFolderID == other.homeFolderID;
    }
};

UserDetails GetUserDetails(const std::string &accessToken);

bool FileUpload(const std::string &path, unsigned long long folderId, const std::string &accessToken, std::string &fileId);