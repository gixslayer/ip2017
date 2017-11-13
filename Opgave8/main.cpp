// Suppress deprecation errors on strerror when building on MSVC
#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cassert>
#include <iomanip>
#include <limits>
#include <sstream>
#include <algorithm>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

// region Mandatory

struct Length
{
	int minutes;							// #minutes (0..)
	int seconds;							// #seconds (0..59)
};

struct Track
{
	string artist;                          // name of artist
	string cd;                              // cd title
	int    year;                            // year of appearance (if known)
	int    track;							// track number
	string title;                           // track title
	string tags;                            // tags
	Length time;							// playing time
	string country;                         // main countr(y/ies) of artist
};

struct TrackDisplay
{
	bool showArtist;						// if true, show name of artist
	bool showAlbum;							// if true, show cd title
	bool showYear;							// if true, show year of appearance
	bool showTrack;							// if true, show track number
	bool showTitle;							// if true, show track title
	bool showTags;                          // if true, show tags
	bool showLength;						// if true, show playing time
	bool showCountry;                       // if true, show countr(y/ies) of artist
};

/// Expected number of entries in Nummers.txt.
constexpr int NUM_ENTRIES = 5623;

bool match (string sub, string source)
{// Precondition:
    assert (true) ;
/*  Postcondition:
    Result is true only if sub is a literal (case sensitive) sub string of source.
*/
    return source.find(sub) != string::npos ;
}

ostream& operator<< (ostream& os, const Length length)
{// Precondition:
    assert (true) ;
/*  Postcondition:
    the value of length is shown via out in the format: minutes, ':', seconds (two digits)
*/

    os << setfill('0') << setw(2) << length.minutes << ':' << setw(2) << length.seconds;

    return os;
}

istream& operator>> (istream& is, Length& length)
{// Precondition:
    assert (true) ;
/*  Postcondition:
    the value of length has been read from in: first minutes, then ':', then seconds
*/

    is >> length.minutes;
    is.ignore(); // Ignore ':' character.
    is >> length.seconds;

    return is;
}

Length operator+ (const Length& a, const Length& b)
{// Precondition:
    assert (0 <= a.minutes && 0 <= a.seconds && a.seconds < 60 && 0 <= b.minutes && 0 <= b.seconds && b.seconds < 60) ;
/*  Postcondition:
    Result is the sum of a and b.
*/

    int total_seconds = a.minutes * 60 + a.seconds + b.minutes * 60 + b.seconds;

    return {total_seconds / 60, total_seconds % 60};
}

void test_length_operators() {
    // pre:
    assert(true);
    // post:
    // Length operators have been tested and verified by the user.

    vector<Length> test_cases = {{42, 42}, {0, 0}, {3, 21}, {3, 14}};

    cout << "Testing << operator" << endl;
    for(auto test_case : test_cases) {
        cout << test_case << " m=" << test_case.minutes << "\ts=" << test_case.seconds << endl;
    }

    cout << "\nTesting + operator" << endl;
    for(auto case_a : test_cases) {
        for(auto case_b : test_cases) {
            cout << case_a << " + " << case_b << " = " << case_a + case_b << endl;
        }
    }

    cout << "\nTesting >> operator (enter 0:0 to stop)" << endl;
    Length length = {};

    do {
        cin >> length;

        cout << "m=" << length.minutes << "\ts=" << length.seconds << endl;

    } while(length.minutes != 0 && length.seconds != 0);
}

/// Show the specified track information.
/// \param track - The track to show information from.
/// \param lt - What information to show from the track.
void show_track (Track track, TrackDisplay lt)
{// Precondition:
    assert (true) ;
/*  Postcondition:
    only the members of track are shown for which the corresponding member in lt is true.
*/

    if (lt.showTitle)   cout << "Title: "   << track.title << '\n';
    if (lt.showArtist)  cout << "Artist: "  << track.artist << '\n';
    if (lt.showLength)  cout << "Length: "  << track.time << '\n';
    if (lt.showTrack)   cout << "Track: "   << track.track << '\n';
    if (lt.showAlbum)   cout << "Album: "   << track.cd << '\n';
    if (lt.showYear)    cout << "Year: "    << track.year << '\n';
    if (lt.showCountry) cout << "Country: " << track.country << '\n';
    if (lt.showTags)    cout << "Tags: "    << track.tags << '\n';

    cout << endl;
}

