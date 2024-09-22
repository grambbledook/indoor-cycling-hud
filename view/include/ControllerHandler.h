#pragma once
#include <any>
#include <functional>
#include <vector>
#include <string>

class ControllerHandler {
public:
    auto subscribe(
        const std::function<void(const std::string &, const std::vector<std::any> &)> &receiver) const -> void {
        views.push_back(receiver);
    }

    template<typename... Args>
    auto next(const std::string &path, Args... args) const -> void {
        const std::vector<std::any> params = {args...};
        for (const auto &receiver: views) {
            receiver(path, params);
        }
    }

private:
    mutable std::vector<std::function<void(const std::string &, const std::vector<std::any> &)> > views;
};
