#include <iostream>
#include <fstream>          // for file I/O
#include <cassert>          // for assertion checking
#include <iomanip>
#include <sstream>
#include <cstring>
#include <chrono>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

//region Mandatory

enum Action {Encrypt, Decrypt} ;

int seed = 0 ;
void initialise_pseudo_random (int r)
{
//  pre-condition:
    assert (r > 0 && r <= 65536) ;
/*  post-condition:
    seed has value r.
*/
    seed = r ;
}

int next_pseudo_random_number ()
{
//  pre-condition:
    assert (seed > 0 && seed <= 65536) ;
/*  post-condition:
    result value > 0 and result value <= 65536 and result value != seed at entry of function
*/
    const int seed75 = seed * 75 ;
    int next = (seed75 & 65535) - (seed75 >> 16) ;
    if (next < 0)
        next += 65537 ;
    seed = next ;
    return next ;
}

/// Performs the rotate algorithm on the specified character \a a.
/// \param a - The character to perform the action on, must be in range [0, 127].
/// \param r - The rotation amount, cannot be negative.
/// \param action - The action to perform.
/// \return The rotated character, in the range [0, 127].
char rotate_char (char a, int r, Action action)
{
    // pre-conditions:
    assert(a >= 0 && a < 128);
    assert(r >= 0);
    // post-conditions:
    // result value >= 0 and result value < 128

    if(a < 32) {
        return a;
    } else if(action == Encrypt) {
        return static_cast<char>((a-32 + (r % (128-32)) + (128-32)) % (128-32)+32);
    } else { /*if(action == Decrypt) {*/
        return static_cast<char>((a-32 - (r % (128-32)) + (128-32)) % (128-32)+32);
    }
}

/// Tests if the result of the rotate character algorithm is valid.
/// \param a - The input character, must be in range [0, 127].
/// \param b - The encrypted \a a character, must be in range [0, 127].
/// \param a_2 - The decrypted \a b character, must be in range [0, 127].
/// \param r - The rotation amount, cannot be negative.
/// \return True if the algorithm result is valid, false otherwise.
bool test_rotated_char(char a, char b, char a_2, int r) {
    // pre-conditions:
    assert(a >= 0 && a < 128);
    assert(b >= 0 && b < 128);
    assert(a_2 >= 0 && a_2 < 128);
    assert(r >= 0);
    // post-conditions:
    // result is true if a,b,a_2 is an acceptable character rotation for r

    return a == a_2 && ((r % (128-32) == 0 || a < 32) ? a == b : a != b);
}

/// Tests the rotate character algorithm for a specified rotation \a r.
/// \param r - The rotation amount, cannot be negative.
/// \param verbose - Print verbose output.
void test_rotate_char_for(int r, bool verbose = true) {
    // pre-conditions:
    assert (r >= 0) ;
    // post-conditions:
    // failed_tests == 0

    int failed_tests = 0;

    cout << "Testing rotate char for r=" << r << endl;

    for(int i = 0; i < 128; ++i) {
        char a = static_cast<char>(i);
        char b = rotate_char(a, r, Encrypt);
        char a_2 = rotate_char(b, r, Decrypt);
        bool passed = test_rotated_char(a, b, a_2, r);

        // Only display verbose information if enabled, because the sheer amount of console I/O is a massive performance
        // bottleneck when doing bulk testing (and quite frankly it's not really relevant as long as the tests pass).
        if(verbose) {
            cout << setw(3) << static_cast<int>(a) << " = " << (a < 32 ? ' ' : a) << "\t\t"
                 << setw(3) << static_cast<int>(b) << " = " << (b < 32 ? ' ' : b) << "\t\t"
                 << "test: " << (passed ? "pass" : "fail") << endl;
        }

        if(!passed) {
            ++failed_tests;
        }
    }

    assert(failed_tests == 0);
}

/// Driver for testing the character rotation algorithm.
/// \param automate_testing - Perform automated testing instead of manual testing.
void test_rotate_char (bool automate_testing = false)
{
    // pre-conditions:
    assert ( true ) ;
    // post-conditions:
    // user has finished testing by entering a negative r.

    if(automate_testing) {
        for (int i = 0; i < 65536; ++i) {
            test_rotate_char_for(i, false);
        }
    } else {
        int r = 0;

        do {
            cout << "Enter r (negative to quit): ";
            cin >> r;

            if (r >= 0) {
                test_rotate_char_for(r, true);
                cout << endl;
            }
        } while (r >= 0);
    }
}

