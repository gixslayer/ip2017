#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <regex>
#include <thread>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

// region Mandatory

enum Cell {Dead=0, Live};                         // a cell is either Dead or Live (we use the fact that dead = 0 and live = 1)

const char DEAD             = '.' ;               // the presentation of a dead cell (both on file and screen)
const char LIVE             = '*' ;               // the presentation of a live cell (both on file and screen)
const int NO_OF_ROWS        = 40 ;                // the number of rows (height) of the universe (both on file and screen)
const int NO_OF_COLUMNS     = 60 ;                // the number of columns (width) of the universe (both on file and screen)
const int ROWS              = NO_OF_ROWS    + 2 ; // the number of rows in a universe array, including the 'frame' of dead cells
const int COLUMNS           = NO_OF_COLUMNS + 2 ; // the number of columns in a universe array, including the 'frame' of dead cells

const int MAX_FILENAME_LENGTH = 80 ;              // the maximum number of characters for a file name (including termination character)

/// The format a universe configuration file line must adhere to.
const regex CONFIG_LINE_FORMAT(R"(^[\.\*]{60}\r*)");
/// The minimum number of live neighbors a live cell must have to avoid dying of underpopulation.
const int MIN_CELL_POPULATION = 2;
/// The maximum number of live neighbors a live cell can have before it dies due to overpopulation.
const int MAX_CELL_POPULATION = 3;
/// The minimum number of live neighbors a dead cell must have to become alive again.
const int MIN_CELL_REPRODUCTION = 3;
/// The maximum number of live neighbors a dead cell can have to become alive again.
const int MAX_CELL_REPRODUCTION = 3;

//  Part 1: one-dimensional arrays
/// Lets the user enter a file name via the console.
/// \param filename - The buffer to hold the entered file name.
/// \return True if the input size (including termination character) did not exceed the buffer size, false otherwise.
bool enter_filename (char filename [MAX_FILENAME_LENGTH])
{
    // pre-conditions:
    assert(filename != nullptr);
    // post-conditions:
    // filename will contain at most MAX_FILENAME_LENGTH characters of the user input (including termination character).
    // return value is true if the user entered at most MAX_FILENAME_LENGTH characters (including termination character)
    // and false otherwise.

    string input;

    cout << "Enter filename (max length: " << MAX_FILENAME_LENGTH - 1 << "): ";
    getline(cin, input);

    // Make sure the copy length leaves enough room for the string terminator.
    size_t length = min(input.length(), static_cast<size_t>(MAX_FILENAME_LENGTH - 1));
    memcpy(filename, input.c_str(), length);
    filename[length] = 0;

    return input.length() <= length;
}

//  Part 2: setting the scene
/// Reads the universe state from a universe configuration file.
/// \param inputfile - The universe configuration file.
/// \param universe - The universe to receive the configuration.
/// \return True if the configuration was read successfully, false otherwise.
bool read_universe_file (ifstream& inputfile, Cell universe [ROWS][COLUMNS])
{
    // pre-conditions:
    assert(inputfile.is_open());
    assert(universe != nullptr);
    // post-conditions:
    // If the return value is true, the universe will contain the configuration read from the input file, and the dead
    // frame is placed around it. If an error occurs while reading false is returned.

    string input_row;

    // Fill top and bottom row with dead cells (part of frame).
    fill_n(&universe[0][0], COLUMNS, Dead);
    fill_n(&universe[ROWS-1][0], COLUMNS, Dead);

    /* fill_n could be replaced with this:
    for(int i = 0; i < COLUMNS; ++i) {
        universe[0][i] = Dead;
        universe[ROWS - 1][i] = Dead;
    }*/

    for(int row = 1; row <= NO_OF_ROWS; ++row) {
        if(!getline(inputfile, input_row)) {
            cerr << "Bad configuration file: could not read line " << row << endl;
            return false;
        } else if(!regex_match(input_row, CONFIG_LINE_FORMAT)) {
            cerr << "Bad configuration file: invalid format on line " << row << endl;
            return false;
        }

        // Set first and last column to dead cells (part of frame).
        universe[row][0] = Dead;
        universe[row][COLUMNS-1] = Dead;

        // Set the 'actual' row based on the line read from the input file.
        for(int column = 1; column <= NO_OF_COLUMNS; ++column) {
            universe[row][column] = input_row[column - 1] == DEAD ? Dead : Live;
        }
    }

    return true;
}

