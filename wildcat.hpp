#ifndef WILDCAT_HPP
#define WILDCAT_HPP

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <experimental/optional>
#include <tuple>

using std::experimental::optional;

enum class Class {
    Fr,
    So,
    Jr,
    Sr,
};

enum class Gender {
    F,
    M,
};

struct Runner {
    std::string name;
    optional<Class> klass;
    optional<Gender> gender;
};

std::ostream& operator<<(std::ostream &os, const Runner &r);

using TeamId = int;

struct Team {
    std::string initials;
    std::string name;
    std::string location;
};

using Teams = std::map<TeamId, Team>;

using RunnerId = int;

struct Place {
    RunnerId runner_id;
    unsigned int place_number;
};

struct Squad {
    unsigned int score;
    std::vector<Place> places;
};

bool operator==(const Squad &a, const Squad &b);
bool operator>(const Squad &a, const Squad &b);
bool operator<(const Squad &a, const Squad &b);

using Squads = std::map<TeamId, Squad>;

using Runners = std::map<RunnerId, Runner>;

struct Rosters {
    std::map<RunnerId, TeamId> runner_to_team;
    std::map<TeamId, std::vector<RunnerId>> team_to_runners;
};

float to_total_seconds(float minutes, float seconds, float ms);

class FinishTime {
public:
    FinishTime(float total_seconds);
    FinishTime(int minutes = 0, int seconds = 0, int ms = 0);
    int get_minutes() const;
    int get_seconds() const;
    int get_ms() const;
    float get_total_seconds() const;
private:
    int minutes;
    int seconds;
    int ms;
    float total_seconds;
    friend std::ostream& operator<<(std::ostream &os, const FinishTime &ft);
};

std::ostream& operator<<(std::ostream &os, const FinishTime &ft);

struct Finish {
    RunnerId runner_id;
    FinishTime finish_time;
};

using Finishes = std::vector<Finish>;

bool import_rosters_v1(const std::string &roster_file, Rosters &rosters, Teams &teams, Runners &runners);
bool import_barcodes_v1(const std::string &barcode_file, std::vector<RunnerId> &barcodes);
bool import_times_v1(const std::string &times_file, std::vector<float> &times);
void make_finishes(const std::vector<float> &times, const std::vector<RunnerId> &barcodes, Finishes &finishes);
void separate_combined_heat(const Rosters &rosters, const Finishes &all, Finishes &varsity, Finishes &jv);
void score_race(const Runners &runners, const Teams &teams, const Rosters &rosters, const Finishes &finishes);

struct Wildcat {
    Runners runners;
    std::vector<RunnerId> barcodes;
    std::vector<float> times;
    Finishes finishes;
    Rosters rosters;
    Teams teams;
};

#endif
