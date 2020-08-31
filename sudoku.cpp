#include <SDL/SDL.h>
#include "sudoku.h"

Sudoku::Sudoku()
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode(COTE * 16 + 9, COTE * 16 + 9, 32, SDL_HWSURFACE | SDL_RESIZABLE);

    cur_pos.x = 2;
    cur_pos.y = 2;

    cur_choice = START;
    choice_pos.x = 10; choice_pos.y = 134;

    start = IMG_Load("./images/start.png");

    number = new SDL_Surface*[10];
    number[1] = IMG_Load("./images/1.png");
    number[2] = IMG_Load("./images/2.png");
    number[3] = IMG_Load("./images/3.png");
    number[4] = IMG_Load("./images/4.png");
    number[5] = IMG_Load("./images/5.png");
    number[6] = IMG_Load("./images/6.png");
    number[7] = IMG_Load("./images/7.png");
    number[8] = IMG_Load("./images/8.png");
    number[9] = IMG_Load("./images/9.png");
    number[0] = IMG_Load("./images/0.png");

   fixed_number = new SDL_Surface*[9];
    fixed_number[1] = IMG_Load("./images/1_fixe.png");
    fixed_number[2] = IMG_Load("./images/2_fixe.png");
    fixed_number[3] = IMG_Load("./images/3_fixe.png");
    fixed_number[4] = IMG_Load("./images/4_fixe.png");
    fixed_number[5] = IMG_Load("./images/5_fixe.png");
    fixed_number[6] = IMG_Load("./images/6_fixe.png");
    fixed_number[7] = IMG_Load("./images/7_fixe.png");
    fixed_number[8] = IMG_Load("./images/8_fixe.png");
    fixed_number[9] = IMG_Load("./images/9_fixe.png");
    fixed_number[0] = IMG_Load("./images/0_fixe.png");

    loading_screen = new SDL_Surface*[5];
    loading_screen[0] = IMG_Load("./images/loading.png");
    loading_screen[1] = IMG_Load("./images/loading1.png");
    loading_screen[2] = IMG_Load("./images/loading2.png");
    loading_screen[3] = IMG_Load("./images/loading3.png");
    loading_screen[4] = IMG_Load("./images/loading4.png");

    cursor = new SDL_Surface*[2];
    cursor[0] = IMG_Load("./images/cursor.png");
    cursor[1] = IMG_Load("./images/cursor_selected.png");

    choice = IMG_Load("./images/choice.png");
    help = IMG_Load("./images/help.png");
    ending = IMG_Load("./images/end.png");
    dialog = IMG_Load("./images/dialog.png");
    error = IMG_Load("./images/error.png");
    good = IMG_Load("./images/good.png");

    redo = true;
}

Sudoku::~Sudoku()
{
    SDL_FreeSurface(screen);
    SDL_FreeSurface(start);
    grid.clear();
    tmp.clear();
    grid_copy.clear();
    boolean.clear();
    delete(loading_screen);
    delete(cursor);
    delete(number);
    delete(fixed_number);
    redo = false;
}

void Sudoku::start_game()
{
    SDL_Event event;
    bool begin = false;
    for (int i = 0; i < 9; i++)
    {
        grid.push_back(vector < int >(9, 0));
        grid_copy.push_back(vector < int >(9, 0));
        boolean.push_back(vector < int >(9,NOT_FOUND));
    }
    while (!begin)
    {
    	if (cur_choice == START)
		{
			choice_pos.x = 10; choice_pos.y = 100;
		}

		else if (cur_choice == HELP)
		{
			choice_pos.x = 10; choice_pos.y = 120;
		}
		       SDL_BlitSurface(start, NULL, screen, NULL);
        SDL_BlitSurface(choice, NULL,screen,&choice_pos);
        SDL_Flip(screen);
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
                        show_help();
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
        start_game();
        generate_grid();
        grid_copy_grid();
        grid_cleanup();
        set_boolean();
        show_grid_copy();
        move_cursor();
        SDL_Delay(1000);
        reload();
    }
    this->~Sudoku();
}