/// Prints the universe to the standard output stream.
/// \param universe - The universe to print.
void show_universe (Cell universe [ROWS][COLUMNS])
{
    // pre-conditions:
    assert(universe != nullptr);
    // post-conditions:
    // The universe has been printed to the standard output stream.

    for(int row = 0; row < ROWS; ++row) {
        for (int column = 0; column < COLUMNS; ++column) {
            cout << (universe[row][column] == Dead ? DEAD : LIVE);
        }

        cout << '\n';
    }

    cout << endl;
}

//  Part 3: the next generation
/// Gets the number of live neighbors for a given cell.
/// \param universe - The universe of the cell.
/// \param row - The row of the cell.
/// \param column - The column of the cell.
/// \return The number of live neighbors the cell has.
int get_num_live_neighbors(Cell universe[ROWS][COLUMNS], int row, int column) {
    // pre-conditions:
    assert(universe != nullptr);
    assert(row >= 0 && row <= NO_OF_ROWS);
    assert(column >= 0 && column <= NO_OF_COLUMNS);
    // post-conditions:
    // result value is the number of live neighbors for the cell at (row, column).

    int live_neighbors = 0;

    if(universe[row - 1][column - 1] == Live) ++live_neighbors;
    if(universe[row - 1][column + 0] == Live) ++live_neighbors;
    if(universe[row - 1][column + 1] == Live) ++live_neighbors;
    if(universe[row + 0][column - 1] == Live) ++live_neighbors;
    if(universe[row + 0][column + 1] == Live) ++live_neighbors;
    if(universe[row + 1][column - 1] == Live) ++live_neighbors;
    if(universe[row + 1][column + 0] == Live) ++live_neighbors;
    if(universe[row + 1][column + 1] == Live) ++live_neighbors;

    return live_neighbors;
}

/// Determine if \a value lies within a given inclusive range.
/// \param value - The value to test.
/// \param min - The minimum inclusive bound.
/// \param max - The maximum inclusive bound.
/// \return True if the value is within the range [min, max], false otherwise.
int in_range(int value, int min, int max) {
    // pre-conditions:
    assert(min <= max);
    // post-conditions:
    // return value is true if value is in the range [min, max], false otherwise.

    return value >= min && value <= max;
}

/// Computes the next generation of the universe \a now into \a next.
/// \param now - The current universe.
/// \param next - The universe to receive the next generation.
void next_generation (Cell now [ROWS][COLUMNS], Cell next [ROWS][COLUMNS])
{
    // pre-conditions:
    assert(now != nullptr);
    assert(next != nullptr);
    // post-conditions:
    // next will contain the next generation, generated from the current generation.

    // Fill top and bottom row with dead cells (part of frame).
    fill_n(&next[0][0], COLUMNS, Dead);
    fill_n(&next[ROWS-1][0], COLUMNS, Dead);

    for(int row = 1; row <= NO_OF_ROWS; ++row) {
        for(int column = 1; column <= NO_OF_COLUMNS; ++column) {
            int live_neighbors = get_num_live_neighbors(now, row, column);
            Cell curr_state = now[row][column];
            Cell next_state = curr_state;

            if(curr_state == Live && !in_range(live_neighbors, MIN_CELL_POPULATION, MAX_CELL_POPULATION)) {
                next_state = Dead; // Cell dies due to over/under-population.
            } else if(curr_state == Dead && in_range(live_neighbors, MIN_CELL_REPRODUCTION, MAX_CELL_REPRODUCTION)) {
                next_state = Live; // Cell becomes alive due to reproduction.
            }

            next[row][column] = next_state;
        }

        // Set first and last column to dead cells (part of frame).
        next[row][0] = Dead;
        next[row][COLUMNS - 1] = Dead;
    }
}

/// Driver for the mandatory part of the assignment.
void mandatory_driver() {
    char buffer[MAX_FILENAME_LENGTH];
    Cell universe[ROWS][COLUMNS];
    Cell next_universe[ROWS][COLUMNS];

    while (enter_filename(buffer) && strlen(buffer) != 0) {
        ifstream in_stream(buffer);

        if (!in_stream) {
            cerr << "Failed to open input file: " << strerror(errno) << endl;
        } else if (read_universe_file(in_stream, universe)) {
            show_universe(universe);
            next_generation(universe, next_universe);
            show_universe(next_universe);
        }
    }
}

// endregion

// region Bonus platform code

