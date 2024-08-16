#ifndef FILE_HELPER_HPP
#define FILE_HELPER_HPP

#include <string>
#include <fstream>
#include <iostream>

class file_helper {
public:
    static std::string read_file(const std::string &file_name) {
        std::ifstream file(file_name);
        if (!file.is_open()) {
            std::cerr << "No such file or directory.\n";
            return "";
        }
        std::string line;
        std::string data;

        while (getline(file, line)) {
            data += line + "\n";
        }

        file.close();
        return data;
    }
    
    static std::string read_file(const std::string &file_name, bool& result) {
        std::ifstream file(file_name);
        if (!file.is_open()) {
            std::cerr << "No such file or directory.\n";
            result = false;
            return "";
        }
        std::string line;
        std::string data;

        while (getline(file, line)) {
            data += line + "\n";
        }

        file.close();
        result = true;
        return data;
    }

    static bool write_file(const std::string &file_name, const std::string& data) {
        std::ofstream file(file_name);
        if (!file.is_open()) {
            std::cerr << "No such file or directory.\n";
            return false;
        }

        file << data;

        if (!file.good())
            return false;

        file.close();

        return true;
    }
};

#endif // FILE_HELPER_HPP