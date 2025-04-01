#pragma once

#include <chrono>
#include <vector>
#include <functional>
#include <optional>
#include <mutex>

/// @brief Class that can schedule certain events or execute them periodically.
class Timer
{
    private:
    static float get_cur_time_s();
    float prev_time, delta_time;

    public:
    Timer();

    /// @brief Event executed with certain timing or periodicity.
    struct Timed_event
    {
        std::function<void(void)> event = {};
        std::optional<std::reference_wrapper<std::mutex>> mutex; /// The mutex to lock when triggering event.
        float t_seconds = 0.0;
        float countdown = 0.0;
        int n_repeat = -1;

        /// @brief Creates a Timed_event.
        /// @param event The function to call when event happens.
        /// @param t_seconds Period between event calls in seconds.
        /// @param n_repeat How many times to fire the event. Set to -1 to repeat it infinitely.
        Timed_event(std::function<void(void)> event, float t_seconds, int n_repeat = -1);
    };

    void process();
    std::vector<Timed_event> events;
    bool skip_stalled;
};