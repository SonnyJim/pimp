#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
SDL_Texture *tex = NULL;

//Watch face is 6 x 12 pixels
//Pixel is 5x5 + 1 block spacing
#define pxsize 5
#define scale 3
#define spacing 1

int width = (6 * pxsize * scale) + (7 * spacing * scale);
int height = (12 * pxsize * scale) + (13 * spacing * scale);

SDL_Event event;
SDL_Rect block;

enum modes
{
    MODE_TIME,
    MODE_DATE
};

int hours, minutes, seconds, month, day;
int mode = MODE_TIME;
int offsety, offsetx = 0;
int print_seconds = 0;

SDL_TimerID timer;


Uint32 set_mode_time (Uint32 interval, void *param)
{
    mode = MODE_TIME;
}

void toggle_mode (void)
{
    if (mode == MODE_TIME)
    {
        SDL_RemoveTimer (timer);
        mode = MODE_DATE;
        timer = SDL_AddTimer(4000, set_mode_time, NULL);
    }

    else
        mode = MODE_TIME;
}

void toggle_seconds (void)
{
    if (print_seconds == 0)
        print_seconds = 1;
    else
        print_seconds = 0;
}

int poll_event (void)
{
    SDL_PollEvent (&event);
    if (event.type == SDL_QUIT)
        return 1;

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_SPACE:
                toggle_mode ();
                break;
            case SDLK_s:
                toggle_seconds ();
                break;
        }
    }

    return 0;
}

void get_time (void)
{
    time_t now;
    struct tm *now_tm;

    now = time(NULL);
    now_tm = localtime(&now);
    hours = now_tm->tm_hour;
    minutes = now_tm->tm_min;
    seconds = now_tm->tm_sec;
    day = now_tm->tm_mday;
    month = now_tm->tm_mon;
    month += 1;

    if (hours > 12)
        hours -= 12;
}

void draw_hours (int hour)
{
    int i;
    //Set up initial block position for hours
    block.w = pxsize * scale;
    block.h = pxsize * scale;
    block.x = spacing * scale;
    block.y = height - (pxsize * scale);
   
    block.x += offsetx;
    block.y += offsety;

    //Draw hours
    for (i = 1; i <= hour; i++)
    {
        block.y -= spacing * scale;
        SDL_RenderFillRect( ren, &block );
        block.y -= block.h;
    }
}

void draw_minutes (int mins)
{
    int i, j, rows, remainder;

    if (mins > 60)
        mins = 60;
    else if (mins <= 0)
        return;

    block.w = pxsize * scale;
    block.h = pxsize * scale;
    
    //Draw minutes
    rows = mins / 5;
    remainder = mins % 5;
    
    block.x = (spacing * scale) + block.w;
    block.y = height - block.h;
    block.y -= spacing * scale;
    
    block.x += offsetx;
    block.y += offsety;
    //Draw the complete rows
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < 5; j++)
        {
            block.x += spacing * scale;
            SDL_RenderFillRect( ren, &block );
            block.x += block.w;
        }
        block.x = (spacing * scale) + block.w;
        block.x += offsetx;
        block.y -= spacing * scale;
        block.y -= block.h;
    }

    //Draw the remainder
    block.x = (spacing * scale) + block.w;
    block.x += offsetx;
    for (i = 0; i < remainder; i++)
    {
        block.x += spacing * scale;
        SDL_RenderFillRect( ren, &block );
        block.x += block.w;
    }
}

void draw_seconds (int secs)
{
    int i, j, rows, remainder, w, h;

    if (secs > 60)
        secs = 60;
    else if (secs <= 0)
        return;

    w = block.w;
    h = block.h;
    block.w = (pxsize * scale) / 2;
    block.h = (pxsize * scale) / 2;
    
    rows = secs / 5;
    remainder = secs % 5;
   
    block.x = (spacing * scale) + w;
    block.y = height - h;
    block.y -= spacing * scale;
    
    block.x += offsetx;
    block.y += offsety;
    //Draw the complete rows
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < 5; j++)
        {
            block.x += spacing * scale;
            SDL_RenderFillRect( ren, &block );
            block.x += w;
        }
        block.x = (spacing * scale) + w;
        block.x += offsetx;
        block.y -= spacing * scale;
        block.y -= h;
    }

    //Draw the remainder
    block.x = (spacing * scale) + w;
    block.x += offsetx;
    for (i = 0; i < remainder; i++)
    {
        block.x += spacing * scale;
        SDL_RenderFillRect( ren, &block );
        block.x += w;
    }
}

void draw_unlit (void)
{
    int i, j;
    //Set background colour to black
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    SDL_RenderClear (ren);

    //Draw unlit blocks
    SDL_SetRenderDrawColor(ren, 0, 0, 50, 0);
    draw_hours (12);
    draw_minutes (60);
}

void draw_date (void)
{
    draw_unlit ();
    SDL_SetRenderDrawColor(ren, 0, 0, 255, 0);
    draw_hours (month);
    draw_minutes (day);

}

void draw_time (void)
{
    draw_unlit ();
    SDL_SetRenderDrawColor(ren, 0, 0, 255, 0);
    draw_hours (hours);
    draw_minutes (minutes);
    
    if (print_seconds)
    {
        SDL_SetRenderDrawColor(ren, 0, 0, 100, 0);
        draw_seconds (seconds);
    }
}

void print_help (void)
{
    fprintf (stdout, "Pimp time\n");
    fprintf (stdout, "-h        print this help\n");
    fprintf (stdout, "-c        centre on screen\n");
    fprintf (stdout, "-s        show seconds\n");
}

int main( int argc, char* argv[] )
{
    SDL_DisplayMode current;
    int running = 0;
    int c;
    int centre_on_screen = 0;

    while ((c = getopt (argc, argv, "hcs")) != -1)
    {
        switch (c)
        {
            case 'h':
            default:
                print_help ();
                return;
                break;
            case 'c':
                centre_on_screen = 1;
                break;
            case 's':
                print_seconds = 1;
                break;
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf (stdout, "Error initialising SDL video: %s\n", SDL_GetError());
        return 1;
    }
    if (SDL_GetCurrentDisplayMode(0, &current) != 0)
    {
        fprintf (stderr, "Error getting current display mode: %s\n", SDL_GetError());
        return 1;
    }
    
    win = SDL_CreateWindow("Pimp time", (current.w / 2) - (width / 2), (current.h / 2) - (height / 2),
                           width, height, SDL_WINDOW_SHOWN);
    
    if (centre_on_screen)
    {
        offsetx = (current.w / 2) - (width / 2);
        offsety = (current.h / 2) - (height / 2);
    }
    else
    {
        offsetx = 0;
        offsety = 0;
    }

    if (win == NULL)
    {
        fprintf (stderr, "Error creating window: %s\n", SDL_GetError());
        return 1;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL)
    {
        fprintf (stderr, "Error creating renderer: %s\n", SDL_GetError());
        return 1;
    }

    running = 1;
    while (running)
    {
        if (poll_event())
            running = 0;
        get_time ();

        if (mode == MODE_TIME)
            draw_time ();
        else
            draw_date ();
        SDL_RenderPresent (ren);
    }

    //Clean up
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
