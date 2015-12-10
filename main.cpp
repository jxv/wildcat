#include "mainwindow.hpp"

/*
int main(int argc, char **argv) {
    const auto flags = 
        SDL_INIT_JOYSTICK |
        SDL_INIT_AUDIO;
    if (SDL_Init(flags) == -1) {
        return EXIT_FAILURE;
        std::cerr << "Can't start SDL2\n";
    }
    
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        std::cerr << "Can't open audio\n";
        return EXIT_FAILURE;
    }

    if (SDL_NumJoysticks() < 1) {
        std::cerr << "No joystick\n";
    }

    auto js = SDL_JoystickOpen(0);
    auto beep = Mix_LoadWAV("beep.wav");

    if (!beep) {
        std::cerr << "Can't open \"beep.wav\"\n";
        return EXIT_FAILURE;
    }

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.wildcat");
    MainWindow main_window(js, beep);
    auto val =  app->run(main_window);

    Mix_CloseAudio();
    SDL_Quit();
    return val;
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
    score(w);


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

    // exported results
    if (true) {
        std::ofstream file;
        file.open("both_results.txt");
        if (!file.is_open()) {
            std::cerr << "Can't open \"both_results.txt\"\n";
        } else {
            file << w;
        }
        file.close();
    }

    return EXIT_SUCCESS;
}


void print_results(Wildcat &w) {
    switch (w.heat.tag) {
    case Heat::Tag::Single: {
        for (auto &finish : *w.heat.single.finishes) {
            std::cout << finish.runner_id << ' ';
            std::cout << finish.time << '\n';
        }
        std::cout << "Varsity:\n"; 
        print_results(*w.heat.single.results, w.teams);
        std::cout << '\n'; 
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
}