void Sudoku::generate_grid()
{
    while (check_redo())
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                grid[i][j] = 0;
            }
        }
        tmp.clear();
        generate_diag(0, 3, 0, 3);
        generate_diag(3, 6, 3, 6);
        generate_diag(6, 9, 6, 9);
        generate_not_diag(0, 3, 3, 6);
        generate_not_diag(0, 3, 6, 9);
        generate_not_diag(3, 6, 6, 9);
        generate_not_diag(3, 6, 0, 3);
        generate_not_diag(6, 9, 0, 3);
        generate_not_diag(6, 9, 3, 6);
        loading();
    }
    fprintf(stderr,"out\n");
}

bool Sudoku::check_redo()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j] == 0)
            {
                fprintf(stderr,"redo\n");
                return true;

            }
        }
    }
    return false;
}

void Sudoku::generate_diag(int i_min, int i_max, int j_min, int j_max)
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
            while (check(n) || check_3x3(n, i, j));
            grid[i][j] = n;
            tmp.push_back(n);
        }
    }
    tmp.clear();
}

bool Sudoku::check(int n)
{
    for (int i = 0; i < tmp.size(); i++)
    {
        if (tmp[i] == n)
            return true;
    }
    return false;
}

bool Sudoku::check_3x3(int n, int i, int j)
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

void Sudoku::generate_not_diag(int i_min, int i_max, int j_min, int j_max)
{
    init_tmp();
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
            while (check(n) || check_3x3(n, i, j)
                    || check_line(n, i) || check_row(n, j));
            grid[i][j] = n;
            tmp.push_back(n);
        }
        counter = 0;
    }
}

bool Sudoku::check_line(int n,int i)
{
    for (int j = 0; j < 9; j++)
        if (grid[i][j] == n)
            return true;
    return false;
}

bool Sudoku::check_row(int n, int j)
{
    for (int i = 0; i < 9; i++)
        if (grid[i][j] == n)
            return true;
    return false;
}