/// Get a line from is, strip the trailing CR character if it exists, and save the result in str.
/// \param is - The stream to read from.
/// \param str - The string that receives the read line.
void getcleanline(istream& is, string& str) {
    // pre:
    assert(true);
    // post:
    // str contains the next read line from is, stripped from a trailing \r character.

    std::getline(is, str);

    if(!str.empty() && str[str.length()-1] == '\r') {
        str.erase(str.end() - 1, str.end());
    }
}

/// Read a track from the input stream.
/// \param in - The input stream to read from.
/// \param track - The track to hold the result
/// \return The input stream \a in.
istream& operator>> (istream& in, Track& track)
{// Precondition:
    assert (true) ;
/*  Postcondition:
    the content of the first 8 lines from in have been read and are stored in the corresponding members of track.
    The following (empty) line from in has also been read.
*/
    string year;
    string track_nr;
    string time;

    getcleanline(in, track.artist);
    getcleanline(in, track.cd);
    getcleanline(in, year);
    getcleanline(in, track_nr);
    getcleanline(in, track.title);
    getcleanline(in, track.tags);
    getcleanline(in, time);
    getcleanline(in, track.country);
    in.ignore(numeric_limits<streamsize>::max(), '\n'); // Skip blank line, which might consist of multiple characters.

    // Using this over direct formatted input, as it's a complete nightmare to get any kind of consistency across
    // multiple platforms otherwise.
    track.year = atoi(year.c_str());
    track.track = atoi(track_nr.c_str());
    stringstream ss(time);
    ss >> track.time;

    return in;
}

/// Read the track database from the input file stream.
/// \param ifs - The stream to read the database from.
/// \return All read tracks from the file, in the order they were read.
vector<Track> read_database(ifstream& ifs) {
    // pre:
    assert(ifs.is_open());
    // post:
    // return value is the vector of all Track entries read from ifs.

    Track track;
    vector<Track> database;
    TrackDisplay display = {true, true, true, true, true, true, true, true};

    while((ifs >> track, ifs.good())) {
        database.push_back(track);

        show_track(track, display);
    }

    return database;
}

/// Check if a vector contains a specific element.
/// \tparam T - The type of the element.
/// \param vector - The vector to search.
/// \param elem - The element to search for.
/// \return True if the element was found, false otherwise.
template<typename T>
bool contains(const vector<T>& vector, const T& elem) {
    // pre:
    assert(true);
    // post:
    // return value is true if vector contains elem, false otherwise.

    return std::find(vector.cbegin(), vector.cend(), elem) != vector.cend();
}

/// List all tracks that (partially) match the specified title.
/// \param tracks - The database of tracks to list from.
/// \param track - The (partial) title of the track.
/// \param display - Display matches to the standard output stream.
/// \return The number of matches.
int match_tracks(const vector<Track>& tracks, const string& track, bool display) {
    // pre:
    assert(true);
    // post:
    // returns the number of tracks in tracks, such that the parameter track is a substring of the track title. If
    // display is true, then the title, artist, album, year, track, time, country and tags of each match is displayed.

    int num_matches = 0;
    TrackDisplay track_display = {true, true, true, true, true, true, true, true};

    for(auto entry : tracks) {
        if(match(track, entry.title)) {
            ++num_matches;

            if(display) {
                show_track(entry, track_display);
            }
        }
    }

    return num_matches;
}

/// List all artists that (partially) match the specified artist.
/// \param tracks - The database of tracks to list from.
/// \param artist - The (partial) artist name of the track.
/// \param display - Display matches to the standard output stream.
/// \return The number of matches.
int match_artists(const vector<Track>& tracks, const string& artist, bool display) {
    // pre:
    assert(true);
    // post:
    // returns the number of unique artists in tracks. If display is true, each unique artist is displayed.

    TrackDisplay track_display = {true};
    vector<string> matches = {};

    for(auto entry : tracks) {
        if(match(artist, entry.artist) && !contains(matches, entry.artist)) {
            matches.push_back(entry.artist);

            if(display) {
                show_track(entry, track_display);
            }
        }
    }

    return static_cast<int>(matches.size());
}