/// Opens a user specified input and output file.
/// \param infile - The input file, cannot be currently open.
/// \param outfile - The output file, cannot be currently open.
/// \return True if both files were opened successfully, false otherwise.
bool open_input_and_output_file (ifstream& infile, ofstream& outfile)
{
    // pre-conditions:
    assert(!infile.is_open());
    assert(!outfile.is_open());
    // post-conditions:
    // result is true if the infile and outfile are open, false otherwise

    string infile_name;
    string outfile_name;
    bool result = false;

    cout << "Input file name: ";
    cin >> infile_name;
    cout << "Output file name: ";
    cin >> outfile_name;

    if(infile_name == outfile_name) {
        cerr << "Input file name cannot be the same as the output file name" << endl;
    } else {
        infile.open(infile_name, ifstream::in);
        outfile.open(outfile_name, ifstream::out);

        if(!infile) {
            cerr << "Failed to open input file: " << strerror(errno) << endl;
        } else if(!outfile) {
            cerr << "Failed to open output file: " << strerror(errno) << endl;
        } else {
            cout << "Successfully opened input and output files" << endl;
            result = true;
        }
    }

    return result;
}

Action get_user_action ()
{// Pre-condition:
    assert ( true ) ;
/*  Post-condition:
    result is the Action that the user has indicated that the program should encrypt / decrypt
*/
    cout << "Do you want to encrypt the file? (y/n): " ;
    string answer ;
    cin  >> answer ;
    if (answer == "y")
        return Encrypt;
    else
        return Decrypt;
}

int initial_encryption_value ()
{// Pre-conditie:
    assert (true) ;
/*  Post-condition:
    result is a value between 0 and 65355 (both inclusive)
*/
    int initial_value = -1 ;
    while (initial_value < 0 || initial_value > 65535)
    {
        cout << "Please enter the initial coding value (greater or equal to 0 and less than 65536)" << endl ;
        cin  >> initial_value ;
    }
    return initial_value ;
}

/// Process the content of the input file and write it to the output file, using the provided \a action and \a initial_value.
/// \param infile - The input file.
/// \param outfile - The output file.
/// \param action - The action to perform.
/// \param initial_value - The secret key used to perform the action, must be in range (0, 65536).
void use_OTP (ifstream& infile, ofstream& outfile, Action action, int initial_value)
{
    // pre-conditions:
    assert(infile.is_open());
    assert(outfile.is_open());
    assert(initial_value > 0 && initial_value < 65536);
    // post-conditions:
    // outfile contains the processed content of infile, using the specified action and initial_value

    char a;
    initialise_pseudo_random(initial_value);

    while(infile.get(a)) {
        if(a == '\r')  {
            // The assignment is designed for Windows, which implicitly eats the CR characters, so eat them explicitly
            // for *nix compatibility. Also write them to the output stream to produce identical results, which allows
            // for easy comparison via file checksums.
            outfile << '\r';
        } else {
            int r = next_pseudo_random_number();
            char b = rotate_char(a, r, action);

            outfile << b;
        }
    }
}

/// Driver for the mandatory part of the assignment.
/// \return The application's exit code.
int mandatory_driver() {
    const Action ACTION = get_user_action() ;
    ifstream input_file  ;
    ofstream output_file ;
    if (!open_input_and_output_file (input_file,output_file))
    {
        cout << "Program aborted." << endl ;
        return -1 ;
    }
    const int INITIAL_VALUE = initial_encryption_value () ;
    use_OTP (input_file,output_file,ACTION,INITIAL_VALUE);
    input_file.clear () ;
    output_file.clear () ;
    input_file.close () ;
    output_file.close () ;

    if (!input_file || !output_file)
    {
        cout << "Not all files were closed successfully. The output might be incorrect." << endl ;
        return -1 ;
    }
    return 0 ;
}

//endregion

//region Bonus

/// Decrypt the input line to the output stream using the secret key \a initial_value.
/// \param line - The line containing the cipher-text.
/// \param out_stream - The output stream to receive the clear-text.
/// \param initial_value - The secret key used to decrypt, must be in range (0, 65536).
void decrypt(const string& line, ostream& out_stream, int initial_value) {
    // pre-conditions:
    assert(initial_value > 0 && initial_value < 65536);
    // post-conditions:
    // out_stream contains the decrypted content of line, followed by a newline character

    initialise_pseudo_random(initial_value);

    for(char a : line) {
        // Ignore CR on *nix platforms.
        if (a == '\r') continue;

        int r = next_pseudo_random_number();
        char b = rotate_char(a, r, Decrypt);

        out_stream << b;
    }

    out_stream << '\n';
}

