#define TRACK_LENGTH_COMPARISON // Enable to compare tracks by running time.
#define PROFILE // Enable to profile Track operator invocations.
#define VISUAL_PROFILE // Enable to visually profile Track operator invocations.

#ifdef VISUAL_PROFILE
#ifndef PROFILE
#define PROFILE // Visual profiling requires normal profiling enabled.
#endif
#endif

#ifdef PROFILE
#define NDEBUG // Disable asserts while profiling.
// Thread local is important here, as the code is multi-threaded and each algorithm runs in its own thread, and thus
// requires this counter to be thread local.
static thread_local int num_operations;
#endif

/*********************************************************************
*   Voorbeeldraamwerk voor opdracht 9 IPC031.
*   Inhoud:
*   - de type definities voor de muziek-database voor *arrays* (zelf omzetten naar vector, zie opdracht 1)
*   - ordeningsrelaties op Track;
*   - inlezen van Nummers.txt bestand;
*   - functies en datastructuren uit hoorcollege #9 IPC031:
*   - sorteer-algoritmen insertion_sort, selection_sort, bubble_sort
*     (gebaseerd op *arrays*, zelf omzetten naar vector)
*   - main die Nummers.txt inleest en een sorteer-algoritme aanroept.
**********************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <sstream>
#include <functional>
#include <thread>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

/*********************************************************************
*   MuziekDB type definities en globale array liedjes:
**********************************************************************/
struct Length
{
    int minutes;							// #minuten  (0..)
    int seconds;							// #seconden (0..59)
};
struct Track
{
    string artist;                          // naam van uitvoerende artiest
    string cd;                              // titel van cd
    int    year;                            // jaar van uitgave
    int    track;							// nummer van track
    string title;                           // titel van track
    string tags;                            // tags van track
    Length time;							// lengte van track
    string country;                         // land van artiest
};

// region Length Operators

bool operator<(const Length& a, const Length& b)
{
    return a.minutes == b.minutes ? a.seconds < b.seconds : a.minutes < b.minutes;
}

bool operator==(const Length& a, const Length& b)
{
    return a.minutes == b.minutes && a.seconds == b.seconds;
}

//	afgeleide ordeningen op Length waarden:
bool operator>(const Length& a, const Length& b)
{
    return b < a;
}

bool operator<=(const Length& a, const Length& b)
{
    return !(b < a);
}

bool operator>=(const Length& a, const Length& b)
{
    return b <= a;
}

// endregion

/************************************************************************
*   Ordenings-relaties op Track:
*      definieer zelf < en == voor Length en Track, dan volgen
*      de overige ordeningen (>, <=, >=, >) daar vanzelf uit.
************************************************************************/
bool operator<(const Track& a, const Track& b)
{
#ifdef PROFILE
    ++num_operations;
#endif

#ifdef TRACK_LENGTH_COMPARISON
    if (a.time == b.time) {
        if (a.artist == b.artist) {
            if (a.title == b.title) {
                return a.cd < b.cd;
            }
            else return a.title < b.title;
        }
        else return a.artist < b.artist;
    }
    else return a.time < b.time;
#else
    if (a.artist == b.artist) {
        if (a.cd == b.cd) {
            if (a.year == b.year) {
                return a.track < b.track;
            }
            else return a.year < b.year;
        }
        else return a.cd < b.cd;
    }
    else return a.artist < b.artist;
#endif
}

bool operator==(const Track& a, const Track& b)
{
#ifdef PROFILE
    ++num_operations;
#endif

#ifdef TRACK_LENGTH_COMPARISON
    return a.time == b.time && a.artist == b.artist && a.title == b.title && a.cd == b.cd;
#else
    return a.artist == b.artist && a.cd == b.cd && a.year == b.year && a.track == b.track;
#endif
}

//	afgeleide ordeningen op Track waarden:
bool operator>(const Track& a, const Track& b)
{
    return b < a;
}

bool operator<=(const Track& a, const Track& b)
{
    return !(b < a);
}

bool operator>=(const Track& a, const Track& b)
{
    return b <= a;
}


/************************************************************************
*   Inlezen van Nummers.txt bestand:
************************************************************************/
istream& operator>> (istream& in, Length& lengte)
{// Preconditie:
    assert(true);
    /*  Postconditie:
    de waarde van lengte is ingelezen uit in: eerst minuten, daarna ':', daarna seconden.
    */
    char colon;
    in >> lengte.minutes >> colon >> lengte.seconds;
    return in;
}

ostream& operator<< (ostream& out, const Length lengte)
{
    out << lengte.minutes << ':';
    if (lengte.seconds < 10)
        out << '0';
    out << lengte.seconds;
    return out;
}

