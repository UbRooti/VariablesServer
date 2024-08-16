#ifndef UR_REQUESTS_HPP
#define UR_REQUESTS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <curl/curl.h>

namespace requests {
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
    {
        size_t total_size = size * nmemb;
        userp->append(static_cast<char*>(contents), total_size);
        return total_size;
    }

    std::string UrlEncode(const std::string& value) {
        std::ostringstream encoded;
        for (unsigned char c : value) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded << c;
            } else {
                encoded << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)c;
            }
        }
        return encoded.str();
    }

    std::string GetResponseFromUrl(const std::string& url) 
    {
        CURL* curl;
        CURLcode res;
        std::string response_string;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                response_string = "Error";
            }

            curl_easy_cleanup(curl);
        } else {
            std::cerr << "Failed to initialize curl" << std::endl;
            response_string = "Error";
        }

        return response_string;
    }

    // Функция для выполнения POST-запроса
    std::string PostRequest(const std::string& url, const std::string& post_data) 
    {
        CURL* curl;
        CURLcode res;
        std::string response_string;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                response_string = "Error";
            }

            curl_easy_cleanup(curl);
        } else {
            std::cerr << "Failed to initialize curl" << std::endl;
            response_string = "Error";
        }

        return response_string;
    }

    class Request
    {
        std::unordered_map<std::string, std::string> params;
        std::string req_url;
    public:
        Request(const std::string& url)
        : req_url(url)
        {}

        void add_param(const std::string& key, const std::string& value)
        {
            if (params.count(key) > 0) return;
            params.insert(std::make_pair(key, value));
        }

        std::string send()
        {
            std::stringstream ss;
            ss << req_url;

            if (!params.empty())
            {
                bool first = true;
                for (const auto& p : params)
                {
                    if (first) {
                        ss << "?";
                        first = false;
                    } else {
                        ss << "&";
                    }

                    ss << UrlEncode(p.first) << "=" << UrlEncode(p.second);
                }
            }

            return GetResponseFromUrl(ss.str());
        }

        std::string send_post()
        {
            std::stringstream post_data;
            bool first = true;
            for (const auto& p : params)
            {
                if (first) {
                    first = false;
                } else {
                    post_data << "&";
                }

                post_data << UrlEncode(p.first) << "=" << UrlEncode(p.second);
            }

            return PostRequest(req_url, post_data.str());
        }
    };
}

#endif // UR_REQUESTS_HPP