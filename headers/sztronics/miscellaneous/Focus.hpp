#pragma once

#include <optional>
#include <mutex>
#include <memory>

class Focus
{
    public:
    class Focus_scope
    {
        public:
        std::mutex focus_mutex;
        std::optional<std::reference_wrapper<Focus>> current_controller;
        std::optional<std::reference_wrapper<Focus>> get_controller();
    };

    static Focus_scope global_scope;

    Focus(Focus_scope& focus_space = global_scope);
    bool has_control();
    void claim_control();

    Focus& operator= (Focus&& other);
    Focus(Focus&& other);
    
    private:
    bool control;
    Focus(const Focus& other) = delete;
    Focus& operator= (Focus& other) = delete;
    Focus_scope& focus_space;    
};