#include "Robot.h"
/*
	Karel is a framework for manipulating with Karel the robot.
	Karel is a subclass of ROBOT from the library Robot.
	Last Modified:	September 16 2006, by Pieter Koopman, RU Nijmegen
	With some additions: September 12 2007, by Ger Paulussen & Peter Achten
	More additions: August 21 2013, by Peter Achten
*/


// =========================================================================

// Here are the examples that have been explained during the lecture
void take_3_steps ()
{
    step () ;
    step () ;
    step () ;
}

void swap_ball ()
{
    if (on_ball ())
        get_ball () ;
    else
	    put_ball () ;
    step () ;
}

// put_a_ball_somewhere adds a single ball to Charles' world on a random location
void put_a_ball_somewhere ()
{
    int lane   = 1 + rand () % (WereldHoogte  - 2) ;
    int street = 1 + rand () % (WereldBreedte - 2) ;
    create_ball (street, lane) ;
}

// make_church creates a Church-like shape of walls
void make_church ()
{
    place_rectangle (10,10,20,8) ;
    place_rectangle (10,18, 4,4) ;
    place_walls (12,22,4,false) ;
    place_walls (11,25,2,true) ;
}

// make_block creates an arbitrary block inside Charles' world and places a ball
// on the 'north' most lane at the 'west' most street corner.
void make_block ()
{
    const int left   = rand () % (WereldBreedte / 3) + 5;
    const int bottom = rand () % (WereldHoogte  / 4) + 5;
    const int width  = rand () % (WereldBreedte / 2) + 5;
    const int height = rand () % (WereldHoogte  / 2) + 2;
    place_rectangle (left,bottom,width,height) ;
    create_ball (left, WereldHoogte-2) ;
}
// turn_north makes Charles look north
void turn_north ()
{
    while (!north())
        turn_left();
}

void turn_around ()
{
    turn_left() ;
    turn_left() ;
}
// walk_to_wall makes Charles walk ahead and stop immediately before the wall
void walk_to_wall ()
{
    while (!in_front_of_wall())
        step () ;
}

// go_to_north_west_corner makes Charles move to north-west corner and look to the east
// it is assumed that that there are only the exterior walls
void go_to_north_west_corner ()
{
    turn_north() ;
    walk_to_wall() ;
    turn_left() ;
    walk_to_wall() ;
    turn_around() ;
}

// sweep_lane makes Charles walk ahead until it is on top of a ball or immediately before a wall
void sweep_lane ()
{
    while (!on_ball() && !in_front_of_wall())
        step () ;
}

// go_down_east_if_not_on_ball moves Charles one lane to the south if he is not on a ball.
// it is assumed that Charles is facing east and is not on the southernmost lane
void go_down_east_if_not_on_ball ()
{
    if (!on_ball())
    {
        turn_right() ;
        step() ;
        turn_right() ;
    }
}

// go_down_west_if_not_on_ball moves Charles one lane to the south if he is not on a ball.
// it is assumed that Charles is facing west and is not on the southernmost lane
void go_down_west_if_not_on_ball ()
{
    if (!on_ball())
    {
        turn_left () ;
        step () ;
        turn_left () ;
    }
}

// sweep_lanes makes Charles move to the east and find the ball, followed by
// going on lane south and moving to the west to find the ball.
// Assumptions: Charles is standing at the north-west corner and is looking east, there are only the exterior walls, and a ball is present.
// Result:      Charles is standing on the ball
void sweep_lanes ()
{
    while (!on_ball())
    {
        sweep_lane () ;
        go_down_east_if_not_on_ball () ;
        sweep_lane () ;
        go_down_west_if_not_on_ball () ;
    }
}

// This example has been discussed during the lecture
// The effect of this function is that Charles will search for a ball in its universe.
// Assumptions: Charles is standing on its base position, there are only the exterior walls, and a ball is present.
// Result:      Charles is standing on the ball.
void find_ball ()
{
    sweep_lanes () ;
}

// here starts the part with code to be completed by the students

// Name / student number / study student 1 : Ciske Harsema / s1010048 / Informatica
// Name / student number / study student 2 :

/*
    Part 1.1: cleaning up a string of balls
*/
//{

// Clean all balls along the edges by moving forward and turning corners till Charles no longer stands on a ball
void clean_string_with_balls ()
{
    // Setup code
	make_string_with_balls () ;

	while(on_ball()) {
        get_ball();

        // If Charles has reached a corner, turn right so he can continue tracing the walls.
        if(in_front_of_wall()) {
            turn_right();
        }

        step();
	}

	// At this point Charles is done, but facing north. Final turn to face east.
	turn_right();
}

//}

/*
    Part 1.2: cleaning up chaos with balls
*/
//{

void move_to_wall() {
    while(!in_front_of_wall()) {
        step();
    }
}

void clean_line() {
    while(on_ball()) {
        get_ball();

        // Make sure Charles never tries to go out of bounds if a ball is picked up at the edge
        if(!in_front_of_wall()) {
            step();
        }
    }
}