istream& operator>> (istream& infile, Track& track)
{
    string dummy;
    std::getline(infile, track.artist);       // artist
    std::getline(infile, track.cd);           // cd
    infile >> track.year;              // year
    std::getline(infile, dummy);            // verwijder t/m newline
    infile >> track.track;             // track nr
    std::getline(infile, dummy);            // verwijder t/m newline
    std::getline(infile, track.title);        // track title
    std::getline(infile, track.tags);         // track tags
    infile >> track.time;              // track time
    std::getline(infile, dummy);            // verwijder t/m newline
    std::getline(infile, track.country);      // country
    std::getline(infile, dummy);            // verwijder t/m newline
    return infile;
}

ostream& operator<< (ostream& out, const Track track)
{
    out << track.artist << " " << track.cd << " [" << track.track << "] (" << track.time << ")";
    return out;
}

int lees_liedjes(ifstream& infile, vector<Track>& tracks)
{
    int aantal_ingelezen_liedjes = 0;
    do {
        Track volgende;
        infile >> volgende;
        if (infile) {
            tracks.push_back(volgende);
            ++aantal_ingelezen_liedjes;
        }
    } while (infile);
    return aantal_ingelezen_liedjes;
}

int lees_bestand(string bestandnaam, vector<Track>& tracks)
{
    ifstream nummersDBS(bestandnaam.c_str());
    if (!nummersDBS)
    {
        cout << "Kon '" << bestandnaam << "' niet openen." << endl;
        return -1;
    }
    cout << "Lees '" << bestandnaam << "' in." << endl;
    int aantal = lees_liedjes(nummersDBS, tracks);
    nummersDBS.close();
    return aantal;
}

void toon_MuziekDB(const vector<Track>& tracks)
{
    int i = 0;
    stringstream ss;

    for (const auto& track : tracks) {
        ss << ++i << ". " << track << '\n';
    }

    cout << ss.rdbuf() << flush;
}

/************************************************************************
*   functies en datastructuren uit hoorcollege #9 IPC031:
************************************************************************/
struct Slice
{
    int from; 	// 0    <= from
    int to; 	// from <= to
};

Slice mkSlice(int from, int to)
{
    //	pre-condition:
    assert(0 <= from && from <= to);
    Slice s = { from, to };
    return s;
}

bool valid_slice(Slice s)
{
    return 0 <= s.from && s.from <= s.to;
}

// region Vector Sorting

bool is_sorted(vector<Track>& vector, Slice s)
{
    //	pre-condition:
    assert(valid_slice(s));	// ...and s.to < size of vector
    //	post-condition:
    //	result is true if	vector[s.from]   <= vector[s.from+1]
    //						vector[s.from+1] <= vector[s.from+2]
    //						...
    //						vector[s.to-1]   <= vector[s.to]
    bool sorted = true;
    for (int i = s.from; i < s.to && sorted; i++)
        if (vector[i] > vector[i + 1])
            sorted = false;
    return sorted;
}

int find_position(vector<Track>& vector, Slice s, const Track& y)
{
    //	pre-condition:
    assert(valid_slice(s) && is_sorted(vector, s));    // ...and s.to < size of vector
    //	post-condition: s.from <= result <= s.to+1
    for (int i = s.from; i <= s.to; i++)
        if (y <= vector[i])
            return i;
    return s.to + 1;
}

int find_position_bonus(vector<Track>& vector, Slice s, const Track& y)
{
    //	pre-condition:
    assert(valid_slice(s) && is_sorted(vector, s));    // ...and s.to < size of vector
    // 	post-condition: s.from <= result <= s.to+1
    for (int i = s.to; i >= s.from; --i)
        if (y > vector[i])
            return i + 1;

    return s.from;
}

void shift_right(vector<Track>& vector, Slice s)
{
    //	pre-condition:
    assert(valid_slice(s));	// ... and s.to < size (vector) - 1
    //	post-condition:  vector[s.from+1]	= 	old vector[s.from]
    //			    	 vector[s.from+2]	= 	old vector[s.from+1]
    //						...
    //			    	 vector[s.to+1]		= 	old vector[s.to]
    for (int i = s.to + 1; i > s.from; i--)
        vector[i] = vector[i - 1];
}

void insert(vector<Track>& vector, int& length, Track y)
{
    const int pos = find_position(vector, mkSlice(0, length - 1), y);
    if (pos < length)
        shift_right(vector, mkSlice(pos, length - 1));
    vector[pos] = y;
    length++;
}

