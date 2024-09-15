#pragma once
#include <any>
#include <functional>
#include <vector>
#include <string>

class ControllerHandler {
public:
    void subscribe(const std::function<void(const std::string&, const std::vector<std::any>&)> &receiver) const {
        views.push_back(receiver);
    }

    template<typename... Args>
    void next(const std::string &path, Args... args) const {
        const std::vector<std::any> params = {args...};
        for (const auto &receiver: views) {
            receiver(path, params);
        }
    }

private:
    mutable std::vector<std::function<void(const std::string&, const std::vector<std::any>&)>> views;
};