#ifndef TIME_HPP
#define TIME_HPP

#include <iostream>

float to_total_seconds(float minutes, float seconds, float ms);

class Time {
public:
    Time();
    Time(float total_seconds);
    Time(int minutes, int seconds, int ms = 0);
    int get_minutes() const;
    int get_seconds() const;
    int get_ms() const;
    float get_total_seconds() const;
private:
    int minutes;
    int seconds;
    int ms;
    float total_seconds;
    friend std::ostream& operator<<(std::ostream &os, const Time &ft);
};

std::ostream& operator<<(std::ostream &os, const Time &ft);
Time operator+(const Time &a, const Time &b);

#endif
