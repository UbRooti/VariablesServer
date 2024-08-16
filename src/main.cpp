#include <iostream>
#include <crow_all.h>
#include "file_helper.hpp"
#include <unordered_map>
#include "json.hpp"
#include <filesystem>

const std::string DATA_PATH = "res/";

void create_data_dir()
{
    if (!std::filesystem::exists(DATA_PATH))
    {
        if (std::filesystem::create_directories(DATA_PATH))
        {
            std::cout << "Successfully created data directory\n";
        }
        else
        {
            std::cout << "Failed to create data directory\n";
        }
    }
}

struct Value
{
    std::string type;
    std::string name;
    std::string data;

    static nlohmann::json to_json(const Value& val)
    {
        nlohmann::json json;

        json["type"] = val.type;
        json["name"] = val.name;
        json["data"] = val.data;

        return json;
    }

    static Value from_json(const nlohmann::json& json)
    {
        Value val;

        if (!json.contains("name") || !json.contains("data") || !json.contains("type")) 
            return val;

        val.name = json["name"];
        val.type = json["type"];
        val.data = json["data"];

        return val;
    }

    std::string get_send_data()
    {
        nlohmann::json json;

        json["name"] = name;
        json["type"] = type;
        json["data"] = data;

        return json.dump();
    }

    operator bool()
    {
        return !type.empty() && !name.empty() && !data.empty();
    }
};

class VariableManager
{
    std::unordered_map<std::string, Value> values;
    std::string path;

public:
    VariableManager(const std::string& path = DATA_PATH + "variables.json")
    : path(path)
    {
        load();
    }
    
    ~VariableManager()
    {
        save();
    }

    void load()
    {
        bool result;
        std::string file = file_helper::read_file(path, result);

        if (!result)
        {
            std::cerr << "Failed to read file \"" << path << "\"\n";
            std::cerr << "Failed to load variables database. Creating new one\n";
            return;
        }

        for (nlohmann::json json : nlohmann::json::parse(file))
        {
            Value val = Value::from_json(json);
            if (val)
            {
                values.insert(std::pair<std::string, Value>(val.name, val));
            }
        }

        std::cout << "Successfully loaded variables database\n";
    }

    void save()
    {
        nlohmann::json json;

        for (auto obj : values)
        {
            if (obj.second)
            {
                json.push_back(obj.second.to_json(obj.second));
            }
        }

        if (file_helper::write_file(path, json.dump(4)))
        {
            std::cout << "Successfully saved variables database\n";
        }
        else
        {
            std::cerr << "Failed to save variables database\n";
        }
    }

    bool exists(const std::string& name)
    {
        return values.count(name) > 0;
    }

    std::string get_send_value(const std::string& name)
    {
        if (exists(name))
        {
            return values.find(name)->second.get_send_data();
        }
        else
        {
            return "failed";
        }
    }

    std::string get_data(const std::string& name)
    {
        if (exists(name))
        {
            return values.find(name)->second.data;
        }
        else
        {
            return "failed";
        }
    }

    std::string get_type(const std::string& name)
    {
        if (exists(name))
        {
            return values.find(name)->second.type;
        }
        else
        {
            return "failed";
        }
    }

    std::string get_list()
    {

        if (values.empty())
            return "empty";

        std::stringstream ss;

        for (std::pair<std::string, Value> p : values)
        {
            ss << p.first << "\n";
        }

        return ss.str();
    }

    Value get_value(const std::string& name)
    {
        if (!exists(name))
            return Value();
        return values.find(name)->second;
    }

    std::string set_value(const std::string& name, const std::string& data, const std::string& type)
    {
        auto it = values.find(name);

        if (it != values.end())
        {
            if (it->second.type.compare(type))
            {   
                it->second.data = data;
                return "success";
            }
            else
            {
                return "failed";
            }
        }
        else
        {
            Value val { type, name, data };

            values.insert(std::pair<std::string, Value>(val.name, val));
            return "success";
        }
    }

    bool remove(const std::string& name)
    {
        auto it = values.find(name);

        if (it != values.end())
        {
            values.erase(it);
            return true;
        }

        return false;
    }
};

class Config
{

    void set_default_settings()
    {
        port = 8080;
    }

private:
    int port;
    std::string auth_token;

