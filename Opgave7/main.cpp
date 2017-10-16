// Suppress deprecation errors on strerror when building on MSVC
#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

// Use BONUS macro to build the bonus version.
#define BONUS
bool read_word_bonus(ifstream&, string&);

// region Mandatory

enum Command { Enter, Content, Stop, Count, Where, Context, NumCommands };
const string COMMAND_TEXT[] = { "enter", "content", "stop", "count", "where", "context" };

const int MAX_WORDS = 30000;
const int MAX_WORDS_SEQUENCE = 100;
const int ERROR_OVERFLOW = -1;

static string content[MAX_WORDS];
static int content_size = 0;
static string word_sequence[MAX_WORDS_SEQUENCE];
static int word_sequence_size = 0;

/// Splits a string around a delimiter.
/// \param str - The string to split.
/// \param delim - The delimiter to split the string around.
/// \param lhs - The string to receive everything to the left of the delimiter.
/// \param rhs - The string to receive everything to the right of the delimiter.
void split_on(const string& str, char delim, string& lhs, string& rhs) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // lhs contains the string left of the delimiter and rhs contains the string right of the delimiter.

    size_t index = str.find(delim);

    if(index == string::npos) {
        lhs = str;
        rhs = "";
    } else {
        lhs = str.substr(0, index);
        rhs = index + 1 == str.length() ? "" : str.substr(index + 1);
    }
}

/// Splits a string based on a delimiter.
/// \param str - The string to split.
/// \param delim - The delimiter to split on.
/// \param dest - The array to receive the split strings.
/// \param dest_size - The size of the \a dest array.
/// \return The number of strings in the \a dest array, or ERROR_OVERFLOW in case the \a dest array isn't big enough.
int split_into(const string& str, char delim, string dest[], int dest_size) {
    // pre-conditions:
    assert(dest != nullptr);
    assert(dest_size > 0);
    // post-conditions:
    // dest contains the split strings, and return value is the number of strings in dest, unless dest was too small in
    // which case the return value is ERROR_OVERFLOW.

    int size = 0;
    size_t pos = 0;
    size_t index;

    while((index = str.find(delim, pos)) != string::npos) {
        string segment = str.substr(pos, index - pos);

        if(!segment.empty()) {
            // Prevent array overflow.
            if(size == dest_size) {
                return ERROR_OVERFLOW;
            }

            dest[size++] = segment;
        }

        pos = index + 1;
    }

    if(pos < str.length()) {
        string final_segment = str.substr(pos);

        // Prevent array overflow.
        if(size == dest_size) {
            return ERROR_OVERFLOW;
        }

        dest[size++] = final_segment;
    }

    return size;
}

/// Attempts to parse the user command in \a input.
/// \param input - The user command string to parse.
/// \param command - The resulting command.
/// \param args - The resulting argument string.
/// \return True if the parsing was successful, false otherwise.
bool parse_command(const string &input, Command &command, string &args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // If the parsing was successful, return value is true and command/args contain the parsed command and argument
    // string. If the parsing failed the return value is false.

    string command_text;
    bool valid = false;

    split_on(input, ' ', command_text, args);

    for(int i = 0; i < NumCommands && !valid; ++i) {
        if(command_text == COMMAND_TEXT[i]) {
            command = static_cast<Command >(i);
            valid = true;
        }
    }

    if(!valid) {
        cerr << "Unknown command: " << command_text << endl;
     }

    return valid;
}

/// Prints all the available commands to the standard output.
void show_commands() {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // All commands are printed to the standard output.

    cout << "Commands:";

    for(int i = 0; i < NumCommands; ++i) {
        cout << ' ' << COMMAND_TEXT[i];
    }

    cout << endl;
}

/// Gets a command from the user.
/// \param command - The resulting command.
/// \param args - The resulting argument string.
void get_command(Command &command, string &args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // command contains the user command, and args the argument string.

    string input;

    do {
        cout << ">";
        getline(cin, input);
    } while (!parse_command(input, command, args));
}

/// Attempts to read a word from the stream.
/// \param stream - The stream to read from.
/// \param word - The resulting word.
/// \return True if the word was read successfully, false otherwise.
bool read_word(ifstream& stream, string& word) {
    // pre-conditions:
    assert(stream.is_open());
    // post-conditions:
    // If a word was read successfully, word contains the word and the return value is true. If a word could not be read
    // the return value is false.

    stream >> word;

    return !stream.fail();
}

