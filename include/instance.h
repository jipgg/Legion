#pragma once
#include <cstddef>
#include <memory>
#include <string>

class instance {
    size_t id_;
    std::weak_ptr<instance> parent_;
    std::shared_ptr<instance> children_;
    std::string class_name_;
public:
    std::string name;
    virtual ~instance() = default;
    instance(const std::string& class_name = "Instance"); 
};