void Sudoku::loading()
{
    SDL_BlitSurface(loading_screen[0], NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(30);
    SDL_BlitSurface(loading_screen[1], NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(30);
    SDL_BlitSurface(loading_screen[2], NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(30);
    SDL_BlitSurface(loading_screen[3], NULL, screen, NULL);
    SDL_Delay(30);
    SDL_Flip(screen);
    SDL_BlitSurface(loading_screen[4], NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(30);
}

void Sudoku::grid_copy_grid()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            grid_copy[i][j] = grid[i][j];
}

void Sudoku::grid_cleanup()
{
   for (int i = 0; i < 9; i += 3)
        for (int j = 0; j < 9; j += 3)
            grid_3x3_cleanup(i, i + 2, j, j + 2);
}

void Sudoku::grid_3x3_cleanup(int i_min, int i_max, int j_min, int j_max)
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

void Sudoku::set_boolean()
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

void Sudoku::show_grid_copy()
{
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 255, 0));
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            SDL_Rect pos;
            if (j < 3)
                pos.x = j * 16 + 2;
            else
            {
                if (j >= 6)
                    pos.x = j * 16 + 6;
                else
                    pos.x = j * 16 + 4;
            }
            if (i < 3)
                pos.y = i * 16 + 2;
            else
            {
                if (i >= 6)
                    pos.y = i * 16 + 6;
                else
                    pos.y = i * 16 + 4;
            }
            if (grid_copy[i][j] == 1)
            {
                if (boolean[i][j] == FIXED)
                    SDL_BlitSurface(fixed_number[1], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[1], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 2)
            {
                if (boolean[i][j] == FIXED)
                    SDL_BlitSurface(fixed_number[2], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[2], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 3)
            {
                if (boolean[i][j] == FIXED)
                    SDL_BlitSurface(fixed_number[3], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[3], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 4)
            {
                if (boolean[i][j]  == FIXED)
                    SDL_BlitSurface(fixed_number[4], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[4], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 5)
            {
                if (boolean[i][j]  == FIXED)
                    SDL_BlitSurface(fixed_number[5], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[5], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 6)
            {
                if (boolean[i][j]  == FIXED)
                    SDL_BlitSurface(fixed_number[6], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[6], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 7)
            {
                if (boolean[i][j]  == FIXED)
                    SDL_BlitSurface(fixed_number[7], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[7], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 8)
            {
                if (boolean[i][j]  == FIXED)
                    SDL_BlitSurface(fixed_number[8], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)

                    SDL_BlitSurface(number[8], NULL, screen, &pos);
            }
            if (grid_copy[i][j] == 0)
            {
                SDL_BlitSurface(number[0], NULL, screen, &pos);
            }
            else if (grid_copy[i][j] == 9)
            {
                if (boolean[i][j]  == FIXED)
                    SDL_BlitSurface(fixed_number[9], NULL, screen, &pos);
                if (boolean[i][j] != FIXED)
                    SDL_BlitSurface(number[9], NULL, screen, &pos);
            }
        }
    }
    SDL_Flip(screen);
}

void Sudoku::init_tmp()
{
    tmp.clear();
    for (int i = 1; i <= 9; i++)
        tmp.push_back(i);
}

void Sudoku::show_grid()
{
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 255, 0));
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            SDL_Rect pos;
            if (j < 3)
                pos.x = j * 16 + 2;
            else
            {
                if (j >= 6)
                    pos.x = j * 16 + 6;
                else
                    pos.x = j * 16 + 4;
            }
            if (i < 3)
                pos.y = i * 16 + 2;
            else
            {
                if (i >= 6)
                    pos.y = i * 16 + 6;
                else
                    pos.y = i * 16 + 4;
            }
            if (grid[i][j] == 1)
                SDL_BlitSurface(number[1], NULL, screen, &pos);
            else if (grid[i][j] == 2)
                SDL_BlitSurface(number[2], NULL, screen, &pos);
            else if (grid[i][j] == 3)
                SDL_BlitSurface(number[3], NULL, screen, &pos);
            else if (grid[i][j] == 4)
                SDL_BlitSurface(number[4], NULL, screen, &pos);
            else if (grid[i][j] == 5)
                SDL_BlitSurface(number[5], NULL, screen, &pos);
            else if (grid[i][j] == 6)
                SDL_BlitSurface(number[6], NULL, screen, &pos);
            else if (grid[i][j] == 7)
                SDL_BlitSurface(number[7], NULL, screen, &pos);
            else if (grid[i][j] == 8)
                SDL_BlitSurface(number[8], NULL, screen, &pos);
            else if (grid[i][j] == 0)
                SDL_BlitSurface(number[0], NULL, screen, &pos);
            else
                SDL_BlitSurface(number[9], NULL, screen, &pos);
        }
    }
    SDL_Flip(screen);
}

void Sudoku::move_cursor()
{
    int i = cur_pos.y / 16, j = cur_pos.x / 16;
    SDL_BlitSurface(cursor[0], NULL, screen, &cur_pos);
    SDL_Flip(screen);
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
            case SDLK_UP:
            {
                if (cur_pos.y - 16 > 0)
                {
                    if (cur_pos.y == 52 || cur_pos.y == 102)
                        cur_pos.y -= 18;
                    else
                        cur_pos.y -= 16;
                    i--;
                }
                break;
            }
            case SDLK_DOWN:
            {
                if (cur_pos.y + 16 <= COTE * 16)
                {
                    if (cur_pos.y == 34 || cur_pos.y == 84)
                        cur_pos.y += 18;
                    else
                        cur_pos.y += 16;
                    i++;
                }
                break;
            }
            case SDLK_LEFT:
            {
                if (cur_pos.x - 16 > 0)
                {
                    if (cur_pos.x == 52 || cur_pos.x == 102)
                        cur_pos.x -= 18;
                    else
                        cur_pos.x -= 16;
                    j--;
                }
                break;
            }
            case SDLK_RIGHT:
            {
                if (cur_pos.x + 16 <= COTE * 16)
                {
                    if (cur_pos.x == 34 || cur_pos.x == 84)
                        cur_pos.x += 18;
                    else
                        cur_pos.x += 16;
                    j++;
                }
                break;
            }
            case SDLK_SPACE:
            {
                if (boolean[i][j] != FIXED)
                {
                    SDL_BlitSurface(cursor[1], NULL, screen, &cur_pos);
                    SDL_Flip(screen);
                    insert_nb(i, j);
                    if (grid[i][j] == grid_copy[i][j])
                        boolean[i][j] = FOUND;
                }
                break;
            }
            case SDLK_s:
            {
                show_grid();
                SDL_Delay(1000);
                break;
            }
            case SDLK_a:
            {
                bool pass = false;
                SDL_Event new_event;
                while(!pass)
                {
                    SDL_Rect pos;
                    pos.x = 14;
                    pos.y = 40;
                    SDL_BlitSurface(dialog,NULL,screen,&pos);
                    SDL_Flip(screen);
                    SDL_WaitEvent(&new_event);
                    switch (new_event.type)
                    {
                    case SDL_KEYDOWN:
                    {
                        switch (new_event.key.keysym.sym)
                        {
                        case SDLK_y:
                            if (end_game())
                            {
                                SDL_BlitSurface(good,NULL,screen,&pos);
                                SDL_Flip(screen);
                                SDL_Delay(2000);
                                pass = true;
                                finished = true;
                                break;
                            }
                            if (!end_game())
                            {
                                SDL_BlitSurface(error,NULL,screen,&pos);
                                SDL_Flip(screen);
                                SDL_Delay(500);
                                pass = true;
                                break;
                            }
                        case SDLK_n:
                            pass = true;
                            break;
                        }
                    }
                    }
                }
            }
            }
        }
        show_grid_copy();
        SDL_BlitSurface(cursor[0], NULL, screen, &cur_pos);
        SDL_Flip(screen);
        }
    }
}

