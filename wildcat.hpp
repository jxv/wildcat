#ifndef WILDCAT_HPP
#define WILDCAT_HPP

#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <experimental/optional>
#include <tuple>
#include <set>
#include <memory>
#include "time.hpp"

using std::experimental::optional;

enum class Class {
    Fr,
    So,
    Jr,
    Sr,
};

std::ostream &operator<<(std::ostream &os, const Class klass);

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
    Time time;
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

struct Finish {
    RunnerId runner_id;
    Time time;
    unsigned int score;
};

using Finishes = std::vector<Finish>;

struct Result {
    unsigned int place;
    TeamId team_id;
    Squad squad;
};

using Results = std::vector<Result>;

bool operator==(const Result &a, const Result &b);
bool operator>(const Result &a, const Result &b);
bool operator<(const Result &a, const Result &b);

struct Heat {
    enum class Tag {
        Single,
        Combined,
    };

    Tag tag;
    union {
        struct {
            Results* results;
            Finishes* finishes;
        } single;
        struct {
            Results* varsity_results;
            Finishes* varsity_finishes;
            Results* jv_results;
            Finishes* jv_finishes;
        } combined;
    };

    Heat();
    Heat(Tag);
    ~Heat();

    void set_single();
    void set_combined();
};

struct Wildcat {
    Runners runners;
    std::vector<RunnerId> barcodes;
    std::vector<float> times;
    Finishes finishes;
    Rosters rosters;
    Teams teams;
    Heat heat;
};

std::ostream &operator<<(std::ostream &os, const Wildcat &w);

bool import_rosters_v1(const std::string &roster_file, Rosters &rosters, Teams &teams, Runners &runners);
bool import_barcodes_v1(const std::string &barcode_file, std::vector<RunnerId> &barcodes);
bool import_times_v1(const std::string &times_file, std::vector<float> &times);
void make_finishes(const std::vector<float> &times, const std::vector<RunnerId> &barcodes, Finishes &finishes);
void separate_combined_heat(const Rosters &rosters, const Finishes &all, Finishes &varsity, Finishes &jv);
void score_race(const Runners &runners, const Teams &teams, const Rosters &rosters, Finishes &finishes, Results &results);
void print_results(Results &results, Teams &teams);

void output_results(std::ostream &os,
    const Rosters &rosters, const Teams &teams, const Runners &runners, const Results &results);
void score(Wildcat &w);

#endif