void move_down_line() {
    step();
    turn_left();
}

bool should_process_next_line() {
    turn_left();

    return !in_front_of_wall();
}

void process_line() {
    move_to_wall();
    turn_around();
    clean_line();
}

void move_back_to_start() {
    turn_right();
    move_to_wall();
    turn_right();
    move_to_wall();
    turn_right();
}

// Clean all balls by scanning from east to west on each line, till the south wall is hit
void clean_chaos_with_balls ()
{
    // Setup code
	make_chaos_with_balls () ;

	// A post-tested loop would be better suited, but this works too
	process_line();

	while(should_process_next_line()) {
        move_down_line();
        process_line();
	}

	// At this point Charles is done, but in the bottom-right corner facing south
	move_back_to_start();
}

//}

// this is the initial set-up for the tracing Charles assignment
void tracing_Charles ()
{
    reset () ;
    create_ball (WereldBreedte - 5, WereldHoogte - 3) ;
    steps (WereldBreedte - 5) ;
}

/*
    Bonus part 1.1: around the block
*/
//{

bool has_wall_on_left() {
    bool result;

    turn_left();
    result = in_front_of_wall();
    turn_right();

    return result;
}

void find_north_west_corner() {
    // Find the ball in the north lane, indicating the start of the rectangle
    while(!on_ball()) {
        step();
    }

    // Once the ball is found, move back one step and face south
    turn_around();
    step();
    turn_left();

    // Keep moving south till Charles is next to the rectangle
    while(!has_wall_on_left()) {
        step();
    }
}

void place_balls() {
    // Repeat 4 times, once for each side of the rectangle
    for(int i = 0; i < 4; ++i) {
        // Place balls along the edge
        while(has_wall_on_left()) {
            put_ball();
            step();
        }

        // Place corner ball and turn the corner
        put_ball();
        turn_left();
        step();
    }
}

void move_back_to_start_bonus() {
    turn_around();
    move_to_wall();
    turn_left();
    move_to_wall();
    turn_around();
}

// Places balls around a block by first finding the north-west corner, then moving along the edges
void around_the_block ()
{
    // Setup code
    make_block () ;

    find_north_west_corner();
    place_balls();
    move_back_to_start_bonus();
}

//}

/*
    Bonus part 1.2

    .OC.OX  <>
    .OC.OX  <4>
    .OC.OX  <4,false>
    .OC.OX  <4>
    .OC.OX  <5>
    .OC.OX  <5,1>
    .OC.OX  <5,1,true>
    .OC.OX  <5,1>
    .OC.OX  <5,2>
    .Oc.OX  <5>
    .Oc.OX  <6>
    .O.cOX  <4>
    .O.cOX  <4,false>
    .O.cOX  <4>
    .O.cOX  <5>
    .O.cOX  <5,1>
    .O.cOX  <5,1,false>
    .O.cOX  <5,1>
    .O.cOX  <5,3>
    .O.COX  <5>
    .O.COX  <6>
    .O.OCX  <4>
    .O.OCX  <4,true>
    .O.OCX  <4>
    .O.OCX  <>
*/

// Charles searches for the ball and remains there
void search_the_ball ()
{
	put_a_ball_somewhere () ;
	find_ball () ;
}

// For testing purposes, you can define your own function here:
void test ()
{
    // enter your Charles code here
}

// end of part with code to be completed by students
// =========================================================================


void quick  () { rest(    1); };
void normal () { rest(dInit); };
void slow   () { rest(  250); };
void very_slow  () { rest( 1000); };

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    Menu charles_menu ((char*)"Charles") ;
    charles_menu.add ((char*)"Clean", reset)
                .add ((char*)"Stop",  stop) ;

	Menu l1_menu ((char*)"Examples Lecture 1");
	l1_menu.add ((char*)"Steps",take_3_steps)
	       .add ((char*)"Swap ball",swap_ball)
	       .add ((char*)"Find ball",search_the_ball);

	Menu a1_menu ((char*)"Assignment 1");
	a1_menu.add ((char*)"Balls: a string...", clean_string_with_balls )
		   .add ((char*)"Balls: chaos...", clean_chaos_with_balls )
		   .add ((char*)"Balls: tracing Charles...", tracing_Charles )
		   .add ((char*)"Bonus: around the block...", around_the_block )
	       .add ((char*)"Test a function",test);

	Menu sn_menu ((char*)"Velocity");
	sn_menu.add ((char*)"Quick", quick)
		   .add ((char*)"Normal",normal)
		   .add ((char*)"Slow",slow)
		   .add ((char*)"Very slow",very_slow);

	try
	{
		karelsWereld().Run (charles_menu,WINARGS(hInstance, hPrevInstance, szCmdLine, iCmdShow));
	}
	catch (IllegaleActie dezeIllegaleActie )
	{
		dezeIllegaleActie.report ();
	}
	catch (...)
	{
		makeAlert ("Something went terribly wrong!");
	}

	return 0;
}
