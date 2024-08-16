# Simple Variable Management Server

## About the Project

This project is a simple HTTP server built with C++ using the Crow framework. It allows users to manage a set of variables stored as JSON objects. The server provides a RESTful API to get, set, delete, and list these variables. It also includes a basic configuration management system.

## Why Choose This Project

- **Lightweight:** The server is built with minimal dependencies, making it lightweight and efficient.
- **Simple API:** The RESTful API provided by the server is easy to use and integrate with other systems.
- **Configurable:** The server can be easily configured using a JSON configuration file.
- **Extensible:** The codebase is simple and can be easily extended to add more features or adapt to specific needs.

## Dependencies

```bash
sudo apt update
sudo apt install git cmake libasio-dev build-essential
```
## Building server
The server is now only available on linux because it has been prepared for use with Raspberry Pi
### Linux:
```bash
git clone https://github.com/UbRooti/VariablesServer
cd VariablesServer
chmod +x build.sh
./build.sh
```

After compiling the server, an executable file should appear in the `bin/` directory.

## Configuration

Before running the server, you may want to configure it. The server uses a configuration file (`config.json`) located in the `res/` directory by default. The configuration file allows you to set the server port and authentication token.

## Example `config.json`

```json
{
    "port": 8080,
    "auth_token": "your_auth_token_here"
}
```

    port: The port on which the server will listen. Default is 8080.
    auth_token: The token required for authentication. If this is left empty, the server will allow anonymous access.

If you want to set up an authentication token after starting the server, you can run the program with the --configure-auth flag:

    ./server --configure-auth

This will prompt you to enter a new authentication token.
Usage
Running the Server

To run the server, simply execute the compiled binary. The server will start on the port specified in the configuration file.

    ./server

RESTful API Endpoints

### 1. Get a Variable

Request:
```http
GET /get?name=<variable_name>
```
Response:

    On success: Returns the data associated with the variable.
    On failure: Returns "failed".

### 2. Set a Variable

Request:
```http
GET /set?name=<variable_name>&type=<data_type>&data=<your_data>
```
Response:

    On success: Returns "success".
    On failure: Returns "failed".

### 3. Check if a Variable Exists

Request:
```http
GET /exists?name=<variable_name>
```
Response:

    "true" if the variable exists.
    "false" if the variable does not exist.

### 4. Delete a Variable

Request:
```http
GET /remove?name=<variable_name>
```
Response:

    On success: Returns "success".
    On failure: Returns "failed".

### 5. List All Variables

Request:
```http
GET /list
```
Response:

    Returns a list of all variable names.

### Authentication

If an authentication token is set in the config.json file, all requests must include the token as a URL parameter:
```http
?auth_token=<your_auth_token>
```
Or server will return `no_access`

If no token is set, the server will allow anonymous access.