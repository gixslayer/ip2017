#include "Robot.h"
/*
	Karel is a framework for manipulating with Karel the robot.
	Karel is a subclass of ROBOT from the library Robot.
	Last Modified:	September 16 2006, by Pieter Koopman, RU Nijmegen
	With some additions: September 12 2007, by Ger Paulussen & Peter Achten
	More additions: August 21 2013, by Peter Achten
*/


// =========================================================================

// make_church creates a Church-like shape of walls
void make_church ()
{
    const int lane   = 2 + rand () % (WereldHoogte / 5) ;
    const int street = 2 + rand () % (WereldBreedte / 5) ;
    const int width  = 3 + rand () % (WereldBreedte / 2) ;
    const int height = 2 + rand () % (WereldHoogte / 3) ;
    place_rectangle (street,lane,width,height) ;
    place_rectangle (street + 2,lane + height, 2 + width / 3, 2 + height / 3) ;
    place_walls (street + 3 + width / 6,lane + height + height / 3 + 3,4,false) ;
    place_walls (street + 2 + width / 6,lane + height + height / 3 + 5,2,true) ;
    create_ball (street, WereldHoogte - 2) ;
}

// safe_put_ball makes Charles put a ball on his current location if it is empty
void safe_put_ball ()
{
    if (!on_ball ())
        put_ball () ;
}

// safe_step makes Charles move one step ahead if not blocked by a wall
void safe_step ()
{
    if (!in_front_of_wall())
        step () ;
}

// balls (n) makes Charles place at most n balls straight ahead (if not blocked by a wall)
// in a safe way (putting balls only on empty spots)
void balls (int no_of_balls)
{
    for (int i = 1 ; i < no_of_balls ; i++)
    {
        safe_put_ball () ;
        safe_step () ;
    }
    if (no_of_balls > 0)
        safe_put_ball () ;
}

// here starts the part with code to be completed by the students

// Name / student number / study student 1 : Ciske Harsema / s1010048 / Informatica
// Name / student number / study student 2 :

/*
    Part 1.1: Hansl and Gretl
*/
//{

// Turns Charles around 180 degrees.
void turn_around() {
    turn_left();
    turn_left();
}

// Makes Charles take a step back, while maintaining the current direction.
// NOTE: Assumes Charles can step back without running into a wall.
void step_back() {
    turn_around();
    step();
    turn_around();
}

// Makes Charles face east.
void face_east() {
    while(!north()) {
        turn_right();
    }

    turn_right();
}

// Determines whether Charles is currently in front of a ball.
bool in_front_of_ball() {
    bool result = false;

    if(!in_front_of_wall()) {
        step();
        result = on_ball();
        step_back();
    }

    return result;
}

// Follow a straight line of balls until the line ends (due to a lack of balls
// or a wall). After the end is found, Charles will be on the last ball facing
// the direction this function was called with.
void follow_path_line() {
    while(on_ball() && !in_front_of_wall()) {
        step();
    }

    if(!in_front_of_wall()) {
        step_back();
    }
}

// Tries to determine the next path line by first looking to the right, then
// the left, of the current direction. If a new path is found then Charles will
// face that direction and the function returns true. If no path is found the
// direction of Charles remains unchanged and the function returns false.
bool find_next_path_line() {
    // First check right of the original direction.
    turn_right();
    if(in_front_of_ball()) {
        return true;
    }

    // Then check left of the original direction.
    turn_around();
    if(in_front_of_ball()) {
        return true;
    }

    // Turn right once more to maintain the original direction if no path is found.
    turn_right();
    return false;
}

// Follows the path by following each straight line until the end, Charles then
// tries to find the next direction of the path. If no new direction can be found
// Charles has reached the end of the path.
void follow_path ()
{
    do {
        follow_path_line();
    } while(find_next_path_line());

    // SA note: I assume post-tested while loops (do-while) are allowed
    // if not, here is the same code using a pre-tested loop.
    /*follow_path_line();
    while(find_next_path_line()) {
        follow_path_line();
    }*/

    // As per assignment requirements, Charles has to face east.
    face_east();
}

void hansl_and_gretl ()
{
	make_path_with_balls() ;
	follow_path () ;
}

//}

/*
    Part 1.2: stalactites and stalagmites
*/
//{

// Walk in the current direction placing balls till a wall is reached.
// NOTE: Assumes the path is clear of any balls.
void fill_till_wall() {
    while(!in_front_of_wall()) {
        put_ball();
        step();
    }

    // At this point Charles is in front of a wall, so he cannot take another
    // step, but still has to place a ball on the current position.
    put_ball();
}