/// Command handler for the enter command.
/// \param args - The argument string that contains a filename.
void cmd_enter(const string& args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The enter command has been executed. If it failed, the user is informed. If the command did complete successfully
    // then the words read from the file are stored in memory.

    ifstream stream(args);
    string word;

    if(!stream) {
        cout << "Could not open input file: " << strerror(errno) << endl;
    } else {
        content_size = 0;

#ifdef BONUS
        while(read_word_bonus(stream, word)) {
#else
        while(read_word(stream, word)) {
#endif
            if(content_size == MAX_WORDS) {
                cerr << "Failed to read file: word count cannot exceed " << MAX_WORDS << endl;
                content_size = 0;
                return;
            }

            content[content_size++] = word;
        }

        cout << "Read " << content_size << " words" << endl;
    }
}

/// Command handler for the content command.
void cmd_content() {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The words stored in memory are displayed in the standard output.

    cout << "Currently storing " << content_size << " words:";

    for(int i = 0; i < content_size; ++i) {
        cout << ' ' << content[i];
    }

    cout << endl;
}

/// Attempts to parse the argument string as a space separated word sequence and stores the result in the static word
/// sequence variables. The array \a word_sequence will contain the sequence, while \a word_sequence_size will contain
/// the number of entries in \a word_sequence. If the parsed word sequence is longer than \a MAX_WORDS_SEQUENCE, an
/// error is displayed and the parsing fails.
/// \param args - The argument string to parse.
/// \return True if the parsing was successful, and the sequence contains at least one word, false otherwise.
bool parse_word_sequence(const string& args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // If the parsing was successful, word_sequence contains the parsed sequence and word_sequence_size the sequence
    // length. If the parsing failed or the sequence was empty, an error message was displayed and false is returned.

    word_sequence_size = split_into(args, ' ', word_sequence, MAX_WORDS_SEQUENCE);

    if(word_sequence_size == ERROR_OVERFLOW) {
        cerr << "Failed to parse word sequence: length cannot exceed " << MAX_WORDS_SEQUENCE << " words" << endl;
    } else if(word_sequence_size == 0) {
        cerr << "Please enter a word sequence (space separated)" << endl;
    }

    return word_sequence_size != 0 && word_sequence_size != ERROR_OVERFLOW;
}

/// Determines if the currently loaded words in \a content have the word sequence stored in \a word_sequence, starting
/// from \a start, at \a index.
/// \param index - The index in the \a content array to begin comparison at.
/// \param start - The index in the \a word_sequence array to begin comparison at.
/// \return True if the sequences match, false otherwise.
bool has_word_sequence_at(int index, int start = 0) {
    // pre-conditions:
    assert(index >= 0 && index + word_sequence_size - start <= content_size);
    // post-conditions:
    // return value is true if the sequences match, false otherwise.

    for(int i = start; i < word_sequence_size; ++i) {
        if(word_sequence[i] != content[index + i - start]) {
            return false;
        }
    }

    return true;
}

/// Command handler for the count command.
/// \param args - The argument string that contains a word sequence.
void cmd_count(const string& args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The number of occurrences of the word sequence are displayed.

    if(content_size == 0) {
        cerr << "Please load a file first via " << COMMAND_TEXT[Enter] << endl;
    } else if(parse_word_sequence(args)) {
        int count = 0;

        for (int i = 0; i < content_size - word_sequence_size + 1; ++i) {
            if (has_word_sequence_at(i)) {
                ++count;
            }
        }

        double percentage = (static_cast<double>(count) / content_size) * 100.0;
        cout << "count: " << count << "\ttotal: " << content_size << "\tpercentage: " << percentage << endl;
    }
}

/// Command handler for the where command.
/// \param args - The argument string that contains a word sequence.
void cmd_where(const string& args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The number of occurrences and the indexes of the word sequence are displayed.

    if(content_size == 0) {
        cerr << "Please load a file first via " << COMMAND_TEXT[Enter] << endl;
    } else if(parse_word_sequence(args)) {
        int count = 0;

        for (int i = 0; i < content_size - word_sequence_size + 1; ++i) {
            if (has_word_sequence_at(i)) {
                ++count;

                cout << "Found at index " << i + 1 << endl;
            }
        }

        double percentage = (static_cast<double>(count) / content_size) * 100.0;
        cout << "\ncount: " << count << "\ttotal: " << content_size << "\tpercentage: " << percentage << endl;
    }
}

/// Attempts to parse a base-10 integer.
/// \param str - The string to parse.
/// \param result - The parsed result.
/// \param min_value - The minimum value the parsed value must have.
/// \return True if the parsing was successful, false otherwise.
bool parse_int(const string& str, int& result, int min_value = 0) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // If parsing was successful the return value is true and result contains the parsed result. If parsing failed, or
    // the parsed value was below the min value, the return value is false.

    size_t index;

    try {
        result = stoi(str, &index);
    } catch (...) {
        return false;
    }

    return index == str.length() && result >= min_value;
}

/// Displays the current word sequence found at \a index, along with the preceding and following \a m words.
/// \param index - The index of the found word sequence.
/// \param m - The number of preceding and following words to display.
void display_context(int index, int m) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The word sequence starting at index is displayed, along with the preceding and following m words.

    int start = index - m;
    int end = index + word_sequence_size + m - 1;

    start = max(0, start);
    end = min(content_size, end);

    cout << "Found at index " << index + 1 << ':';

    for(int i = start; i < end; ++i) {
        cout << ' ';
        if(i == index) cout << '|';
        cout << content[i];
        if(i == index + word_sequence_size - 2) cout << '|';
    }

    cout << endl;
}

