#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "sudoku.h"

#define SIZE 650
#define S_SIZE 70

Sudoku::Sudoku()
{
    // Init SDL Video
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "Error initializing SDL App.";
        exit(1);
    }

    // Creating window and renderer
    window = SDL_CreateWindow("Sudoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZE, SIZE, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Seting up attributes
    srand(time(0));
    cur_pos.x = 5;
    cur_pos.y = 5;
    cur_pos.h = S_SIZE;
    cur_pos.w = S_SIZE;
    choice_pos.h = 30;
    choice_pos.w = 30;
    cur_choice = START;
    choice_pos.x = 0; 0;
    choice_pos.h = 30;
    choice_pos.w = 30;
    redo = true;

    // Loading images
    start = IMG_LoadTexture(renderer, "./images/start.png");

    number = new SDL_Texture*[10];
    for (int i = 0; i < 10; i++) {
        std::string path = "./images/" + std::to_string(i) + ".png";
        number[i] = IMG_LoadTexture(renderer, path.c_str());
    }

    fixed_number = new SDL_Texture *[10];
    for (int i = 0; i < 10; i++) {
        std::string path = "./images/" + std::to_string(i) + "_fixe.png";
        fixed_number[i] = IMG_LoadTexture(renderer, path.c_str());
    }

    loading_screen = new SDL_Texture *[5];
    for (int i = 0; i < 5; i++) {
        std::string path;
        if (i > 0)
            path = "./images/loading" + std::to_string(i) + ".png";
        else path = "./images/loading.png";
        loading_screen[i] = IMG_LoadTexture(renderer, path.c_str());
    }

    cursor = new SDL_Texture *[2];
    cursor[0] = IMG_LoadTexture(renderer,"./images/cursor.png");
    cursor[1] = IMG_LoadTexture(renderer,"./images/cursor_selected.png");

    choice = IMG_LoadTexture(renderer,"./images/choice.png");
    help = IMG_LoadTexture(renderer,"./images/help.png");
    ending = IMG_LoadTexture(renderer,"./images/end.png");
    dialog = IMG_LoadTexture(renderer,"./images/dialog.png");
    error = IMG_LoadTexture(renderer,"./images/error.png");
    good = IMG_LoadTexture(renderer,"./images/good.png");
}

Sudoku::~Sudoku()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(start);
    delete[](loading_screen);
    delete[](cursor);
    delete[](number);
    delete[](fixed_number);
    redo = false;
}

void Sudoku::startGame()
{
    SDL_Event event;
    bool begin = false;
    for (int i = 0; i < 9; i++)
    {
        grid.push_back(std::vector < int >(9, 0));
        grid_copy.push_back(std::vector < int >(9, 0));
        boolean.push_back(std::vector < int >(9,NOT_FOUND));
    }
    while (!begin)
    {
    	if (cur_choice == START)
		{
            choice_pos.x = 25; choice_pos.y = SIZE * 0.70;
		}

		else if (cur_choice == HELP)
		{
			choice_pos.x = 25; choice_pos.y = SIZE * 0.80;
		}

        SDL_RenderCopy(renderer, start, nullptr, nullptr);
        SDL_RenderCopy(renderer, choice, nullptr, &choice_pos);
        SDL_RenderPresent(renderer);

        SDL_WaitEvent(&event);
        switch (event.type)
		{
		case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_DOWN:
					if (cur_choice == START)
						cur_choice = HELP;
					break;
				case SDLK_UP:
					if (cur_choice == HELP)
						cur_choice = START;
					break;
				case SDLK_SPACE:
					if (cur_choice == START)
						begin = true;
					else if (cur_choice == HELP)
					{
                        showHelper();
					}
					break;
			}
			}
		}
    }
}

void Sudoku::launch()
{
    while (redo)
    {
        startGame();
        generateGrid();
        copyGrid();
        gridCleanup();
        setBoolean();
        showGridCopy();
        moveCursor();
        SDL_Delay(1000);
        reload();
    }
    this->~Sudoku();
}

