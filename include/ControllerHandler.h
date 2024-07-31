#pragma once
#include <memory>
#include <utility>

class ControllerHandler {
public:
    void subscribe(const std::function<void(std::string)> &receiver) const {
        views.push_back(receiver);
    }

    void next(const std::string &path) const {
        for (const auto &receiver: views) {
            receiver(path);
        }
    }

private:
    mutable std::vector<std::function<void(std::string)> > views;
};