#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
// Use ANSI escape codes on *nix platforms.
const string live_color("\x1B[32m");
const string dead_color("\x1B[31m");
const string reset_color("\x1B[0m");
const string clear_console("\x1B[2J\x1B[3J\x1B[H");
#else
// Very ugly hacked code that *sort of* works for Windows. Yes it really needs error handling etc, but really CBA.
#include <Windows.h>

static int last_color = -1;

void set_console_color(ostream& stream, int color) {
    if (color == last_color) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    last_color = color;
    WORD wAttributes = (csbi.wAttributes & 0x00f0) | (color & 0x0f);

    SetConsoleTextAttribute(hConsole, wAttributes);
}

ostream& live_color(ostream& stream) {
    set_console_color(stream, FOREGROUND_GREEN);

    return stream;
}

ostream& dead_color(ostream& stream) {
    set_console_color(stream, FOREGROUND_RED);

    return stream;
}

ostream& reset_color(ostream& stream) {
    set_console_color(stream, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

    return stream;
}

ostream& clear_console(ostream& stream) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwCharsWritten;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, L' ', dwConSize, { 0, 0 }, &dwCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, { 0, 0 }, &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, { 0, 0 });

    return stream;
}
#endif

// endregion

// region Bonus

/// Possible actions the user can perform.
enum Action {
    LoadConfiguration = 1,
    SaveConfiguration,
    ShowUniverse,
    RunSteps,
    Exit,
    LastAction
};

/// Attempts to parse \a str as an integer.
/// \param str - The string to parse.
/// \param result - The resulting integer.
/// \return True if the integer was parsed successfully, false otherwise.
bool parse_int(const string& str, int& result) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // If parsing succeeded the return value is true and result contains the parsed integer.

    size_t pos;
    bool is_valid = false;

    try {
        // stoi reads until the first non-base character (1x is read as 1). Verify the entire string was actually read.
        result = stoi(str, &pos);
        is_valid = pos == str.length();
    } catch (...) {}

    return is_valid;
}

/// Gets an integer from the user in a specified inclusive range.
/// \param prompt - The prompt for the input.
/// \param min - The minimum value the input must have.
/// \param max - The maximum value the input can have.
/// \return The integer from user input in the range [min, max].
int get_int(const string& prompt, int min = 1, int max = numeric_limits<int>::max()) {
    // pre-conditions:
    assert(min <= max);
    // post-conditions:
    // return value is the integer from user input in the range [min, max].

    int value = 0;
    bool has_valid_int = false;
    string input;

    do {
        cout << prompt;
        getline(cin, input);

        if(!parse_int(input, value)) {
            cerr << "Input is not a valid integer\n" << endl;
        } else if(!in_range(value, min, max)) {
            cerr << "Input is not in range [" << min << ", " << max << "]\n" << endl;
        } else {
            has_valid_int = true;
        }
    } while(!has_valid_int);

    return value;
}

/// Writes the universe configuration to an output file stream.
/// \param out_stream - The output file stream to write to.
/// \param universe - The universe configuration to write.
void write_universe_file(ofstream& out_stream, Cell universe[ROWS][COLUMNS]) {
    // pre-conditions:
    assert(out_stream.is_open());
    assert(universe != nullptr);
    // post-conditions:
    // out_stream contains the universe configuration.

    for(int row = 1; row <= NO_OF_ROWS; ++row) {
        for(int column = 1; column <= NO_OF_COLUMNS; ++column) {
            out_stream << (universe[row][column] == Dead ? DEAD : LIVE);
        }

        out_stream << '\n';
    }
}

/// Prints the universe to the standard output stream using colored formatting.
/// \param universe - The universe to print.
void show_fancy_universe(Cell universe[ROWS][COLUMNS]) {
    // pre-conditions:
    assert(universe != nullptr);
    // post-conditions:
    // The universe has been printed to the standard output stream.

    for(int row = 0; row < ROWS; ++row) {
        for(int column = 0; column < COLUMNS; ++column) {
            if(universe[row][column] == Dead) {
                cout << dead_color << DEAD;
            } else {
                cout << live_color << LIVE;
            }
        }

        cout << '\n';
    }

    cout << reset_color << endl;
}