void insert_bonus(vector<Track>& vector, int& length, Track y)
{
    const int pos = find_position_bonus(vector, mkSlice(0, length - 1), y);
    if (pos < length)
        shift_right(vector, mkSlice(pos, length - 1));
    vector[pos] = y;
    length++;
}

void swap(vector<Track>& vector, int  i, int  j)
{
    //	pre-condition:
    assert(i >= 0 && j >= 0);	// ... and i < size of vector
    // ... and j < size of vector
    // Post-condition: vector[i] = old vector[j] and vector[j] = old vector[i]
    const Track help = vector[i];
    vector[i] = vector[j];
    vector[j] = help;
}

//	vector-based insertion sort:
void insertion_sort(vector<Track>& vector)
{
    int sorted = 1;
    while (sorted < vector.size())
    {
        insert(vector, sorted, vector[sorted]);
    }
}

//	vector-based insertion sort:
void insertion_sort_bonus(vector<Track>& vector)
{
    int sorted = 1;
    while (sorted < vector.size())
    {
        insert_bonus(vector, sorted, vector[sorted]);
    }
}

//	vector-based selection sort:
int smallest_value_at(vector<Track>& vector, Slice s)
{
    //	pre-condition:
    assert(valid_slice(s));	// ... and s.to < size (s)
    //	Post-condition: s.from <= result <= s.to and vector[result] is
    //	the minimum value of vector[s.from] .. vector[s.to]
    int  smallest_at = s.from;
    for (int index = s.from + 1; index <= s.to; index++)
        if (vector[index] < vector[smallest_at])
            smallest_at = index;
    return smallest_at;
}

void selection_sort(vector<Track>& vector)
{
    for (int unsorted = 0; unsorted < vector.size(); unsorted++)
    {
        const int k = smallest_value_at(vector, mkSlice(unsorted, static_cast<int>(vector.size()) - 1));
        swap(vector, unsorted, k);
    }
}

//	vector-based bubble sort:
bool bubble(vector<Track>& vector, Slice s)
{
    //	pre-condition:
    assert(valid_slice(s));	// ... and s.to < size(vector)
    //	Post-condition:
    //	maximum of vector[s.from]..vector[s.to] is at vector[s.to]
    //	if result is true then sorted( vector, s )
    bool is_sorted = true;
    for (int i = s.from; i < s.to; i++)
        if (vector[i] > vector[i + 1])
        {
            swap(vector, i, i + 1);
            is_sorted = false;
        }
    return is_sorted;
}

void bubble_sort(vector<Track>& vector)
{
    int length = static_cast<int>(vector.size());

    while (!bubble(vector, mkSlice(0, length - 1)))
        length--;
}

// endregion

// region Visualization

#ifdef PROFILE
const size_t INITIAL_LENGTH = 99;
const size_t INCREMENTS = 100;
const char FULL_MEASURE = '*';
const char REMAINDER_MEASURE = '.';
const int MEASURE = 100000;

void write_n(ofstream& os, const char c, int n) {
    // pre:
    assert(n >= 0);
    // post:
    // the character n has been written to the file os n times.

    for (int i = 0; i < n; ++i) {
        os << c;
    }
}

void visualize_slice(vector<Track>& tracks, ofstream& os, const function<void(vector<Track>&)>& algorithm) {
    // pre:
    assert(true);
    // post:
    // The slice tracks has been visually profiled for the specified algorithm, and the results written to os.

    num_operations = 0;
    algorithm(tracks);
    write_n(os, FULL_MEASURE, num_operations / MEASURE);
    if (num_operations % MEASURE != 0) os << REMAINDER_MEASURE;
    os << '\n';
}

void visualize_algorithm(const vector<Track>& tracks, ofstream& os, const function<void(vector<Track>&)>& algorithm) {
    // pre:
    assert(true);
    // post:
    // The algorithm has been visually profiled, and the results written to os.

    for (size_t length = INITIAL_LENGTH; length <= tracks.size(); length += INCREMENTS) {
        vector<Track> slice(tracks.cbegin(), tracks.cbegin() + length);

        visualize_slice(slice, os, algorithm);
    }
}