    const std::string path;
public:
    Config(const std::string& path = DATA_PATH + "config.json")
    : path(path),
    port(8080), auth_token("")
    {
        load();
    }

    ~Config()
    {
        save();
    }

    void load()
    {
        bool result;
        std::string file = file_helper::read_file(path, result);

        if (!result)
        {
            std::cerr << "Failed to read file \"" << path << "\"\n";
            std::cerr << "Failed to load config. Using default settings\n";
            return;
        }

        nlohmann::json json = nlohmann::json::parse(file);

        if (json.empty())
        {
            set_default_settings();
        }

        if (json.contains("port"))
        {
            if (json["port"].is_number_integer())
            {
                port = json["port"];
            }
        }

        if (json.contains("auth_token"))
        {
            auth_token = json["auth_token"];
        }

        std::cout << "Successfully loaded config\n";
        if (anonymous_access())
        {
            std::cout << "!!! ANONYMOUS ACCESS ENABLED: to disable it you should set auth token in config file or run with \"--configure-auth\" argument !!!\n";
        }
    }

    void save()
    {
        nlohmann::json json;

        json["port"] = port;
        json["auth_token"] = auth_token;

        if (file_helper::write_file(path, json.dump(4)))
        {
            std::cout << "Successfully saved config\n";
        }
        else
        {
            std::cerr << "Failed to save config\n";
        }
    }

    void configure_auth()
    {
        std::cout << "Enter new auth token: ";
        std::string new_token;

        std::cin >> new_token;

        if (new_token != "")
        {
            auth_token = new_token;
        }
        
        std::cout << "New auth token: " << auth_token << "\n";

        save();
    }

    int get_port()
    {
        return port;
    }

    bool anonymous_access()
    {
        return auth_token.empty();
    }

    bool check_auth_token(const std::string& auth_token)
    {
        return this->auth_token == auth_token;
    }
};

int main(int argc, char* argv[])
{
    auto contains_arg = [&argc, &argv](const std::string& name)
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i] == name)
            {
                return true;
            }
        }

        return false;
    };

    create_data_dir();

    Config cfg;

    if (contains_arg("--configure-auth"))
    {
        cfg.configure_auth();
        return 0;
    }

    crow::SimpleApp app;
    VariableManager var_manager;

    CROW_ROUTE(app, "/get_object")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }
        
        auto name = req.url_params.get("name");

        if (!name)
        {
            return crow::response("missing_parameters");
        }

        return crow::response(var_manager.get_send_value(name));
    });

    CROW_ROUTE(app, "/get")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }
        
        auto name = req.url_params.get("name");

        if (!name)
        {
            return crow::response("missing_parameters");
        }

        return crow::response(var_manager.get_data(name));
    });
    
    CROW_ROUTE(app, "/get_type")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }
        
        auto name = req.url_params.get("name");

        if (!name)
        {
            return crow::response("missing_parameters");
        }

        return crow::response(var_manager.get_type(name));
    });

    CROW_ROUTE(app, "/exists")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }
        
        auto name = req.url_params.get("name");

        if (!name)
        {
            return crow::response("missing_parameters");
        }

        return crow::response(var_manager.exists(name) ? "true" : "false");
    });

    CROW_ROUTE(app, "/set")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }
        
        auto name = req.url_params.get("name");
        auto type = req.url_params.get("type");
        auto data = req.url_params.get("data");
        

        if (!name || !type || !data)
        {
            return crow::response("missing_parameters");
        }

        return crow::response(var_manager.set_value(name, data, type));
    });

    CROW_ROUTE(app, "/remove")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }

        auto name = req.url_params.get("name");

        if (!name)
        {
            return crow::response("missing_parameters");
        }

        return crow::response(var_manager.remove(name) ? "success" : "failed");
    });

    CROW_ROUTE(app, "/list")
    ([&var_manager, &cfg](const crow::request& req)
    {

        if (!cfg.anonymous_access())
        {
            auto token = req.url_params.get("auth_token");
            if (!token)
                return crow::response("no_access");
            else if (!cfg.check_auth_token(token)) 
                return crow::response("no_access");
        }

        return crow::response(var_manager.get_list());
    });

    std::cout << "Starting server on port: " << cfg.get_port() << "\n";

    app.port(cfg.get_port()).multithreaded().run();
    return 0;
}