// Walk in the current direction till a wall is reached.
void walk_till_wall() {
    while(!in_front_of_wall()) {
        step();
    }
}

// Naive version: fill from outer wall to inner wall, then turn around and walk
// back to the outer wall.
void fill_cave_line() {
    turn_right();
    fill_till_wall();
    turn_around();
    walk_till_wall();
    turn_right();
}

// Optimized version: fill from outer wall to inner wall, then check if the
// inner wall continues at the same level. If it does continue, fill from
// inner wall to outer wall. If it does not continue, then move back to the
// outer wall without placing balls via the line filled previously.
void fill_cave_line_optimized() {
    // Fill from outer wall to inner wall.
    turn_right();
    fill_till_wall();
    turn_left();

    if(!in_front_of_wall()) {
        // Move over to the next line and face the potential wall.
        step();
        turn_right();

        if(in_front_of_wall()) {
            // Wall found, turn around and fill from inner wall to outer wall.
            turn_around();
            fill_till_wall();
            turn_right();
        } else {
            // No wall found, step back to the previous line and walk back up.
            turn_right();
            step();
            turn_right();
            walk_till_wall();
            turn_right();
        }
    } else {
        // Charles was already facing a wall and could not step over to the next
        // line, so he knows the inner wall does not continue at the same level.
        // Walk back up to the outer wall.
        turn_left();
        walk_till_wall();
        turn_right();
    }
}

// Fills one half of the cave (top or bottom), and ends in the opposing corner.
void fill_cave_with_balls ()
{
    while(!in_front_of_wall()) {
        step();

        // Check to see if Charles is in the north-east or south-west corner.
        if(in_front_of_wall()) {
            // If he is, walk to the next corner.
            turn_right();
            walk_till_wall();
        } else {
            // If not, fill the stalactite/stalagmite below/above Charles.
            // SA note: there are 2 versions which should both work, one naive and one optimized.
            //fill_cave_line();
            fill_cave_line_optimized();
        }
    }

    // At this point Charles is at the south-east or north-west corner, facing south or north.
    // Turn right to face the corner Charles will move to next.
    turn_right();
}

void cave ()
{
	fill_cave_with_balls () ;
	fill_cave_with_balls () ;
}

void start_cave ()
{
    make_cave () ;
    cave () ;
}

//}

/*
    Bonus: Around the church
*/
//{

// Moves in the current direction until Charles is standing on a ball or is facing a wall.
void find_ball() {
    while(!on_ball() && !in_front_of_wall()) {
        step();
    }
}

// Finds the church by first finding the guide ball, then moving down till a
// part of the church is hit.
void find_church() {
    find_ball();
    turn_right();
    walk_till_wall();
    turn_right();
}

// Determines whether there is a wall to the left of Charles's current position.
bool left_of_wall() {
    bool result = false;

    turn_left();
    result = in_front_of_wall();
    turn_right();

    return result;
}

// Continues to trace the church wall until it ends, or a wall is hit.
void walk_along_church_wall() {
    while(left_of_wall() && !in_front_of_wall()) {
        step();
    }
}

// Turns and marks a church corner.
void turn_church_corner() {
    // Mark the corner so Charles can know he passed it.
    put_ball();
    turn_left();
    step();
}

// Walks around the church by tracing the church walls and turning when required.
void walk_around_church() {
    while(!on_ball()) {
        walk_along_church_wall();

        if(in_front_of_wall()) {
            turn_right();
        } else if(!on_ball()) {
            // Only turn the corner if it isn't already marked as passed.
            turn_church_corner();
        }
    }
}

// Walks back to the starting position once Charles has walked around the church.
// NOTE: Assumes Charles is facing west and will not run into obstacles.
void walk_to_start() {
    walk_till_wall();
    turn_right();
    walk_till_wall();
    turn_right();
}

// Find the church, then walk around it, and finally walk back to the starting position.
void rondje_om_de_kerk ()
{
    make_church () ;

    find_church();
    walk_around_church();
    walk_to_start();
}

//}

// For testing purposes, you can define your own function here:
void test ()
{
    // enter your Charles code here
    balls (8/3) ;
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

	Menu a1_menu ((char*)"Assignment 2");
	a1_menu.add ((char*)"Hansl and Gretl", hansl_and_gretl )
		   .add ((char*)"Cave", start_cave )
		   .add ((char*)"Bonus: rondje om de kerk...", rondje_om_de_kerk )
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
