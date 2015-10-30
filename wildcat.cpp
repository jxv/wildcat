#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <experimental/optional>
#include <tuple>
#include "wildcat.hpp"

std::ostream &operator<<(std::ostream &os, const Class klass) {
    switch (klass) {
    case Class::Fr:
        os << "Fr.";
        break;
    case Class::So:
        os << "So.";
        break;
    case Class::Jr:
        os << "Jr.";
        break;
    case Class::Sr:
        os << "Sr.";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream &os, const Runner &r) {
    os << "{";
    os << "\"name\": \"" << r.name << "\"";

    if (r.klass) {
        os << ", ";
        os << "\"class\": ";
        switch (*r.klass) {
        case Class::Fr: os << "\"Fr\""; break;
        case Class::So: os << "\"So\""; break;
        case Class::Jr: os << "\"Jr\""; break;
        case Class::Sr: os << "\"Sr\""; break;
        }
    }

    if (r.gender) {
        os << ", ";
        os << "\"gender\": ";
        switch (*r.gender) {
        case Gender::F: os << "\"F\""; break;
        case Gender::M: os << "\"M\""; break;
        }
    }

    os << "}";
    
    return os;
}

bool operator==(const Squad &a, const Squad &b) {
    return a.score == b.score;
}

bool operator>(const Squad &a, const Squad &b) {
    if (a.score == 0 && b.score == 0) {
        return false;
    } else if (a.score > 0 && b.score == 0) {
        return true;
    } else if (a.score == 0 && b.score > 0) {
        return false;
    } else {
        if (a.score != b.score) {
            return a.score < b.score;
        } else {
            // 6th vs 6th
            if (a.places.size() >= 6 &&
                b.places.size() >= 6 &&
                a.places[5].place_number != b.places[5].place_number) {
                return a.places[5].place_number < b.places[5].place_number;
            }

            // 7th vs 7th
            if (a.places.size() == 7 &&
                b.places.size() == 7 &&
                a.places[6].place_number != b.places[6].place_number) {
                return a.places[6].place_number < b.places[6].place_number;
            }

            // last resort: 5th vs 5th
            return a.places[4].place_number < b.places[4].place_number;
        }
    }
}

bool operator<(const Squad &a, const Squad &b) {
    if (b.score == 0 && a.score == 0) {
        return false;
    } else if (b.score > 0 && a.score == 0) {
        return true;
    } else if (b.score == 0 && a.score > 0) {
        return false;
    } else {
        return b > a;
    }
}

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

bool import_rosters_v1(const std::string &roster_file, Rosters &rosters, Teams &teams, Runners &runners) {

    rosters.runner_to_team.clear();
    rosters.team_to_runners.clear();
    teams.clear();
    runners.clear();

    std::ifstream file;
    file.open(roster_file);

    if (!file.is_open()) {
            std::cerr << "import_rosters_v1(): No file \"" << roster_file << "\"\n";
        return false;
    }

    while (!file.eof()) {
        std::string token;
        RunnerId runner_id;
        Runner runner;

        if (!std::getline(file, token, '\t')) // id
            break; // HACK: exit, if eof
        try {
            runner_id = std::stoi(token);
        }
        catch (...) {
            std::cerr << "import_rosters_v1() with \"" << roster_file << "\": \"" << token << "\" must be a integer.\n";
            return false;
        }

        std::getline(file, token, '\t'); // name
        runner.name = token;

        std::getline(file, token, '\t'); // team
        // sorta complex logic to update teams and rosters
        {
            bool team_exists = false;
            TeamId team_id; // will be assigned in for-each or following if-stmt
            for (auto &team : teams) {
                if (team.second.initials == token) {
                    team_exists = true;
                    team_id = team.first;
                    break;
                }
            }

            if (!team_exists) {
                // add new team to teams
                Team team;
                team.initials = token; 
                team_id = teams.size();

                teams.insert(std::pair<TeamId, Team>(team_id, team));
              
                // add new team into roster
                rosters.team_to_runners.insert(std::pair<TeamId, std::vector<RunnerId>>(team_id, {}));
            }

            rosters.runner_to_team.insert(std::pair<RunnerId, TeamId>(runner_id, team_id));
            rosters.team_to_runners[team_id].push_back(runner_id);
        }

        std::getline(file, token, '\t'); // klass
        try {
            switch (std::stoi(token)) {
            case 9: runner.klass = Class::Fr; break;
            case 10: runner.klass = Class::So; break;
            case 11: runner.klass = Class::Jr; break;
            case 12: runner.klass = Class::Sr; break;
            default:
                std::cerr << "import_rosters_v1() with \"" << roster_file << "\": \"" << token << "\" must be between [9,12]\n";
                return false;
            }
        }
        catch(...) {
        }

        std::getline(file, token); // gender
        if (token == "G" || token == "g" || token == "F" || token == "f") {
            runner.gender = Gender::F;
        } else if (token == "B" || token == "b" || token == "M" || token == "m") {
            runner.gender = Gender::M;
        }
        
        runners.insert(std::pair<RunnerId, Runner>(runner_id, runner));
    }

    return true;
}

bool import_barcodes_v1(const std::string &barcode_file, std::vector<RunnerId> &barcodes) {

    barcodes.clear();

    std::ifstream file;
    file.open(barcode_file);
    
    if (!file.is_open()) {
            std::cerr << "import_barcodes_v1(): No file \"" << barcode_file << "\"\n";
        return false;
    }

    while (!file.eof()) {
        std::string token;
        if (!std::getline(file, token))
            break;
        try {
            barcodes.push_back(std::stoi(token));
        }
        catch(...) {
            std::cerr << "import_barcodes_v1() with \"" << barcode_file << "\": \"" << token << "\" Not a number\n";
            return false;
        }
    }

    return true;
}

bool import_times_v1(const std::string &times_file, std::vector<float> &times) {

    times.clear();

    std::ifstream file;
    std::string token;
    file.open(times_file);

    if (!file.is_open()) {
            std::cerr << "import_times_v1(): No file \"" << times_file << "\"\n";
        return false;
    }

    while (file.is_open()) {
        if (!std::getline(file, token, '\t')) break;
        std::getline(file, token, '\t');
        std::getline(file, token, '\t');
        std::getline(file, token, '\t');
        std::getline(file, token, '\t');
        std::getline(file, token, '\t');
        std::getline(file, token, '\n');

        try {
            times.push_back(std::stof(token));
        }
        catch(...) {
            std::cerr << "import_times_v1() with \"" << times_file << "\": \"" << token << "\" Not a timestamp\n";
            return false;
        }           
    }

    return true;
}

void make_finishes(const std::vector<float> &times, const std::vector<RunnerId> &barcodes, Finishes &finishes) {

    finishes.clear();

    for (auto i = 0; i < times.size() && i < barcodes.size(); i++) {
        finishes.push_back({ barcodes[i], Time(times[i]), 0 });
    }
}

void separate_combined_heat(const Rosters &rosters, const Finishes &all, Finishes &varsity, Finishes &jv) {

    varsity.clear();
    jv.clear();
   
    std::map<TeamId, unsigned int> finished;

    for (auto &finish : all) {
        auto team_id = rosters.runner_to_team.at(finish.runner_id);
        if (finished.count(team_id) == 0) {
            finished[team_id] = 0;
        }
        if (finished[team_id] < 7) {
            finished[team_id]++;
            varsity.push_back(finish);
            continue;
        }
        jv.push_back(finish);
    }
}

bool operator==(const Result &a, const Result &b) {
    return a.squad == b.squad;
}

bool operator>(const Result &a, const Result &b) {
    return a.squad > b.squad;
}

bool operator<(const Result &a, const Result &b) {
    return a.squad < b.squad;
}

void score_race(const Runners &runners, const Teams &teams, const Rosters &rosters, Finishes &finishes, Results &results) {

    results.clear();

    Squads squads;

    for (auto &team : teams) {
        squads.insert(std::pair<TeamId, Squad>(team.first, {0, Time(0), {}}));
    }

    // Fill up squads
    for (unsigned int i = 0; i < finishes.size(); i++) {
        auto const &finish = finishes[i];
        auto team_id = rosters.runner_to_team.at(finish.runner_id);
        
        if (squads.count(team_id)) {
            const auto place_number = i + 1;
            squads[team_id].places.push_back({
                .runner_id = finish.runner_id,
                .place_number = place_number,
            });
        }
    }

    // Sum times
    for (auto &squad : squads) {
        if (squad.second.places.size() < 5) {
            continue;
        }
        for (auto i = 0; i < 5; i++) {
            const auto &place_number = squad.second.places[i].place_number;
            squad.second.time = squad.second.time + finishes[place_number - 1].time;
        }
    }
   
    // Calc score 
    std::set<RunnerId> scorers;
    for (auto &squad : squads) {
        if (squad.second.places.size() < 5) {
            continue;
        }
        for (auto i = 0; i < 7 && i < squad.second.places.size(); i++) {
            scorers.insert(squad.second.places[i].runner_id);
        }
    }
    unsigned int score_num = 1;
    for (auto &finish : finishes) {
        if (!scorers.count(finish.runner_id)) {
            continue;
        }
        finish.score = score_num;

        // add score to squad
        auto &squad = squads.at(rosters.runner_to_team.at(finish.runner_id));
        for (auto i = 0; i < 5; i++) {
            auto &place = squad.places.at(i);
            if (place.runner_id == finish.runner_id) {
                squad.score += score_num;
                break;
            }
        }

        score_num++;
    }

    std::priority_queue<Result> queue;
    for (auto &squad : squads) {
        queue.push({
            .place = 0,
            .team_id = squad.first,
            .squad = squad.second,
        });
    }

    unsigned int place = 1;
    while (queue.size()) {
        auto result = queue.top();
        result.place = place;
        if (result.squad.score != 0) {
            place++;
        }
        results.push_back(result);
        queue.pop();
    }
}

void print_results(Results &results, Teams &teams) {
    for (auto &result : results) {
        std::cout << result.place;
        std::cout << ' ';
        std::cout << teams.at(result.team_id).initials;
        std::cout << " - ";
        std::cout << result.squad.score;
        std::cout << " - ";
        std::cout << result.squad.time;
        std::cout << " -";

        for (auto i = 0; i < 5 && i < result.squad.places.size(); i++) {
            std::cout << ' ' << result.squad.places[i].place_number;
        }
   
        if (result.squad.places.size() >= 6) { 
            std::cout << " (";
        }

        if (5 < result.squad.places.size()) {
            std::cout << result.squad.places[5].place_number;
        }

        if (6 < result.squad.places.size()) {
            std::cout << ' ';
            std::cout << result.squad.places[6].place_number;
        }

        if (result.squad.places.size() >= 6) { 
            std::cout << ")";
        }

        std::cout << '\n';
    }
}

Heat::Heat()
: Heat(Tag::Single)
{}
    
Heat::Heat(Tag tag)
: tag(tag)
{
    switch (tag) {
    case Tag::Single:
        single.results = new Results;
        single.finishes = new Finishes;
        break;
    case Tag::Combined:
        combined.varsity_results = new Results;
        combined.varsity_finishes = new Finishes;
        combined.jv_results = new Results;
        combined.jv_finishes = new Finishes;
        break;
    }
}

Heat::~Heat() {
    switch (tag) {
    case Tag::Single:
        delete single.results;
        delete single.finishes;
        break;
    case Tag::Combined:
        delete combined.varsity_results;
        delete combined.varsity_finishes;
        delete combined.jv_results;
        delete combined.jv_finishes;
        break;
    }
}

void Heat::set_single() {
    switch (tag) {
    case Tag::Single:
        break;
    case Tag::Combined:
        delete combined.varsity_results;
        delete combined.varsity_finishes;
        delete combined.jv_results;
        delete combined.jv_finishes;
        single.results = new Results;
        single.finishes = new Finishes;
        break;
    } 
    tag = Tag::Single;
}

void Heat::set_combined() {
    switch (tag) {
    case Tag::Single:
        delete single.results;
        delete single.finishes;
        combined.varsity_results = new Results;
        combined.varsity_finishes = new Finishes;
        combined.jv_results = new Results;
        combined.jv_finishes = new Finishes;
        break;
    case Tag::Combined:
        break;
    }
    tag = Tag::Combined;
}

void output_results(std::ostream &os,
        const Rosters &rosters, const Teams &teams, const Runners &runners, const Finishes &finishes, const Results &results) {
    os << '\n';
    os << "RACE #_______________________ DIV #___________________________\n";
    os << '\n';
    os << "INDIVIDUALS\n";
    os << "==================================================================================\n";
    os << '\n';
    os << "Place  Team             Name                              Grade  Time        Score\n"; 
    os << "----------------------------------------------------------------------------------\n";

    auto place = 1;
    for (auto &finish : finishes) {
        std::stringstream ss;
        auto extend = [&] (unsigned int limit) {
            for (auto i = ss.str().length(); i < limit; i++) {
                ss << ' ';
            }
        };
        //
        ss << place;
        extend(7);
        ss << teams.at(rosters.runner_to_team.at(finish.runner_id)).initials;
        extend(24);
        ss << runners.at(finish.runner_id).name;
        extend(58);
        auto klass = runners.at(finish.runner_id).klass;
        if (klass) {
            ss << *klass;
        }
        extend(65);
        ss << finish.time;
        extend(77);
        if (finish.score) {
            ss << finish.score;
        }
        //
        place++;
        os << ss.str();
        os << '\n';
    }
 
    os << "==================================================================================\n";
    os << '\n';
    os << '\n';
    os << "TEAM SCORES\n";
    os << "==================================================================================\n";
    os << '\n';

    for (auto &result : results) {
        std::stringstream ss;
        //
        ss << '#';
        ss << result.place;
        ss << ' ';
        ss << teams.at(result.team_id).initials;
        ss << "\n   ";

        for (auto i = 0; i < 5 && i < result.squad.places.size(); i++) {
            ss << ' ' << result.squad.places[i].place_number;
        }
   
        if (result.squad.places.size() >= 6) { 
            ss << " (";
        }

        if (5 < result.squad.places.size()) {
            ss << result.squad.places[5].place_number;
        }

        if (6 < result.squad.places.size()) {
            ss << ' ';
            ss << result.squad.places[6].place_number;
        }

        if (result.squad.places.size() >= 6) { 
            ss << ")";
        }

        if (result.squad.score) {
            ss << " = ";
            ss << result.squad.score;
        }

        if (result.squad.score) {
            ss << "\n    ";
            ss << result.squad.time;
        }

        //
        ss << '\n';
        ss << '\n';
        os << ss.str();
    }

    os << '\n';
    os << "==================================================================================\n";
    os << '\n';
}

std::ostream &operator<<(std::ostream &os, const Wildcat &w) {

    switch (w.heat.tag) {
    case Heat::Tag::Single:
        output_results(os, w.rosters, w.teams, w.runners, *w.heat.single.finishes, *w.heat.single.results);
        break;
    case Heat::Tag::Combined:
        output_results(os, w.rosters, w.teams, w.runners, *w.heat.combined.varsity_finishes, *w.heat.combined.varsity_results);
        os << '\n';
        output_results(os, w.rosters, w.teams, w.runners, *w.heat.combined.jv_finishes, *w.heat.combined.jv_results);
        break;
    }
    os << '\n';
    os << "			Wildcat Timing & Scoring System © 2010-2015\n";
    os << '\n';
    return os;
}