/// Asks the user for a universe configuration file to load, then attempts to load it.
/// \param universe - The universe to receive the configuration.
/// \note If the loading fails an error message is displayed and the content of \a universe is undefined.
void load_configuration(Cell universe[ROWS][COLUMNS]) {
    // pre-conditions:
    assert(universe != nullptr);
    // post-conditions:
    // If loading succeeded, universe contains the universe configuration from the user specified file. If loading
    // failed, an error message is displayed and the content of universe is undefined.

    char filename[MAX_FILENAME_LENGTH];

    if(!enter_filename(filename)) {
        cerr << "File name exceeded max input length" << endl;
    } else  {
        ifstream in_stream(filename);

        if(!in_stream) {
            cerr << "Could not open input file: " << strerror(errno) << endl;
        } else if(!read_universe_file(in_stream, universe)) {
            cerr << "Failed to load configuration file" << endl;
        } else {
            cout << "Loaded universe configuration from file " << filename << '\n' << endl;

            show_fancy_universe(universe);
        }
    }
}

/// Asks the user for a universe configuration file to save to, then attempts to save the universe.
/// \param universe - The universe to save to the output file.
/// \note If the saving fails an error message is displayed and the content of the output file is undefined.
void save_configuration(Cell universe[ROWS][COLUMNS]) {
    // pre-conditions:
    assert(universe != nullptr);
    // post-conditions:
    // If saving succeeded, the output file contains the universe configuration from universe. If saving failed, an
    // error message is displayed and the content of the output file is undefined.

    char filename[MAX_FILENAME_LENGTH];

    if(!enter_filename(filename)) {
        cerr << "File name exceeded max input length" << endl;
    } else  {
        ofstream out_stream(filename);

        if(!out_stream) {
            cerr << "Could not open output file: " << strerror(errno) << endl;
        } else {
            write_universe_file(out_stream, universe);
            out_stream.close();

            if(!out_stream) {
                cerr << "Failed to close output file, content might not be correct" << endl;
            } else {
                cout << "Saved universe configuration to file " << filename << endl;
            }
        }
    }
}

/// Computes a user specified number of steps on the universe and displays all the intermediate results.
/// \param universe - The universe to begin computation from, will also contain the new universe.
void run_steps(Cell universe[ROWS][COLUMNS]) {
    // pre-conditions:
    assert(universe != nullptr);
    // post-conditions:
    // All steps have been printed to the standard output stream, and universe contains the last computed universe.

    Cell next_universe[ROWS][COLUMNS];

    int steps = get_int("Number of steps: ");
    int animation_delay = get_int("Animation delay (in ms): ");

    for(int step = 0; step < steps; ++step) {
        // next_universe is used to temporarily hold the next generation, until it is copied back into universe.
        next_generation(universe, next_universe);
        memcpy(universe, next_universe, ROWS*COLUMNS*sizeof(Cell));

        cout << clear_console;
        show_fancy_universe(universe);

        chrono::milliseconds delay(animation_delay);
        this_thread::sleep_for(delay);
    }
}

/// Prompts the user to select and action, then returns the selected action.
/// \return The selected action.
Action get_action() {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // return value is the action selected by the user.

    cout << "\n[Select action]" << endl;
    cout << "1. Load configuration - load universe from configuration file" << endl;
    cout << "2. Save configuration - save universe to configuration file" << endl;
    cout << "3. Show universe - show the current universe" << endl;
    cout << "4. Run steps - compute a number of generations" << endl;
    cout << "5. Exit - exit the application\n" << endl;

    int input = get_int(">", 1, LastAction - 1);

    return static_cast<Action>(input);
}

/// Handles the selected user action.
/// \param action - The action to perform.
/// \param universe - The current universe.
/// \return True if the application must continue running, false if it must exit.
bool handle_action(Action action, Cell universe[ROWS][COLUMNS]) {
    // pre-conditions:
    assert(universe != nullptr);
    // post-conditions:
    // The action has been performed and the return value determines if the application should continue running.

    switch(action) {
        case LoadConfiguration:
            load_configuration(universe);
            return true;
        case SaveConfiguration:
            save_configuration(universe);
            return true;
        case ShowUniverse:
            show_fancy_universe(universe);
            return true;
        case RunSteps:
            run_steps(universe);
            return true;
        case Exit:
        default:
            return false;
    }
}

/// Driver for the bonus part of the assignment.
void bonus_driver() {
    Action action;
    Cell universe[ROWS][COLUMNS];

    // Initialize universe to an all dead state.
    fill_n(&universe[0][0], ROWS*COLUMNS, Dead);

    do {
        action = get_action();
    } while(handle_action(action, universe));
}

// endregion

/// Application entry point.
/// \return The applications exit code.
int main()
{
    //mandatory_driver();
    bonus_driver();

    return EXIT_SUCCESS;
}
