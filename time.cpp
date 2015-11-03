#include <cmath>
#include "time.hpp"

float to_total_seconds(float minutes, float seconds, float ms) {
    return (60 * minutes) + seconds + (ms / 100);
}

Time::Time()
: Time(0, 0, 0)
{}

Time::Time(float total_seconds)
: total_seconds(total_seconds)
{
    minutes = static_cast<int>(std::floor(total_seconds / 60));
    seconds = static_cast<int>(std::floor(total_seconds - (minutes * 60)));
    ms = static_cast<int>((total_seconds - std::floor(total_seconds)) * 100);
}

Time::Time(int minutes, int seconds, int ms)
: minutes(minutes)
, seconds(seconds)
, ms(ms)
, total_seconds(to_total_seconds(minutes, seconds, ms))
{}

int Time::get_minutes() const {
    return minutes;
}

int Time::get_seconds() const {
    return seconds;
}

int Time::get_ms() const {
    return ms;
}

float Time::get_total_seconds() const {
    return total_seconds;
}

Time operator+(const Time &a, const Time &b) {
    const auto ms = a.get_ms() + b.get_ms();
    const auto seconds = a.get_seconds() + b.get_seconds() + (ms / 100);
    const auto minutes = a.get_minutes() + b.get_minutes() + (seconds / 60);
    return Time(minutes, seconds % 60, ms % 100);
}

std::ostream& operator<<(std::ostream &os, const Time &ft) {
    if (ft.minutes < 10) {
        os << '0';
    }
    os << ft.minutes << ':';

    if (ft.seconds < 10) {
        os << '0';
    }
    os << ft.seconds << '.';

    if (ft.ms < 10) {
        os << '0';
    }
    os << ft.ms;
    return os;
}