/// List all albums that (partially) match the specified artist.
/// \param tracks - The database of tracks to list from.
/// \param artist - The (partial) artist name of the track.
/// \param display - Display matches to the standard output stream.
/// \return The number of matches.
int match_cds(const vector<Track>& tracks, const string& artist, bool display) {
    // pre:
    assert(true);
    // post:
    // returns the number of unique (artist,cd) pairs in tracks, such that the parameter artist is a substring of the
    // artist element in each pair. If display is true, then the album title, artist and year is displayed for each
    // unique match.

    TrackDisplay track_display = {true, true, true};
    vector<pair<string, string>> matches = {};

    for(auto entry : tracks) {
        auto pair = make_pair(entry.artist, entry.cd);

        if(match(artist, entry.artist) && !contains(matches, pair)) {
            matches.push_back(pair);

            if(display) {
                show_track(entry, track_display);
            }
        }
    }

    return static_cast<int>(matches.size());
}

/// Count the number of unique albums.
/// \param tracks - The database of tracks to count from.
/// \return The number of unique (artist,cd) pairs in \a tracks.
int number_of_cds(const vector<Track>& tracks) {
    // pre:
    assert(true);
    // post:
    // returns the number of unique (artist,cd) pairs in tracks.

    vector<pair<string, string>> matches = {};

    for(auto entry : tracks) {
        auto pair = make_pair(entry.artist, entry.cd);

        if(!contains(matches, pair)) {
            matches.push_back(pair);
        }
    }

    return static_cast<int>(matches.size());
}

// endregion

// region Bonus

/// Enumerates the possible user commands.
enum Command {find_track, find_artist, find_cd_by_artist, num_cds, stop, find_cd_with_track, find_artist_with_cd,
    runningtime_of_cd, total_running_time, find_cd_with_year};
/// The text of each user command.
const vector<string> COMMAND_TEXT = {"track", "artist", "cds", "#cds", "stop", "cd?", "artist?", "runningtime", "time",
                                     "year"};

/// The number of seconds in each minute.
constexpr int SECONDS_PER_MINUTE = 60;
/// The number of seconds in each hour.
constexpr int SECONDS_PER_HOUR = 60*SECONDS_PER_MINUTE;
/// The number of seconds in each day.
constexpr int SECONDS_PER_DAY = 24*SECONDS_PER_HOUR;

/// Split a line on the first space character.
/// \param line - The line to split
/// \return A pair of strings (a,b) such that a is all the text of \a line before the first space, and b is all the text
/// after the first space. If \a line contains no space character, a equals \a line and b is empty.
pair<string, string> split_cmd_line(const string& line) {
    auto index = line.find(' ');

    if(index == string::npos) {
        return make_pair(line, "");
    } else {
        string cmd = line.substr(0, index);
        string args = line.substr(index + 1);

        return make_pair(cmd, args);
    }
}

/// Get a user command from the standard input.
/// \param prompt - The prompt to give the user.
/// \return The user command, which is a pair of the command and argument string.
pair<Command, string> get_cmd(const string& prompt) {
    string line;
    bool has_cmd = false;
    Command cmd;
    string args;

    do {
        cout << prompt;
        getline(cin, line);

        auto split_cmd = split_cmd_line(line);
        auto index = find(COMMAND_TEXT.cbegin(), COMMAND_TEXT.cend(), split_cmd.first);

        if(index != COMMAND_TEXT.cend()) {
            cmd = static_cast<Command>(index - COMMAND_TEXT.cbegin());
            args = split_cmd.second;

            has_cmd = true;
        } else {
            cerr << "Unknown command " << split_cmd.first << endl;
        }
    } while(!has_cmd);

    return make_pair(cmd, args);
}

/// Execute the 'find' command.
/// \param database - The database of tracks.
/// \param track - The 'track' argument.
void cmd_find_track(const vector<Track>& database, const string& track) {
    // pre:
    assert(true);
    // post:
    // The command 'track track' has been executed.

    int matches = match_tracks(database, track, true);

    cout << matches << (matches == 1 ? " match" : " matches") << " found" << endl;
}

