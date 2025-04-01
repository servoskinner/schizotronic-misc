#include <sztronics/miscellaneous/Timer.hpp>

Timer::Timed_event::Timed_event(std::function<void(void)> event, float t_seconds, int n_repeat = -1) : 
    event(event), t_seconds(t_seconds), \                                                                                         
    countdown(t_seconds), mutex(), \                                                                             
    n_repeat(n_repeat) {}



float Timer::get_cur_time_s() {
    auto now = std::chrono::steady_clock::now();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    return static_cast<float>(nanoseconds)/1000000000.0;
}



Timer::Timer() : events(), delta_time(0), \
                 prev_time(get_cur_time_s()), skip_stalled(true) {}


                 
void Timer::process()
{
    float cur_time = get_cur_time_s();

    delta_time = cur_time - prev_time;

    for (int ev_id = 0; ev_id < events.size(); ev_id++)
    {
        events[ev_id].countdown -= delta_time;
        while(events[ev_id].countdown <= 0.0)
        {
            // Lock mutex if provided
            std::optional<std::lock_guard<std::mutex>> opt_lock;
            if(events[ev_id].mutex.has_value()) {
                opt_lock.emplace(events[ev_id].mutex->get());
            }

            if (events[ev_id].n_repeat > 0) {
                events[ev_id].n_repeat--;
            }
            else if (events[ev_id].n_repeat == 0) {
                events.erase(events.begin() + ev_id);
                ev_id--;
                break;
            }
            
            events[ev_id].event();

            if (skip_stalled) {
                events[ev_id].countdown = events[ev_id].t_seconds;
            }
            else {
                events[ev_id].countdown += events[ev_id].t_seconds;
            }
        }
    }
    prev_time = cur_time;
}