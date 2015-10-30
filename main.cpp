#include "mainwindow.hpp"

/*
int main(int argc, char **argv) {

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.wildcat");
    MainWindow main_window;
    return app->run(main_window);
}
*/

int main() {
    Wildcat w;
    w.heat.set_combined();

    if (!import_rosters_v1("roster.txt", w.rosters, w.teams, w.runners))
        return EXIT_FAILURE;

    if (!import_barcodes_v1("barcodes.txt", w.barcodes))
        return EXIT_FAILURE;

    if (!import_times_v1("times.txt", w.times))
        return EXIT_FAILURE;

    make_finishes(w.times, w.barcodes, w.finishes);


    switch (w.heat.tag) {
    case Heat::Tag::Single:
        *w.heat.single.finishes = w.finishes;
        break;
    case Heat::Tag::Combined:
        separate_combined_heat(w.rosters, w.finishes,
            *w.heat.combined.varsity_finishes, *w.heat.combined.jv_finishes);
        break;
    }

    switch (w.heat.tag) {
    case Heat::Tag::Single:
        score_race(w.runners, w.teams, w.rosters, *w.heat.single.finishes, *w.heat.single.results);
        break;
    case Heat::Tag::Combined:
        score_race(w.runners, w.teams, w.rosters, *w.heat.combined.varsity_finishes, *w.heat.combined.varsity_results);
        score_race(w.runners, w.teams, w.rosters, *w.heat.combined.jv_finishes, *w.heat.combined.jv_results);
        break;
    }
 
    switch (w.heat.tag) {
    case Heat::Tag::Single: {
        break;
    }
    case Heat::Tag::Combined: {
        for (auto &finish : *w.heat.combined.varsity_finishes) {
            std::cout << finish.runner_id << ' ';
            std::cout << finish.time << '\n';
        }
        
        std::cout << "Varsity:\n"; 
       print_results(*w.heat.combined.varsity_results, w.teams);
        std::cout << '\n'; 

        for (auto &finish : *w.heat.combined.jv_finishes) {
            std::cout << finish.runner_id << ' ';
            std::cout << finish.time << '\n';
        }

        std::cout << "JV:\n"; 
        print_results(*w.heat.combined.jv_results, w.teams);

        break;
    }
    }

    std::cout << w;
    return EXIT_SUCCESS;
}
