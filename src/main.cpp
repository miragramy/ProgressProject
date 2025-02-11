#include <curl/curl.h>
#include <iostream>

int main() {
    // Initialize libcurl
    CURL *curl = curl_easy_init();
    if(curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");

        // Perform the request and capture the result
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Request successful!" << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "libcurl initialization failed!" << std::endl;
    }

    return 0;
}