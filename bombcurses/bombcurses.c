#include <stdio.h>
#include <ncurses.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define KEY_SPACE ' '
#define KEY_CR '\n'

//color support
#define WITH_COLOR 1

struct _bomb;

typedef struct _bomb {
    float x;
    float y;

    float f; //strength of the explosion

    struct _bomb *next;
} bomb;


typedef struct {

    float vx;
    float vy;

    float pos_x;
    float pos_y;

} particle;

typedef struct {

    char exit;
    WINDOW *win;

    particle *particles;
    char *occoupied;
    int occoupied_size;

    int n_particles;

    float cursor_pos_x;
    float cursor_pos_y;

    bomb *root_bomb;

    long long prev_t;
    float dt;

    int restarts;

    int term_max_x;
    int term_max_y;

    char detonated;
    char paused;

} pstate;

long long time_stamp()
{
    //ms
    struct timeval te;
    gettimeofday(&te, NULL);
    return te.tv_usec / 1000;
}

int xy_to_occoupied_idx(pstate *state, float x, float y)
{

    int xp = (int)x;
    int yp = (int)y;

    if ( xp > state->term_max_x - 1 || yp > state->term_max_y - 1 ) {
        return -1;
    }

    int idx;

    idx = (state->term_max_y - 1 - yp ) * state->term_max_x;
    idx += xp;

    return idx;
}

//only gets executed if a particle happend to collide with our particle
// still not perfect
particle *occoupied_idx_to_particle( pstate *state, int idx, particle *execlude )
{

    particle *particles = state->particles;
    for ( int i = 0; i < state->n_particles; i++ ) {

        if ( xy_to_occoupied_idx( state, particles[i].pos_x, particles[i].pos_y ) == idx &&
             particles + i != execlude ) {
            return particles + i;
        }

    }

    return NULL;

}

void on_resize( pstate *state)
{
    getmaxyx( state->win, state->term_max_y, state->term_max_x );

    state->occoupied_size = state->term_max_y * state->term_max_x;
    if (!state->occoupied) {
        state->occoupied = malloc ( state->occoupied_size * sizeof(char) );
    }
    else
        state->occoupied = realloc( state->occoupied, state->occoupied_size );
}

void free_resources( pstate *state )
{

    free(state->occoupied);
    state->occoupied = NULL;
    free(state->particles);
    state->particles = NULL;
    state->n_particles = 0;
    state->detonated = 0;

    if( state-> root_bomb ) {
        bomb *curr_bomb = state-> root_bomb;
        bomb *next;
        while (curr_bomb) {
            next = curr_bomb->next;
            free(curr_bomb);
            curr_bomb = next;
        }

        state->root_bomb = NULL;
    }
}

int create_box( pstate *state, int width, int height, int x_offset )
{

    int n_particles = width * height;
    particle *particles = malloc( n_particles * sizeof(particle) );
    if(!particles)
        return 0;

    state->n_particles = n_particles;
    state->particles = particles;

    if (!particles)
        exit(1);

    float xpos = x_offset;
    float ypos = -0.5;
    for (int i = 0; i < n_particles; i++) {
        if (i % width == 0) {
            xpos = x_offset+0.5;
            ypos += 1.0;
        }
        xpos += 1.0;

        particles[i].pos_y = ypos;
        particles[i].pos_x = xpos;
        particles[i].vy = particles[i].vx = 0.0;

    }

    return 1;

}

void init_tick(pstate *state)
{

    on_resize(state);
    state->prev_t = time_stamp();
    wtimeout( state->win, 1 ); //ms

    state->detonated = 0;
    state->paused = 1;
    state->cursor_pos_y = state->term_max_y / 2;
    state->cursor_pos_x = state->term_max_x / 2;

    
    int width = 4 + state->restarts*4;
    int height = 3 + state->restarts*2;

    if ( !create_box( state, width, height, ( state->term_max_x +width )/2 ) ){
        exit(1);
    }
}

void toggle_pause(pstate *state)
{
    state->paused = !state->paused;
}

void restart(pstate *state)
{
    free_resources(state);
    state->restarts++;
    init_tick(state);

}