/// Command handler for the context command.
/// \param args - The argument string that contains a non negative integer and a word sequence.
void cmd_context(const string& args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The number of occurrences and the context of each matching word sequence is displayed.

    if(content_size == 0) {
        cerr << "Please load a file first via " << COMMAND_TEXT[Enter] << endl;
    } else if(parse_word_sequence(args)) {
        int m;

        if(word_sequence_size < 2) {
            cerr << "Word sequence must be at least 1 word in length" << endl;
        } else if(!parse_int(word_sequence[0], m)) {
            cerr << word_sequence[0] << " is not a valid non negative integer" << endl;
        } else {
            int count = 0;

            for (int i = 0; i < content_size - word_sequence_size + 1; ++i) {
                if (has_word_sequence_at(i, 1)) {
                    ++count;

                    display_context(i, m);
                }
            }

            double percentage = (static_cast<double>(count) / content_size) * 100.0;
            cout << "\ncount: " << count << "\ttotal: " << content_size << "\tpercentage: " << percentage << endl;
        }
    }
}

/// Processes the user command.
/// \param command - The user command to process.
/// \param args - The argument string of the command.
/// \return True if the application should keep running, false if it should stop.
bool process_command(Command command, const string &args) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // The user command has been processed. If the application should continue running the return value is true. If the
    // application should stop, the return value is false.

    switch (command) {
        case Enter:
            cmd_enter(args);
            return true;

        case Content:
            cmd_content();
            return true;

        case Count:
            cmd_count(args);
            return true;

        case Where:
            cmd_where(args);
            return true;

        case Context:
            cmd_context(args);
            return true;

        case Stop:
        default:
            return false;
    }
}

// endregion

// region Bonus

/// Checks if a given character is a letter character.
/// \param c - The character to check.
/// \return True if the character is either a-z, A-Z or ', false otherwise.
bool is_letter_character(char c) {
    // pre-conditions:
    assert(true);
    // post-conditions
    // return value is true if c is a letter character, false otherwise.

    return isalpha(c) || c == '\'';
}

/// Finds the start of a word for a given string.
/// \param input - The input string that contains the word.
/// \return The index in \a input the word starts at, or \a input.length() if it contains no valid word.
size_t find_start(const string& input) {
    // pre-conditions:
    assert(true);
    // post-conditions:
    // return value is the index of the start of the word, or input.length() if it contains no valid word.

    size_t index = 0;

    for(char c : input) {
        if(is_letter_character(c)) {
            return index;
        }

        ++index;
    }

    return index;
}

/// Finds the end of a word for a given string, starting at \a start.
/// \param input - The input string that contains the word.
/// \param start - The index of the start of the word.
/// \param parse_remainder - Set to true if the remainder of the string potentially contains another word, and should be
/// parsed again.
/// \return The index of the first element not part of the word.
size_t find_end(const string& input, size_t start, bool& parse_remainder) {
    // pre-conditions:
    assert(start < input.length());
    // post-conditions:
    // return value is the index of the first element not part of the word. parse_remainder is true of the remainder of
    // the input string potentially contains another word and should be parsed again.

    int hyphen_count = 0;
    parse_remainder = false;

    for(size_t index = start; index < input.length(); ++index) {
        char c = input[index];

        if(c == '-') {
            ++hyphen_count;

            // x-- pattern.
            if(hyphen_count == 2) {
                parse_remainder = true;

                return index - hyphen_count + 1;
            }
        } else if(is_letter_character(c)) {
            hyphen_count = 0;
        } else {
            // Only return x in case of a single dangling hyphen (x- pattern).
            return index - hyphen_count;
        }
    }

    // Only return x in case of a single dangling hyphen (x- pattern).
    return input.length() - hyphen_count;
}

/// Attempts to read a well formed word from the stream.
/// \param stream - The stream to read from.
/// \param word - The fwell formed word that was read.
/// \return True if a word was read successfully, false otherwise.
bool read_word_bonus(ifstream& stream, string& word) {
    // pre-conditions:
    assert(stream.is_open());
    // post-conditions:
    // return value is true if a word was read successfully, and is stored in word. If no word could be read the return
    // value is false.

    bool has_read_word = false;
    string input;

    while(!has_read_word && stream >> input) {
        bool parse_remainder = false;
        size_t start = find_start(input);
        size_t end = find_end(input, start, parse_remainder);

        if (start != input.length()) {
            word = input.substr(start, end - start);
            has_read_word = true;

            // In case a word was partially read, set the stream position to the start of the remaining characters.
            if(parse_remainder) {
                stream.clear();
                stream.seekg(end - input.length(), ifstream::cur);
            }
        }
    }

    return has_read_word;
}

// endregion

/// Application entry point and driver.
/// \return The application exit code.
int main() {
    string args;
    Command command;

    do {
        show_commands();
        get_command(command, args);
    } while(process_command(command, args));

    return EXIT_SUCCESS;
}