/// Execute the 'artist' command.
/// \param database - The database of tracks.
/// \param artist - The 'artist' argument.
void cmd_find_artist(const vector<Track>& database, const string& artist) {
    // pre:
    assert(true);
    // post:
    // The command 'artist artist' has been executed.

    int matches = match_artists(database, artist, true);

    cout << matches << (matches == 1 ? " match" : " matches") << " found" << endl;
}

/// Execute the 'cds' command.
/// \param database - The database of tracks.
/// \param artist - The 'artist' argument.
void cmd_find_cd_by_artist(const vector<Track>& database, const string& artist) {
    // pre:
    assert(true);
    // post:
    // The command 'cds artist' has been executed.

    int matches = match_cds(database, artist, true);

    cout << matches << (matches == 1 ? " match" : " matches") << " found" << endl;
}

/// Execute the '#cds' command.
/// \param database - The database of tracks.
void cmd_number_of_cds(const vector<Track>& database) {
    // pre:
    assert(true);
    // post:
    // The command '#cds' has been executed.

    cout << "Total number of CDs: " << number_of_cds(database) << endl;
}

/// Execute the 'cd?' command.
/// \param database - The database of tracks.
/// \param track - The 'track' argument.
void cmd_find_cd_with_track(const vector<Track>& database, const string& track) {
    // pre:
    assert(true);
    // post:
    // The command 'cd? track' has been executed.

    vector<pair<string, string>> matches = {};
    TrackDisplay display = {true, true, true};

    for(const auto& entry : database) {
        auto pair = make_pair(entry.artist, entry.cd);

        if(match(track, entry.title) && !contains(matches, pair)) {
            matches.push_back(pair);

            show_track(entry, display);
        }
    }

    cout << matches.size() << (matches.size() == 1 ? " match" : " matches") << " found" << endl;
}

/// Execute the 'artist?' command.
/// \param database - The database of tracks.
/// \param cd - The 'cd' argument.
void cmd_find_artist_with_cd(const vector<Track>& database, const string& cd) {
    // pre:
    assert(true);
    // post:
    // The command 'artist? cd' has been executed.

    vector<pair<string, string>> matches = {};
    TrackDisplay display = {true, true};

    for(const auto& entry : database) {
        auto pair = make_pair(entry.artist, entry.cd);

        if(match(cd, entry.cd) && !contains(matches, pair)) {
            matches.push_back(pair);

            show_track(entry, display);
        }
    }

    cout << matches.size() << (matches.size() == 1 ? " match" : " matches") << " found" << endl;
}

/// Execute the 'runningtime' command.
/// \param database - The database of tracks.
/// \param cd - The 'cd' argument.
void cmd_running_time_of_cd(const vector<Track>& database, const string& cd) {
    // pre:
    assert(true);
    // post:
    // The command 'runningtime cd' has been executed.

    vector<pair<string, string>> matches = {};

    for(const auto& entry : database) {
        auto pair = make_pair(entry.artist, entry.cd);

        if(match(cd, entry.cd) && !contains(matches, pair)) {
            matches.push_back(pair);
        }
    }

    for(const auto& match : matches) {
        cout << "Artist: " << match.first << endl;
        cout << "Album: " << match.second << endl;
        cout << "Running time: ";

        int total_time = 0;

        for(const auto& entry : database) {
            if(entry.artist == match.first && entry.cd == match.second) {
                total_time += entry.time.minutes * SECONDS_PER_MINUTE + entry.time.seconds;
            }
        }

        int hours = total_time / SECONDS_PER_HOUR;
        total_time -= hours * SECONDS_PER_HOUR;
        int minutes = total_time / SECONDS_PER_MINUTE;
        int seconds = total_time % SECONDS_PER_MINUTE;

        if(hours != 0) {
            cout << hours << (hours == 1 ? " hour, " : " hours, ");
        }

        cout << minutes << (minutes == 1 ? " minute and " : " minutes and ")
             << seconds << (seconds == 1 ? " second\n" : " seconds\n") << endl;
    }

    cout << matches.size() << (matches.size() == 1 ? " match" : " matches") << " found" << endl;
}

