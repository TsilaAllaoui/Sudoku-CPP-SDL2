#ifndef SUDOKU_H
#define SUDOKU_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <ctime>
#include <cstdlib>
#include <vector>

#define COTE 9

using namespace std;

enum{FOUND,NOT_FOUND,FIXED,START,HELP};

class Sudoku
{
private:
    SDL_Surface *screen;
    bool redo;
    int cur_choice;
    vector < vector < int > >grid, grid_copy, boolean;
    vector < int > tmp;
    SDL_Rect cur_pos, choice_pos;
   SDL_Surface **number, **fixed_number, **loading_screen, **cursor, *start, *choice, *dialog, *error, *good, *ending, *help;
public:
    Sudoku();
    ~Sudoku();
    void start_game();
    void launch();
    void generate_grid();
    bool check_redo();
    bool check(int n);
    bool check_3x3(int n, int i, int j);
    void generate_diag(int i_min, int i_max, int j_min, int j_max);
    void generate_not_diag(int i_min, int i_max, int j_min, int j_max);
    bool check_line(int n, int j);
    bool check_row(int n, int i);
    void loading();
    void grid_copy_grid();
    void grid_cleanup();
    void grid_3x3_cleanup(int i_min, int i_max, int j_min, int j_max);
    void set_boolean();
    void show_grid_copy();
    void init_tmp();
    void show_grid();
    void move_cursor();
    bool end_game();
    void insert_nb(int i, int j);
    void reload();
    void show_help();

};

#endif
