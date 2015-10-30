#include "mainwindow.hpp"

int main(int argc, char **argv) {

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.wildcat");
    MainWindow main_window;
    return app->run(main_window);
}

/*
int main() {
    Wildcat w;

    if (!import_rosters_v1("roster.txt", w.rosters, w.teams, w.runners))
        return EXIT_FAILURE;

    if (!import_barcodes_v1("barcodes.txt", w.barcodes))
        return EXIT_FAILURE;

    if (!import_times_v1("times.txt", w.times))
        return EXIT_FAILURE;

    make_finishes(w.times, w.barcodes, w.finishes);


    Finishes jv, varsity;
    separate_combined_heat(w.rosters, w.finishes, varsity, jv);
   
    for (auto &finish : varsity) {
        std::cout << finish.runner_id << ' ';
        std::cout << finish.finish_time << '\n';
    }
    
    std::cout << "Varsity:\n"; 
    score_race(w.runners, w.teams, w.rosters, varsity);
    std::cout << '\n'; 

    for (auto &finish : jv) {
        std::cout << finish.runner_id << ' ';
        std::cout << finish.finish_time << '\n';
    }

    std::cout << "JV:\n"; 
    score_race(w.runners, w.teams, w.rosters, jv);

    return EXIT_SUCCESS;
}
*/
