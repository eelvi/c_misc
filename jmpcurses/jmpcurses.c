#include <stdio.h>
#include <ncurses.h>
#include <sys/time.h>

typedef struct{

    char exit;
    WINDOW *win;

    float pos_x;
    float pos_y;

    float vec_x;
    float vec_y;

    long long prev_t;
    float dt;

    int term_max_x;
    int term_max_y;

    char bounce;

    short clk;

    } pstate;

long long time_stamp(){ //ms
    struct timeval te;
    gettimeofday(&te, NULL);
    return te.tv_usec/1000;
}

void on_resize( pstate *state){
    getmaxyx( state->win, state->term_max_y, state->term_max_x );
}

void init_tick(pstate *state){
    
    on_resize(state);
    state->prev_t=time_stamp();
    state->pos_y = 20.0f;
    wtimeout( state->win, 1 ); //ms
}


void handle_input( pstate *state, int input ){
    float mov_speed = 5.0f;
    float anti_grav = 1.4f;

    switch (input){
        case KEY_RESIZE:
            on_resize(state);
            break;
        case KEY_UP:
            if(state->vec_y < 0.1)
                state->vec_y = 0.1;
            state->vec_y += (state->dt * mov_speed * anti_grav); //state->dt * mov_speed;
            break;
        case KEY_DOWN:
            state->vec_y -= state->dt * mov_speed;
            break;
        case KEY_LEFT:
            state->vec_x -= state->dt * mov_speed;

            break;
        case KEY_RIGHT:
            state->vec_x += state->dt * mov_speed;
            break;
        case 'x':
        case 'q':
            state->exit=1;
        default:
            break;

    }
    
}

void do_bounce (pstate *state){
    float coeff = 0.7;
    state->vec_y = -(state->vec_y * coeff);
    state->pos_y = 0.01;
}

void gravity( pstate *state ){
    float gravity = 0.250f; //4 steps per second
    if (state->pos_y > 0.005f){
        state->vec_y -= ( state->dt * gravity );
    }
    else{
        if (state->vec_y < 0.0){
            if (state->bounce)
                do_bounce(state);
            else
                state->vec_y=0.01;
        }
        if (state->pos_y < 0)
            state->pos_y = 0.0f;
    }

}

void friction(pstate *state){
    float coefficent = 0.50;
    if (state->vec_x < 0.2)
        coefficent *= 2;
    if (state->pos_y > 2.0 )
        coefficent = 0.0;
    if (state->vec_x > 0.01 || state->vec_x < 0.01){

            state->vec_x -= state->vec_x*coefficent*state->dt;
    }
}

void motion(pstate *state){
    state->pos_x += state->vec_x;
    state->pos_y += state->vec_y;

    friction(state);
}

void translate(pstate *state, int *y, int *x){
    int x_base = state->term_max_x / 2;
    x_base = x_base + ( (int) state->pos_x );
    while ( x_base > state->term_max_x-1 || x_base < 0 ){
        if (x_base < 0)
            x_base += state->term_max_x;
        else
            x_base -= state->term_max_x;
    }
    *x = x_base;
    *y = state->term_max_y - ( (int) state->pos_y) -1;

}

void draw(pstate *state){
    int pos_x, pos_y;
    translate(state, &pos_y, &pos_x);

    char p[2] = {'^','#'};

    unsigned lclk = (unsigned)(state->clk++)%100;
    if (lclk > 50)
        lclk=0;
    else
        lclk=1;
    
    wclear(state->win);
    mvwaddch( state->win, pos_y, pos_x,  p[ lclk ] );
    wrefresh(state->win);

}

void tick(pstate* state){
    long long tstmp = time_stamp();
    if (state->prev_t < tstmp) //wrap around thing
        state->dt = (float) (tstmp - state->prev_t)/1000.0; //10 ms will be 0.010
    state->prev_t = tstmp;
    
    gravity( state );
    motion( state );

}


int main(void)
{
    initscr(); //required ncurses thing

    
    keypad(stdscr, TRUE); //stdscr is the default newbie's window we're in i guess
    cbreak();
    noecho(); //don't repeat what the user typed

    attron(A_BOLD); //attritbute bold
    echo();
    
    pstate current_state={0};
    current_state.win = stdscr;
    current_state.bounce = 1; //toggle bouncing

    pstate *state = &current_state;


    init_tick(state);

    while (! state->exit){
        handle_input( state, getch() );
        tick(state);
        draw(state);
    }

    endwin();

    return 0;

}
