#ifndef SUDOKU_H
#define SUDOKU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <ctime>
#include <cstdlib>
#include <vector>

#define COTE 27

enum {FOUND,NOT_FOUND,FIXED,START,HELP};

class Sudoku
{
private:
    SDL_Window* window;
    SDL_Renderer *renderer;
    bool redo;
    int cur_choice;
    std::vector<std::vector<int>> grid, grid_copy, boolean;
    std::vector<int> tmp;
    SDL_Rect cur_pos, choice_pos;
    SDL_Texture **number, **fixed_number, **loading_screen, **cursor, *start, *choice, *dialog, *error, *good, *ending, *help;
public:
    Sudoku();
    ~Sudoku();
    void startGame();
    void launch();
    void generateGrid();
    bool checkRedo();
    bool check(int n);
    bool check3x3(int n, int i, int j);
    void generateDiag(int i_min, int i_max, int j_min, int j_max);
    void generateNotDiag(int i_min, int i_max, int j_min, int j_max);
    bool checkLine(int n, int j);
    bool checkRow(int n, int i);
    void copyGrid();
    void gridCleanup();
    void grid3x3Cleanup(int i_min, int i_max, int j_min, int j_max);
    void setBoolean();
    void showGridCopy();
    void initTmp();
    void showGrid();
    void moveCursor();
    bool endGame();
    void insertNumber(int i, int j);
    void reload();
    void showHelper();

};

#endif
