#ifndef INTERFACE_MODULE
#define INTERFACE_MODULE

#include <string>

class Module1 {
public:
    virtual ~Module1() = default;
    virtual std::string Command(const std::string& command) = NULL;
    virtual void StartModule(const std::string& command, std::function<void(const std::string&)> callback) = 0;
};

#endif