void detonate( pstate *state, bomb *the_bomb )
{
    float factor = the_bomb->f;
    float sx,sy;
    float dx,dy;
    
    particle *particles = state->particles;
    if (!particles)
        return;

    for (int i = 0; i < state->n_particles; i++) {
        
        dx = (particles[i].pos_x - the_bomb->x) ;
        dy = (particles[i].pos_y - the_bomb->y) ;
        sx = (dx>0.0)?1.0:-1.0;
        sy = (dy>0.0)?1.0:-1.0;
        dx *= sx; //abs value
        dy *= sy;

        particles[i].vx += sx*factor / sqrt(dx);
        particles[i].vy += sy*factor / sqrt(dy);

    }

}

void detonate_all( pstate *state )
{

    if( state-> root_bomb && state->particles ) {
        bomb *curr_bomb = state-> root_bomb;
        bomb *next;
        while (curr_bomb) {
            next = curr_bomb->next;
            detonate(state, curr_bomb);

            curr_bomb = next;
        }

        state->root_bomb = NULL;
        state->detonated = 1;
        state->paused = 0;
    }

}

void place_bomb( pstate *state )
{

    bomb *new_bomb = malloc( sizeof(bomb) );
    if (!new_bomb)
        return;
    new_bomb->x = state->cursor_pos_x;
    new_bomb->y = state->term_max_y - state->cursor_pos_y -1;

    new_bomb->f = 3.91;
    new_bomb->next = NULL;

    if ( !state->root_bomb ){
        state->root_bomb = new_bomb;
    }
    else {

        bomb *last = state->root_bomb;
        while ( last->next ){
            last = last->next;
        }

        last->next = new_bomb;
        
    }

}

void do_delay()
{
    usleep(50 * 1000); // ms
}

void handle_input( pstate *state, int input )
{

    switch (input) {
    case KEY_RESIZE:
        on_resize(state);
        break;
    case KEY_UP:
        if ( !state->detonated ) {
            state->cursor_pos_y--; //left to right, up to down coordinates
            do_delay();
        }
        break;
    case KEY_DOWN:
        if ( !state->detonated ) {
            state->cursor_pos_y++; //left to right, up to down coordinates
            do_delay();
        }
        break;
    case KEY_LEFT:
        if ( !state->detonated ) {
            state->cursor_pos_x--; //left to right, up to down coordinates
            do_delay();
        }
        break;
    case KEY_RIGHT:
        if ( !state->detonated ) {
            state->cursor_pos_x++; //left to right, up to down coordinates
            do_delay();
        }
        break;
    case KEY_SPACE:
        if( !state->detonated )
            place_bomb(state);
        else
            toggle_pause(state);
        break;
    case KEY_ENTER:
    case KEY_CR:
        if( !state->detonated ) {
            detonate_all(state);
        }
        else
            restart(state);
        break;
    case 'x':
    case 'q':
        state->exit = 1;
        break;
    default:
        break;

    }

}

enum relative_directions{
    R_down,
    R_up,
    R_left,
    R_right
};


void gravity( pstate *state )
{

    float gravity = 1.850f; 

    particle *particles = state->particles;
    if (!particles)
        return;

    for (int i = 0; i < state->n_particles; i++) {

        if ( particles[i].pos_y > 0.005f) {
            particles[i].vy -= ( state->dt * gravity );
        }
        else {
            if (state->particles[i].pos_y < 0.0) {
                    state->particles[i].vy = 0.01;
            }
            if (particles[i].pos_y < 0){
                particles[i].pos_y = 0.0f;
            }
        }

    }

}

void friction(pstate *state, particle *par)
{
    float coefficent = 0.90;
    if (par->pos_y > 2.0 )
        coefficent = 0.0;
    if (par->vx > -0.04 || par->vx < 0.04) {
        coefficent  *= 2;
    }
    par->vx -= par->vx * coefficent * state->dt;
}

void motion(pstate *state)
{

    particle *particles = state->particles;
    if (!particles)
        return;

    memset( state->occoupied, 0, state->occoupied_size);

    int x, y, idx;
    for (int i = 0; i < state->n_particles; i++) {
        particles[i].pos_x += particles[i].vx * state->dt;
        particles[i].pos_y += particles[i].vy * state->dt;

        x = (int) particles[i].pos_x;
        y = (int) particles[i].pos_y;

        idx = xy_to_occoupied_idx( state, x, y);

        if (idx >= 0) { //returns negative when outside screen
            if ( state->occoupied[idx] ) { //collision
                particle *other = occoupied_idx_to_particle( state, idx, particles + i );

                particles[i].pos_x -= particles[i].vx * state->dt;
                particles[i].pos_y -= particles[i].vy * state->dt;

                if (other) {
                    float x_sum, y_sum;
                    x_sum = particles[i].vx + other->vx;
                    y_sum = particles[i].vy + other->vy;

                    particles[i].vx = other->vx = x_sum / 2.0;
                    particles[i].vy = other->vy = y_sum / 2.0;
                }


                x = (int) particles[i].pos_x;
                y = (int) particles[i].pos_y;

                idx = xy_to_occoupied_idx( state, x, y );//questionable

            }

            state->occoupied[idx] = '#';

            friction(state, particles + i);
        }

    }
}