void Sudoku::generateGrid()
{
    bool first = true;
    auto renderLoad = [&](const int& i) {
        SDL_RenderCopy(renderer, loading_screen[first ? i : 4], nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(30);
    };
    while (checkRedo())
    {
        grid = std::vector<std::vector<int>>(9, std::vector<int>(9, 0));
        tmp.clear();
        renderLoad(0);
        generateDiag(0, 3, 0, 3);
        generateDiag(3, 6, 3, 6);
        renderLoad(1);
        generateDiag(6, 9, 6, 9);
        generateNotDiag(0, 3, 3, 6);
        renderLoad(2);
        generateNotDiag(0, 3, 6, 9);
        generateNotDiag(3, 6, 6, 9);
        renderLoad(3);
        generateNotDiag(3, 6, 0, 3);
        generateNotDiag(6, 9, 0, 3);
        renderLoad(4);
        generateNotDiag(6, 9, 3, 6);
        first = false;
    }
    fprintf(stderr,"out\n");
}

bool Sudoku::checkRedo()
{
    for (auto& i : grid) {
        if (std::find(i.begin(), i.end(), 0) != i.end())
            return true;
    }
    return false;
}

void Sudoku::generateDiag(int i_min, int i_max, int j_min, int j_max)
{
    for (int i = i_min; i < i_max; i++)
    {
        for (int j = j_min; j < j_max; j++)
        {
            int n;
            do
            {
                n = rand() % 9 + 1;
            }
            while (check(n) || check3x3(n, i, j));
            grid[i][j] = n;
            tmp.push_back(n);
        }
    }
    tmp.clear();
}

bool Sudoku::check(int n)
{
    if (std::find(tmp.begin(), tmp.end(), n) != tmp.end())
        return true;
    return false;
}

bool Sudoku::check3x3(int n, int i, int j)
{
    if (j >= 0 && j <= 2 && i >= 0 && i <= 2)
    {
        for (int k = 0; k <= 2; k++)
        {
            for (int l = 0; l <= 2; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    if (j >= 3 && j <= 5 && i >= 0 && i <= 2)
    {
        for (int k = 0; k <= 2; k++)
        {
            for (int l = 3; l <= 5; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 6 && j <= 8 && i >= 0 && i <= 2)
    {
        for (int k = 0; k <= 2; k++)
        {
            for (int l = 6; l <= 8; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 0 && j <= 2 && i >= 3 && i <= 5)
    {
        for (int k = 3; k <= 5; k++)
        {
            for (int l = 0; l <= 2; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 3 && j <= 5 && i >= 3 && i <= 5)
    {
        for (int k = 3; k <= 5; k++)
        {
            for (int l = 3; l <= 5; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 6 && j <= 8 && i >= 3 && i <= 5)
    {
        for (int k = 3; k <= 5; k++)
        {
            for (int l = 6; l <= 8; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 0 && j <= 2 && i >= 6 && i <= 8)
    {
        for (int k = 6; k <= 8; k++)
        {
            for (int l = 0; l <= 2; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 3 && j <= 5 && i >= 6 && i <= 8)
    {
        for (int k = 6; k <= 8; k++)
        {
            for (int l = 3; l <= 5; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
    else if (j >= 6 && j <= 8 && i >= 6 && i <= 8)
    {
        for (int k = 6; k <= 8; k++)
        {
            for (int l = 6; l <= 8; l++)
                if (grid[k][l] == n)
                    return true;
        }
        return false;
    }
}

void Sudoku::generateNotDiag(int i_min, int i_max, int j_min, int j_max)
{
    initTmp();
    int counts = 0;
    for (int i = i_min; i < i_max; i++)
    {
        int n, counter = 0;
        for (int j = j_min; j < j_max; j++)
        {
            do
            {
                if (counts >= 30)
                    return;
                n = rand() % 9 + 1;
                counter++;
                if (counter >= 30)
                {
                    tmp.clear();
                    for (int i = i_min; i < i_max; i++)
                        for (int j = j_min; j < j_max; j++)
                            grid[i][j] = 0;
                    j = j_min;
                    i = i_min;
                    counter = 0;
                    counts++;
                }
            }
            while (check(n) || check3x3(n, i, j)
                    || checkLine(n, i) || checkRow(n, j));
            grid[i][j] = n;
            tmp.push_back(n);
        }
        counter = 0;
    }
}

bool Sudoku::checkLine(int n,int i)
{
    for (int j = 0; j < 9; j++)
        if (grid[i][j] == n)
            return true;
    return false;
}

bool Sudoku::checkRow(int n, int j)
{
    for (int i = 0; i < 9; i++)
        if (grid[i][j] == n)
            return true;
    return false;
}

void Sudoku::copyGrid()
{
    grid_copy = grid;
}

void Sudoku::gridCleanup()
{
   for (int i = 0; i < 9; i += 3)
        for (int j = 0; j < 9; j += 3)
            grid3x3Cleanup(i, i + 2, j, j + 2);
}

void Sudoku::grid3x3Cleanup(int i_min, int i_max, int j_min, int j_max)
{
    int counter = 0, nb_max;
    nb_max = 9 - (rand() % 3) + 1;
    do
    {
        int x = rand() % (i_max - i_min + 1) + i_min;
        int y = rand() % (j_max - j_min + 1) + j_min;
        grid_copy[x][y] = 0;
        counter++;
    }
    while (counter <= nb_max);
}

void Sudoku::setBoolean()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            if (grid_copy[i][j] == 0)
                boolean[i][j] = NOT_FOUND;
            else
                boolean[i][j] = FIXED;
        }
}

void Sudoku::initTmp()
{
    tmp.clear();
    for (int i = 1; i <= 9; i++)
        tmp.push_back(i);
}

void Sudoku::showGridCopy()
{
    Uint8 r = 0, g = 255, b = 0, a = 255;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    SDL_RenderFillRect(renderer, nullptr);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            SDL_Rect pos;
            pos.h = S_SIZE;
            pos.w = S_SIZE;

            if (j < 3)
                pos.x = j * S_SIZE + 5;
            else
            {
                if (j >= 6)
                    pos.x = j * S_SIZE + 15;
                else
                    pos.x = j * S_SIZE + 10;
            }
            if (i < 3)
                pos.y = i * S_SIZE + 5;
            else
            {
                if (i >= 6)
                    pos.y = i * S_SIZE + 15;
                else
                    pos.y = i * S_SIZE + 10;
            }

            for (int k = 0; k <= 9; k++) {
                if (k == 0) {
                    if (grid_copy[i][j] == k)
                        SDL_RenderCopy(renderer, number[0], nullptr, &pos);
                }
                else {
                    if (grid_copy[i][j] == k)
                    {
                        if (boolean[i][j] == FIXED)
                            SDL_RenderCopy(renderer, fixed_number[k], nullptr, &pos);
                        if (boolean[i][j] != FIXED)
                            SDL_RenderCopy(renderer, number[k], nullptr, &pos);
                    }
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void Sudoku::showGrid()
{
    Uint8 r = 0, g = 255, b = 0, a = 255;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    SDL_RenderFillRect(renderer, nullptr);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            SDL_Rect pos;
            pos.h = S_SIZE;
            pos.w = S_SIZE;
            if (j < 3)
                pos.x = j * S_SIZE + 5;
            else
            {
                if (j >= 6)
                    pos.x = j * S_SIZE + 15;
                else
                    pos.x = j * S_SIZE + 10;
            }
            if (i < 3)
                pos.y = i * S_SIZE + 5;
            else
            {
                if (i >= 6)
                    pos.y = i * S_SIZE + 15;
                else
                    pos.y = i * S_SIZE + 10;
            }

            for (int k = 0; k <= 9; k++) {
                if (grid[i][j] == k) {
                    SDL_RenderCopy(renderer, number[k], nullptr, &pos);
                    break;
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void Sudoku::moveCursor()
{
    int i = cur_pos.y / S_SIZE, j = cur_pos.x / S_SIZE;
    SDL_RenderCopy(renderer, cursor[0], nullptr, &cur_pos);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool finished = false;
    while (!finished)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            {
                this->~Sudoku();
                exit(0);
            }
            case SDLK_UP:
            {
                if (cur_pos.y - S_SIZE > 0)
                {
                    if (cur_pos.y == 6 * S_SIZE + 15 || cur_pos.y == 3 * S_SIZE + 10)
                        cur_pos.y -= S_SIZE + 5;
                    else
                        cur_pos.y -= S_SIZE;
                    i--;
                }
                break;
            }
            case SDLK_DOWN:
            {
                if (cur_pos.y + S_SIZE <= 9 * S_SIZE)
                {
                    if (cur_pos.y == 2 * S_SIZE + 5 || cur_pos.y == 5 * S_SIZE + 10)
                        cur_pos.y += S_SIZE + 5;
                    else
                        cur_pos.y += S_SIZE;
                    i++;
                }
                break;
            }
            case SDLK_LEFT:
            {
                if (cur_pos.x - S_SIZE > 0)
                {
                    if (cur_pos.x == 6 * S_SIZE + 15 || cur_pos.x == 3 * S_SIZE + 10)
                        cur_pos.x -= S_SIZE + 5;
                    else
                        cur_pos.x -= S_SIZE;
                    j--;
                }
                break;
            }
            case SDLK_RIGHT:
            {
                if (cur_pos.x + S_SIZE <= 9 * S_SIZE)
                {
                    if (cur_pos.x == 2 * S_SIZE + 5 || cur_pos.x == 5 * S_SIZE + 10)
                        cur_pos.x += S_SIZE + 5;
                    else
                        cur_pos.x += S_SIZE;
                    j++;
                }
                break;
            }
            case SDLK_SPACE:
            {
                // Inserting number to cell 
                if (boolean[i][j] != FIXED)
                {
                    SDL_RenderCopy(renderer, cursor[1], nullptr, &cur_pos);
                    SDL_RenderPresent(renderer);
                    insertNumber(i, j);
                    if (grid[i][j] == grid_copy[i][j])
                        boolean[i][j] = FOUND;
                }
                break;
            }
            case SDLK_s:
            {
                // Show solution when debugging
                showGrid();
                SDL_Delay(1000);
                break;
            }
            case SDLK_a:
            {
                // Confirming
                if (endGame())
                {
                    SDL_RenderCopy(renderer, good, nullptr, nullptr);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(2000);
                    finished = true;
                    break;
                }
                else if (!endGame())
                {
                    SDL_RenderCopy(renderer, error, nullptr, nullptr);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(500);
                    break;
                }
            }
            }
        }
        showGridCopy();
        SDL_RenderCopy(renderer, cursor[0], nullptr, &cur_pos);
        SDL_RenderPresent(renderer);
        }
    }
}

bool Sudoku::endGame()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (boolean[i][j] == NOT_FOUND)
                return false;
    return true;
}

void Sudoku::insertNumber(int i, int j)
{
    SDL_Event event;
    bool fin = false;
    while (!fin)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_1:
                case SDLK_KP_1:
                {
                    grid_copy[i][j] = 1;
                    if (grid[i][j]==1)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_2:
                case SDLK_KP_2:
                {
                    grid_copy[i][j] = 2;
                    if (grid[i][j]==2)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_3:
                case SDLK_KP_3:
                {
                    grid_copy[i][j] = 3;
                    if (grid[i][j]==3)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_4:
                case SDLK_KP_4:
                {
                    grid_copy[i][j] = 4;
                    if (grid[i][j]==4)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_5:
                case SDLK_KP_5:
                {
                    grid_copy[i][j] = 5;
                    if (grid[i][j]==5)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_6:
                case SDLK_KP_6:
                {
                    grid_copy[i][j] = 6;
                    if (grid[i][j]==6)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_7:
                case SDLK_KP_7:
                {
                    grid_copy[i][j] = 7;
                    if (grid[i][j]==7)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_8:
                case SDLK_KP_8:
                {
                    grid_copy[i][j] = 8;
                    if (grid[i][j]==8)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
                case SDLK_9:
                case SDLK_KP_9:
                {
                    grid_copy[i][j] = 9;
                    if (grid[i][j]==9)
                        boolean[i][j]=FOUND;
                    fin = true;
                    break;
                }
            }
        }
        }
    }
    showGridCopy();
    SDL_RenderPresent(renderer);
}

void Sudoku::reload()
{
    SDL_Event event;
    bool fin = false;
    while (!fin)
    {
        SDL_RenderCopy(renderer, ending, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(30);
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_SPACE:
                return;
            case SDLK_ESCAPE:
            {
                redo = false;
                return;
            }
            default:
            {
                break;
            }
            }
        }
    }
}

void Sudoku::showHelper()
{
	bool pass = false;
	while(!pass)
	{
        SDL_RenderCopy(renderer, help, nullptr, nullptr);
        SDL_RenderPresent(renderer);
	    SDL_Event event;
	    SDL_WaitEvent(&event);
	    if (event.type == SDL_KEYDOWN)
		    pass = true;
	}

}
