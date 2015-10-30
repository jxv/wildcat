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

FinishTime::FinishTime(float total_seconds)
: total_seconds(total_seconds)
{
    minutes = static_cast<int>(std::floor(total_seconds / 60));
    seconds = static_cast<int>(std::floor(total_seconds - (minutes * 60)));
    ms = static_cast<int>((total_seconds - std::floor(total_seconds)) * 100);
}

FinishTime::FinishTime(int minutes, int seconds, int ms)
: minutes(minutes)
, seconds(seconds)
, ms(ms)
, total_seconds(to_total_seconds(minutes, seconds, ms))
{}

int FinishTime::get_minutes() const {
    return minutes;
}

int FinishTime::get_seconds() const {
    return seconds;
}

int FinishTime::get_ms() const {
    return ms;
}

float FinishTime::get_total_seconds() const {
    return total_seconds;
}

std::ostream& operator<<(std::ostream &os, const FinishTime &ft) {
    if (ft.minutes > 0) {
        os << ft.minutes << ':';
    }

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
        finishes.push_back({ barcodes[i], FinishTime(times[i]) });
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

void score_race(const Runners &runners, const Teams &teams, const Rosters &rosters, const Finishes &finishes) {

    Squads squads;

    for (auto &team : teams) {
        squads.insert(std::pair<TeamId, Squad>(team.first, {0, {}}));
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

    for (auto &squad : squads) {
        if (squad.second.places.size() < 5) continue;
        auto score = 0;
        for (auto i = 0; i < 5; i++) {
            score += squad.second.places[i].place_number;
        }
        squad.second.score = score;
    }

    std::priority_queue<std::tuple<Squad, TeamId>> queue;
    for (auto &squad : squads) {
        queue.push(std::tuple<Squad, TeamId>(squad.second, squad.first));
    }

    std::vector<std::tuple<Squad, TeamId>> results;
    while (queue.size()) {
        results.push_back(queue.top());
        queue.pop();
    }

    int i = 1;
    for (auto &result : results) {
        std::cout << i << ' ';
        std::cout << teams.at(std::get<1>(result)).initials;
        std::cout << " - ";
        std::cout << std::get<0>(result).score;
        std::cout << '\n';
        if (std::get<0>(result).score != 0) {
            i++;
        }
    }
}