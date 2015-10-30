#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "wildcat.hpp"
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/window.h>
#include <gdkmm/rgba.h>
#include <SDL.h>

enum RaceListColumn : guint {
    RLC_PLACE = 0,
    RLC_SCORE,
    RLC_TIME,
    RLC_TEAM,
    RLC_NAME,
    RLC_PLACE_NUMBERS = RLC_NAME,
    RLC_COUNT
};

class MainWindow : public Gtk::Window {
public:
    MainWindow();
    virtual ~MainWindow();

protected:
    void on_start_button_clicked();
    void on_stop_button_clicked();
    void on_load_config_button_clicked();
    void on_load_roster_button_clicked();
    void on_load_barcodes_button_clicked();
    void on_load_results_button_clicked();
    void on_export_results_button_clicked();
    void on_pretty_print_results_button_clicked();

private:
    Gtk::Paned main_divider;

    Gtk::Notebook notebook;

    Gtk::ScrolledWindow race_page;
    Gtk::ListViewText race_list;

    Gtk::Grid config_page;
    Gtk::Button load_config_button;
    Gtk::Button load_roster_button;
    Gtk::VBox config_box;

    Gtk::Grid results_page;
    Gtk::Button load_barcodes_button;
    Gtk::Button load_results_button;
    Gtk::Button export_results_button;
    Gtk::Button pretty_print_results_button;
    Gtk::VBox results_box;

    Gtk::VBox right_vbox;
    
    Gtk::Frame race_time_frame;
    Gtk::VBox race_time_vbox;
    Gtk::Box race_time_space;
    Gtk::HBox race_time_button_divider;
    Gtk::Label race_time_label;
    Gtk::Button start_button;
    Gtk::Button stop_button;

    Gtk::MessageDialog stop_race_dialog;

    Gtk::ListViewText results_list;
    Gtk::Frame results_frame;

    Gtk::MessageDialog quit_dialog;
};

#endif