/// Execute the 'time' command.
/// \param database - The database of tracks.
void cmd_total_running_time(const vector<Track>& database) {
    // pre:
    assert(true);
    // post:
    // The command 'time' has been executed.

    int total_duration = 0;

    for(const auto& entry : database) {
        total_duration += entry.time.minutes * 60 + entry.time.seconds;
    }

    int days = total_duration / SECONDS_PER_DAY;
    total_duration -= days * SECONDS_PER_DAY;
    int hours = total_duration / SECONDS_PER_HOUR;
    total_duration -= hours * SECONDS_PER_HOUR;
    int minutes = total_duration / SECONDS_PER_MINUTE;
    int seconds = total_duration % SECONDS_PER_MINUTE;

    cout << "Total running time: "
         << days << (days == 1 ? " day, " : " days, ")
         << hours << (hours == 1 ? " hour, " : " hours, ")
         << minutes << (minutes == 1 ? " minute and " : " minutes and ")
         << seconds << (seconds == 1 ? " second" : " seconds") << endl;
}

/// Execute the 'year' command.
/// \param database - The database of tracks.
/// \param year The 'y' argument.
void cmd_find_cd_with_year(const vector<Track>& database, const string& year) {
    // pre:
    assert(true);
    // post:
    // The command 'year y' has been executed.

    vector<pair<string, string>> matches = {};
    int y = atoi(year.c_str());
    TrackDisplay display = {true, true};

    for(const auto& entry : database) {
        auto pair = make_pair(entry.artist, entry.cd);

        if(entry.year == y && !contains(matches, pair)) {
            matches.push_back(pair);

            show_track(entry, display);
        }
    }

    cout << matches.size() << (matches.size() == 1 ? " match" : " matches") << " found" << endl;
}

/// Handles a user command.
/// \param database - The database of tracks.
/// \param cmd - The command to execute.
/// \return True if the application should continue executing, false otherwise.
bool handle_cmd(const vector<Track>& database, const pair<Command, string>& cmd) {
    // pre:
    assert(true);
    // post:
    // return value is true if the application should continue, false otherwise, and cmd has been executed.

    switch(cmd.first) {
        case find_track:            cmd_find_track(database, cmd.second);           return true;
        case find_artist:           cmd_find_artist(database, cmd.second);          return true;
        case find_cd_by_artist:     cmd_find_cd_by_artist(database, cmd.second);    return true;
        case num_cds:               cmd_number_of_cds(database);                    return true;
        case find_cd_with_track:    cmd_find_cd_with_track(database, cmd.second);   return true;
        case find_artist_with_cd:   cmd_find_artist_with_cd(database, cmd.second);  return true;
        case runningtime_of_cd:     cmd_running_time_of_cd(database, cmd.second);   return true;
        case total_running_time:    cmd_total_running_time(database);               return true;
        case find_cd_with_year:     cmd_find_cd_with_year(database, cmd.second);    return true;
        case stop: return false;
        default: return false;
    }
}

/// Driver for the bonus part of the assignment.
/// \param database - The database of tracks.
void bonus_driver(const vector<Track>& database) {
    pair<Command, string> cmd;

    do {
        cout << "\nAvailable commands:";
        for(const auto& command : COMMAND_TEXT) {
            cout << " " << command;
        }
        cout << endl;

        cmd = get_cmd(">");
    } while(handle_cmd(database, cmd));
}

// endregion

/// Application entry point.
/// \return The application's exit code.
int main() {
// Precondition:
    assert (true);
/*  Postcondition:
    the music database "Nummers.txt" has been read (if present and correctly formatted).
    The assignment queries have been executed and their result has been shown on screen.
    In case of the bonus assignment, the user has been able to query the database and has
    seen the results of these queries.
*/
    //test_length_operators();

    ifstream ifs("../Nummers.txt");
    if (ifs) {
        vector<Track> database = read_database(ifs);

        cout << "Read " << database.size() << " database entries, expected " << NUM_ENTRIES << " ("
             << (database.size() == NUM_ENTRIES ? "OK)" : "BAD)") << endl;

        bonus_driver(database);
    } else {
        cerr << "Failed to open input file: " << strerror(errno) << endl;
    }

	return EXIT_SUCCESS;
}