void draw_particles(pstate *state)
{

    int x = 0;
    int y = 0;

    int x_max = state->term_max_x;

    #if WITH_COLOR
        attron(COLOR_PAIR(1));
    #endif

    for (int i = 0; i < state->occoupied_size; i++) {

        if (state->occoupied[i]) {
            mvwaddch( state->win, y, x, '#') ;
        }

        if (++x == x_max) {
        y++;
            //y--;
            x = 0;
        }

    }
    #if WITH_COLOR
        attron(COLOR_PAIR(1));
    #endif


}

enum quadrant{
    TOP_R,
    BOTTOM_R,

};

void print_str(pstate *state, const char *str, int quadrant, int ln_offset ){
    int len = strlen(str) + 2;
    int y_off=0; 

    if (quadrant==BOTTOM_R)
        y_off = state->term_max_y - ln_offset;
    else if (quadrant==TOP_R)
        y_off = ln_offset;


    mvwaddstr( state->win, y_off, state->term_max_x - len, str );
}

void draw_overlay(pstate *state)
{
    char buff[40] = {0};
    sprintf(buff, (state->paused)?"paused":"" );
    print_str( state, buff, TOP_R, 1 );
    sprintf(buff, (state->detonated)?"detonated":"nope");
    print_str( state, buff, TOP_R, 2 );
    sprintf(buff, "<space>: %s", (state->detonated)? ((!state->paused)?"pause":"continue")
                                                            :"place bomb");
    print_str( state, buff, TOP_R, 3 );
    sprintf(buff, "<enter>: %s", (!state->detonated)?"Allahu akbar":"restart");
    print_str( state, buff, TOP_R, 4 );
    
}


void convert_y( pstate* state, int *y )
{
    *y = state->term_max_y - *y -1;
}

void draw_a_bomb(pstate *state, int y, int x)
{
  #if WITH_COLOR
   attron( COLOR_PAIR(2) );
  #endif

   mvwaddstr( state->win, y, x, "@@");

  #if WITH_COLOR
   attroff( COLOR_PAIR(2) );
  #endif

}

void draw_cursor(pstate *state){
    
        //mvwaddch( state->win, state->cursor_pos_y, state->cursor_pos_x, '!') ;
        draw_a_bomb(state, state->cursor_pos_y, state->cursor_pos_x);

}

void draw_bombs(pstate *state)
{
    
        bomb *next = state->root_bomb;
        int x,y;
        while (next) {
            x= (int) next->x;
            y= (int) next->y;
            convert_y(state, &y);
            draw_a_bomb(state, y, x);
            //mvwaddch( state->win, y, x, '&');

            next = next->next;
        }

}

void draw(pstate *state)
{

    wclear(state->win);

    draw_particles(state);
    if (!state->detonated){
        draw_cursor(state);
        draw_bombs(state);
    }
    draw_overlay(state);

    wrefresh(state->win);

}

void tick(pstate *state)
{
    long long tstmp = time_stamp();
    if (state->prev_t < tstmp) //wrap around thing
        state->dt = (float) (tstmp - state->prev_t) / 1000.0; //10 ms will be 0.010
    state->prev_t = tstmp;

    if (state->paused)
        state->dt = 0.0;

    gravity( state );
    motion( state );

}



void init_colors()
{
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_RED );
    
}

int main(void)
{
    initscr(); //required ncurses thing


    keypad(stdscr, TRUE); //stdscr is the default newbie's window we're in i guess
    cbreak();

    attron(A_BOLD); //attritbute bold

    pstate current_state = {0};
    current_state.win = stdscr;

    pstate *state = &current_state;

    noecho();
    
#if WITH_COLOR
    init_colors();
#endif

    init_tick(state);

    while (!state->exit) {
        handle_input( state, getch() );
        tick(state);
        draw(state);
    }

    free_resources(state);

    endwin();

    return 0;

}
