#include "mainwindow.hpp"

MainWindow::MainWindow()
: load_config_button("Load Config")
, load_roster_button("Load Roster")
, load_barcodes_button("Load Barcodes")
, load_results_button("Load Results")
, export_results_button("Export Results")
, pretty_print_results_button("Pretty Print Results")
, start_button("Start")
, stop_button("Stop")
, race_time_label("00:00.0")
, race_list(RLC_COUNT)
, results_list(RLC_COUNT)
, stop_race_dialog(*this, "Stop the race timer?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL)
, quit_dialog(*this, "Are you sure you want to quit?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO)
{
    set_border_width(10);
    add(main_divider);

    // race page
    {
        race_list.set_column_title(RLC_PLACE, "Place");
        race_list.set_column_title(RLC_TEAM, "Team");
        race_list.set_column_title(RLC_NAME, "Name");
        race_list.set_column_title(RLC_TIME, "Time");
        race_list.set_column_title(RLC_SCORE, "Score");

        for (int i = 0; i < 100; i++) {
            auto row = race_list.append();
            race_list.set_text(row, RLC_PLACE, "_");
            race_list.set_text(row, RLC_TEAM, "_");
            race_list.set_text(row, RLC_NAME, "_");
            race_list.set_text(row, RLC_TIME, "_");
            race_list.set_text(row, RLC_SCORE, "_");
        }

        race_page.add(race_list);
    }

    // config page
    {
        load_config_button.set_border_width(5);
        load_config_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_load_config_button_clicked));
        config_box.pack_start(load_config_button, Gtk::PACK_SHRINK);
        
        load_roster_button.set_border_width(5);
        load_roster_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_load_roster_button_clicked));
        config_box.pack_start(load_roster_button, Gtk::PACK_SHRINK);

        config_box.set_border_width(5);
        config_page.add(config_box);
    }

    // results page
    {
        load_barcodes_button.set_border_width(5);
        load_barcodes_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_load_barcodes_button_clicked));
        results_box.pack_start(load_barcodes_button, Gtk::PACK_SHRINK);

        load_results_button.set_border_width(5);
        load_results_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_load_results_button_clicked));
        results_box.pack_start(load_results_button, Gtk::PACK_SHRINK);
        
        export_results_button.set_border_width(5);
        export_results_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_export_results_button_clicked));
        results_box.pack_start(export_results_button, Gtk::PACK_SHRINK);
        
        pretty_print_results_button.set_border_width(5);
        pretty_print_results_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_pretty_print_results_button_clicked));
        results_box.pack_start(pretty_print_results_button, Gtk::PACK_SHRINK);

        results_box.set_border_width(5);
        results_page.add(results_box);
    }

    // notebook
    {
        notebook.append_page(race_page, "Race");
        notebook.append_page(config_page, "Config");
        notebook.append_page(results_page, "Results");
        notebook.set_border_width(5);
        main_divider.add1(notebook);
    }
   
    // start button 
    {
        start_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_start_button_clicked));
        Gdk::RGBA start_color;
        start_color.set_rgba(0, 0.7, 0);
        start_button.override_color(start_color);
        start_button.set_border_width(5);
    }

    // stop button
    {
        stop_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_stop_button_clicked));
        Gdk::RGBA stop_color;
        stop_color.set_rgba(0.7, 0, 0);
        stop_button.override_color(stop_color);
        stop_button.set_border_width(5);
    }

    // race time frame
    {
        race_time_button_divider.add(start_button);
        race_time_button_divider.add(stop_button);
        
        race_time_vbox.add(race_time_label);
        race_time_vbox.add(race_time_button_divider);

        race_time_frame.set_label("Race Time");
        race_time_frame.add(race_time_vbox);

        results_list.set_column_title(RLC_PLACE, "Place");
        results_list.set_column_title(RLC_TEAM, "Team");
        results_list.set_column_title(RLC_PLACE_NUMBERS, "Runners");
        results_list.set_column_title(RLC_TIME, "Time");
        results_list.set_column_title(RLC_SCORE, "Score");

        results_frame.set_label("Results");
        results_frame.add(results_list);

        right_vbox.pack_start(race_time_frame, Gtk::PACK_SHRINK);
        right_vbox.pack_end(results_frame, Gtk::PACK_EXPAND_WIDGET);
        right_vbox.set_border_width(5);
        
        main_divider.pack2(right_vbox, true, true);
    }

    // stop race dialog
    {
        stop_race_dialog.set_title("Alert!");
    }

    show_all_children();
    results_frame.hide();
}

MainWindow::~MainWindow() {}

void MainWindow::on_load_config_button_clicked() {
    std::cout << "load config\n";
}

void MainWindow::on_load_roster_button_clicked() {
    std::cout << "load roster\n";
}

void MainWindow::on_start_button_clicked() {
    std::cout << "start\n";
    results_frame.show();
}

void MainWindow::on_stop_button_clicked() {
    std::cout << "stop?\n";
    auto ok_or_cancel = stop_race_dialog.run();
    switch (ok_or_cancel) {
    // stop!
    case -5:
        std::cout << "stop\n";
        break;
    // don't stop
    case -6:
        std::cout << "don't stop\n";
        break;
    default: break;
    }
    stop_race_dialog.hide();
}

void MainWindow::on_load_barcodes_button_clicked() {
    std::cout << "load barcodes\n";
}

void MainWindow::on_load_results_button_clicked() {
    std::cout << "load results\n";
}

void MainWindow::on_export_results_button_clicked() {
    std::cout << "export results\n";
}

void MainWindow::on_pretty_print_results_button_clicked() {
    std::cout << "pretty print results\n";
}