void visualize(const vector<Track>& tracks) {
    // pre:
    assert(true);
    // post:
    // All sorting algorithms have been visually profiled, and the results written to the respective output files.

#ifdef TRACK_LENGTH_COMPARISON
    ofstream os_insert("../visualization-insertion-track-length.txt");
    ofstream os_select("../visualization-selection-track-length.txt");
    ofstream os_bubble("../visualization-bubble-track-length.txt");
    ofstream os_bonus("../visualization-bonus-track-length.txt");
#else
    ofstream os_insert("../visualization-insertion.txt");
    ofstream os_select("../visualization-selection.txt");
    ofstream os_bubble("../visualization-bubble.txt");
    ofstream os_bonus("../visualization-bonus.txt");
#endif

    cout << "Visualizing sorting algorithms" << endl;
    cout << "Spawning threads..." << endl;

    thread thr_insert(visualize_algorithm, tracks, ref(os_insert), insertion_sort);
    thread thr_select(visualize_algorithm, tracks, ref(os_select), selection_sort);
    thread thr_bubble(visualize_algorithm, tracks, ref(os_bubble), bubble_sort);
    thread thr_bonus(visualize_algorithm, tracks, ref(os_bonus), insertion_sort_bonus);

    cout << "Waiting for threads to finish..." << endl;

    thr_insert.join();
    thr_select.join();
    thr_bubble.join();
    thr_bonus.join();

    cout << "Completed" << endl;
}
#endif

// endregion

/************************************************************************
*   de hoofdstructuur van het programma:
*       - lees bestand in
*       - sorteer bestand met insertion_sort / selection_sort / bubble_sort
*                          en 'normale' volgorde van Tracks
*                          en oplopend naar track-lengte
************************************************************************/
enum Sorteermethode { Insertion, Selection, Bubble, InsertionBonus, AantalMethoden };
string sorteermethode[] = { "insertion", "selection", "bubble","insertion (bonus)" };

Sorteermethode selecteer()
{
    cout << "Kies een sorteermethode: " << endl;
    for (int m = 0; m < AantalMethoden; m++)
        cout << m + 1 << ": " << sorteermethode[m] << " sort" << endl;
    int keuze;
    cin >> keuze;
    return static_cast<Sorteermethode>(keuze - 1);
}

int main()
{
    vector<Track> tracks;
    int aantalLiedjes = lees_bestand("../Nummers.txt", tracks);
    if (aantalLiedjes < 0)
    {
        cout << "Bestand inlezen mislukt. Programma termineert." << endl;
        return aantalLiedjes;
    }

#ifdef VISUAL_PROFILE
    visualize(tracks);
#else
    #ifdef PROFILE
    num_operations = 0;
#endif
    Sorteermethode m = selecteer();
    cout << "Sorteren bestand met " << sorteermethode[m] << " sort" << endl;
    switch (m)
    {
    case Insertion: insertion_sort(tracks); break;
    case Selection: selection_sort(tracks); break;
    case Bubble:    bubble_sort(tracks); break;
    case InsertionBonus: insertion_sort_bonus(tracks); break;
    default:        cout << "Huh?" << endl;
    }

#ifdef PROFILE
    cout << num_operations << endl;
#else
    cout << "Bestand gesorteerd." << endl;
    toon_MuziekDB(tracks);
#endif
#endif

    return EXIT_SUCCESS;
}

/*
* Part 4:
*
* Without TRACK_LENGTH_COMPARISON
* Sorteren bestand met insertion sort: 15797254
* Sorteren bestand met selection sort: 15806253
* Sorteren bestand met bubble sort: 402228
* Sorteren bestand met insertion (bonus) sort: 17256
*
* With TRACK_LENGTH_COMPARISON
* Sorteren bestand met insertion sort: 7866888
* Sorteren bestand met selection sort: 15806253
* Sorteren bestand met bubble sort: 15800582
* Sorteren bestand met insertion (bonus) sort: 7950582
*
* The big difference for insertion sort is because it has a best case performance of O(n), and a worst case performance
* of O(n*n). The best case happens when the data is already sorted, but in reverse order (data is decreasing). The
* worst case happens when the data is already sorted (data is increasing).
*
* The big difference for bubble sort is because it has a best case performance of O(n), and a worst case performance of
* O(n*n). The best case happens when the data is already sorted (data is increasing). The worst case happens when the
* data is already sorted, but in reverse order (data is decreasing).
*
* It is clear the bonus version of insertion sort compares much better without TRACK_LENGTH_COMPARISON. This is because
* the main problem with the normal insertion sort is avoided (increasing data), by checking the end of the sorted data
* first.
*/

/*
* Part 5:
*
* Insertion sort displays largely the same behaviour, but performs considerably worse without TRACK_LENGTH_COMPARISON.
*
* Again there is a clear difference under bubble sort, as the number of operations required clearly grows exponentially
* with TRACK_LENGTH_COMPARISON, while it seems to grow linearly without it.
*
* It is clear the bonus version of the insertion sort is very efficient without TRACK_LENGTH_COMPARISON, for any slice
* size. While the number of operations seems to grow linearly, it still remains very small.
*/