bool Sudoku::end_game()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (boolean[i][j] == NOT_FOUND)
                return false;
    return true;
}

void Sudoku::insert_nb(int i, int j)
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
            {
                grid_copy[i][j] = 1;
                if (grid[i][j]==1)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_2:
            {
                grid_copy[i][j] = 2;
                if (grid[i][j]==2)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_3:
            {
                grid_copy[i][j] = 3;
                if (grid[i][j]==3)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_4:
            {
                grid_copy[i][j] = 4;
                if (grid[i][j]==4)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_5:
            {
                grid_copy[i][j] = 5;
                if (grid[i][j]==5)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_6:
            {
                grid_copy[i][j] = 6;
                if (grid[i][j]==6)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_7:
            {
                grid_copy[i][j] = 7;
                if (grid[i][j]==7)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_8:
            {
                grid_copy[i][j] = 8;
                if (grid[i][j]==8)
                    boolean[i][j]=FOUND;
                fin = true;
                break;
            }
            case SDLK_9:
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
    show_grid_copy();
    SDL_Flip(screen);
}

void Sudoku::reload()
{
    SDL_Event event;
    bool fin = false;
    while (!fin)
    {
        SDL_BlitSurface(ending, NULL, screen, NULL);
        SDL_Flip(screen);
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

void Sudoku::show_help()
{
	bool pass = false;
	while(!pass)
	{
		SDL_BlitSurface(help,NULL,screen,NULL);
	SDL_Flip(screen);
	SDL_Event event;
	SDL_WaitEvent(&event);
	if (event.type == SDL_KEYDOWN)
		pass = true;
	}

}