/// Decrypt the input stream to the output stream using the secret key \a initial_value.
/// \param in_stream - The input stream containing the cipher-text.
/// \param out_stream - The output stream to receive the clear-text.
/// \param initial_value - The secret key used to decrypt, must be in range (0, 65536).
void decrypt_stream(istream& in_stream, ostream& out_stream, int initial_value) {
    // pre-conditions:
    assert(initial_value > 0 && initial_value < 65536);
    // post-conditions:
    // out_stream contains the decrypted content of in_stream

    initialise_pseudo_random(initial_value);
    char a;

    while(in_stream.get(a)) {
        if(a == '\r')  {
            // The assignment is designed for Windows, which implicitly eats the CR characters, so eat them explicitly
            // for *nix compatibility. Also write them to the output stream to produce identical results, which allows
            // for easy comparison via file checksums.
            out_stream << '\r';
        } else {
            int r = next_pseudo_random_number();
            char b = rotate_char(a, r, Decrypt);

            out_stream << b;
        }
    }
}

/// Brute force the secret key of secret.txt by decrypting the first line into brute_force.txt for each possible key.
/// The line number corresponds to the secret key used.
void brute_force_secret() {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // If no I/O error occurs, brute_force.txt contains the brute forced first line of secret.txt for each possible key

    ifstream in_file("../secret.txt", ifstream::in);
    ofstream out_file("../brute_force.txt", ofstream::out);
    stringstream ss_out;
    string line;

    if(!in_file) {
        cerr << "Failed to open input file: " << strerror(errno) << endl;
    } else if(!out_file) {
        cerr << "Failed to open output file: " << strerror(errno) << endl;
    } else {
        getline(in_file, line);

        // Iterate over the key space and decrypt the first line using each possible key.
        for (int r = 1; r < 65536; ++r) {
            decrypt(line, ss_out, r);
        }

        // Write the output to disk, the reason this is buffered is to avoid a large amount of small I/O calls.
        out_file << ss_out.rdbuf();
        out_file.close();

        if(!out_file) {
            cerr << "Failed to close output file: " << strerror(errno) << endl;
        }
    }
}

/// Decrypt the secret.txt file to the source.txt file using the secret key \a initial_value.
/// \param initial_value - The secret key used to decrypt, must be in range (0, 65536).
void decrypt_secret(int initial_value) {
    // pre-conditions:
    assert(initial_value > 0 && initial_value < 65536);
    // post-conditions:
    // if no I/O error occurs, source.txt contains the decrypted content from secret.txt

    ifstream in_file("../secret.txt", ifstream::in);
    ofstream out_file("../source.txt", ofstream::out);
    stringstream ss_in;
    stringstream ss_out;

    if(!in_file) {
        cerr << "Failed to open input file: " << strerror(errno) << endl;
    } else if(!out_file) {
        cerr << "Failed to open output file: " << strerror(errno) << endl;
    } else {
        in_file >> ss_in.rdbuf();

        decrypt_stream(ss_in, ss_out, initial_value);

        // Write the output to disk, the reason this is buffered is to avoid a large amount of small I/O calls.
        out_file << ss_out.rdbuf();
        out_file.close();

        if(!out_file) {
            cerr << "Failed to close output file: " << strerror(errno) << endl;
        }
    }
}

/// Invokes a function and measures the execution duration.
/// \tparam T - The function type.
/// \tparam Args - The function argument types.
/// \param func - The function to time.
/// \param message - The message to display.
/// \param args - The invocation arguments.
template<class T, class... Args >
void time_function(T func, const string& message, Args&&... args) {
    auto start = chrono::high_resolution_clock ::now();

    func(std::forward<Args>(args)...);

    auto end = chrono::high_resolution_clock::now();
    auto dif = end - start;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dif);

    cout << message << ms.count() << "ms" << endl;
}

/// Invokes and times the functions used for the bonus assignment.
void bonus() {
    /*
     * The secret key was relatively easy to find. Just brute force the first line and do a little post processing to
     * look for something that resembles English, then eyeball the correct answer. EG: "cat brute_force.txt | grep -n THE"
     * This produces 2748:THE STORY OF BEOWULF as an answer, giving the secret key 2748.
     */
    const int SECRET_KEY = 2748;

    time_function(brute_force_secret, "Brute forced in: ");
    time_function(decrypt_secret, "Decrypted in: ", SECRET_KEY);
}

//endregion

/// Application entry point.
/// \return The application's exit code.
int main()
{
    //test_rotate_char();
    bonus();
    //return mandatory_driver();

    return EXIT_SUCCESS;
}
