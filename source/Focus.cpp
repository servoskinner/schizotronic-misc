#include <sztronics/miscellaneous/Focus.hpp>

Focus::Focus(Focus_scope& focus_space) : focus_space(focus_space)
{
    std::lock_guard<std::mutex> lock(focus_space.focus_mutex);

    if (focus_space.current_controller.has_value()) {
        control = false;
    }
    else {
        control = true;
        focus_space.current_controller.emplace(std::ref(*this));
    }
}

bool Focus::has_control()
{
    std::lock_guard<std::mutex> lock(focus_space.focus_mutex);
    return control;
}

std::optional<std::reference_wrapper<Focus>> Focus::Focus_scope::get_controller()
{
    std::lock_guard<std::mutex> lock(focus_mutex);
    return current_controller;
}

void Focus::claim_control()
{
    std::lock_guard<std::mutex> lock(focus_space.focus_mutex);

    if (!control) {
        if (focus_space.current_controller.has_value()) {
            focus_space.current_controller->get().control = false;
        }
        focus_space.current_controller.emplace(std::ref(*this));
        control = true;
    }
}

Focus& Focus::operator=(Focus&& other)
{
    std::optional<std::lock_guard<std::mutex>> lock(other.focus_space.focus_mutex);
    if (other.control) {
        lock.reset();
        claim_control();
    }
    
    return *this;
}

Focus::Focus(Focus&& other) : focus_space(other.focus_space)
{
    std::optional<std::lock_guard<std::mutex>> lock(other.focus_space.focus_mutex);
    if (other.control) {
        lock.reset();
        claim_control();
    }
}

