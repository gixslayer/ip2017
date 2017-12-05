#include <iostream>
#include <cassert>
#include <cmath>
#include <string>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

// region Part 1

/// Raise \a x to the n-th power by repeated multiplication.
/// \tparam T - Type of \a x.
/// \param x - The value to raise to a power.
/// \param n - The power to raise to.
/// \return \a x raised to the n-th power.
template<typename T>
T power (T x, unsigned int n) {
    // pre:
    assert(true);
    // post:
    // return value is x^n.

    return n == 0 ? 1 : x * power(x, n-1);
}

/// Raise \a x to the n-th power by using the multiply and square algorithm.
/// \tparam T - Type of \a x.
/// \param x - The value to raise to a power.
/// \param n - The power to raise to.
/// \return \a x raised to the n-th power.
template<typename T>
T fast_power(T x, unsigned int n) {
    // pre:
    assert(true);
    // post:
    // return value is x^n.
    // run-time complexity: O(log n)

    if(n == 0) {
        return 1;
    } else if(n % 2 == 1) {
        return x * fast_power(x, n-1);
    } else {
        T value = fast_power(x, n/2);

        return value * value;
    }
}

// endregion

// region Part 2

/// Check if \a text from index \a i till \a j is a palindrome.
/// \param text - The string to check.
/// \param i - The start index to check from.
/// \param j - The end index to check till.
/// \return True if \a text is a palindrome from index \a till \a j, false otherwise.
bool palindrome1(const string& text, int i, int j) {
    // pre:
    assert(i >= 0 && i < text.size());
    assert(j >= 0 && j < text.size());
    // post:
    // return value is true if text is a palindrome, false otherwise.

    if(i >= j) {
        return true;
    } else {
        return text[i] == text[j] && palindrome1(text, ++i, --j);
    }
}

/// Check if \a text from index \a i till \a j is a case insensitive palindrome.
/// \param text - The string to check.
/// \param i - The start index to check from.
/// \param j - The end index to check till.
/// \return True if \a text is a palindrome from index \a till \a j, false otherwise.
bool palindrome2(const string& text, int i, int j) {
    // pre:
    assert(i >= 0 && i < text.size());
    assert(j >= 0 && j < text.size());
    // post:
    // return value is true if text is a case insensitive palindrome, false otherwise.

    if(i >= j) {
        return true;
    } else {
        return tolower(text[i]) == tolower(text[j]) && palindrome2(text, ++i, --j);
    }
}

/// Scan direction (left to right or right to left).
enum class Direction {forward, backward};

/// Scan \a text from \a index for the first non space/punctuation mark character.
/// \param text - The text to scan.
/// \param index - The index to begin scanning from.
/// \param direction - The direction to scan in.
/// \return The index of the character, or in case no character was found either -1 or \a text.size()
int scan(const string& text, int index, Direction direction) {
    // pre:
    assert(index >= 0 && index < text.size());
    // post:
    // return value is index of first non ignore character, or -1/text.size() if none exists.

    static string IGNORE = " .,:;\'!?-";

    if(IGNORE.find(text[index]) == string::npos) {
        return index;
    } else {
        int new_index = direction == Direction::forward ? ++index : --index;

        if(new_index < 0 || new_index == text.size()) {
            return new_index;
        } else {
            return scan(text, new_index, direction);
        }
    }
}

/// Check if \a text from index \a i till \a j is a case and space insensitive palindrome.
/// \param text - The string to check.
/// \param i - The start index to check from.
/// \param j - The end index to check till.
/// \return True if \a text is a palindrome from index \a till \a j, false otherwise.
bool palindrome3(const string& text, int i, int j) {
    // pre:
    assert(i >= 0 && i < text.size());
    assert(j >= 0 && j < text.size());
    // post:
    // return value is true if text is a case and space insensitive palindrome, false otherwise.

    if(i >= j) {
        return true;
    } else {
        int new_i = scan(text, i, Direction::forward);
        int new_j = scan(text, j, Direction::backward);

        if(new_i <= j && new_j >= i) {
            return tolower(text[new_i]) == tolower(text[new_j]) && palindrome3(text, ++new_i, --new_j);
        } else {
            return true;
        }
    }
}

// endregion

// region Part 3

/// Check if all characters in \a chars, starting from index \a i, occur in \a source, starting from index \a j, in
/// order (but possibly not adjacent).
/// \param chars - The characters to match.
/// \param i - The \a chars index to begin matching from.
/// \param source - The string to match against.
/// \param j - The \a source index to begin matching against.
/// \return True if all characters in \a chars occur in order in \a source, false otherwise.
bool match_chars(const string& chars, int i, const string& source, int j) {
    // pre:
    assert(i >= 0 && i < chars.size());
    assert(j >= 0 && j < source.size());
    // post:
    // return value is true if all characters in chars[i:] occur in source[j:] in order, but possibly not adjacent.

    if(chars[i] == source[j]) {
        return ++i == chars.size() ? true : match_chars(chars, i, source, ++j);
    } else {
        return ++j == source.size() ? false : match_chars(chars, i, source, j);
    }
}

// endregion

// region Bonus

/// Check if \a str starts with \a prefix.
/// \param str - The string to check on.
/// \param prefix - The prefix to check with.
/// \return True if \a str starts with \a prefix, false otherwise.
bool starts_with(const string& str, const string& prefix) {
    // pre:
    assert(true);
    // post:
    // return value is true if str starts with prefix, false otherwise.

    if(prefix.size() > str.size()) return false;

    return equal(prefix.cbegin(), prefix.cend(), str.cbegin());
}

/// Simplify \a pattern by repeatedly replacing ** with * and *. with .*
/// \param pattern - The pattern to simplify.
/// \return The simplified pattern.
string simplify(const string& pattern) {
    // pre:
    assert(true);
    // post:
    // return value is a simplified version of pattern by repeatedly replacing *. with .* and ** with *

    if(pattern.size() <= 1) return pattern;
    if(starts_with(pattern, "**")) return simplify(pattern.substr(1));
    if(starts_with(pattern, ".*")) return "." + simplify(pattern.substr(1));
    if(starts_with(pattern, "*.")) return "." + simplify("*" + pattern.substr(2));

    return pattern[0] + simplify(pattern.substr(1));
}

/// Check if \a pattern, starting at \a i, is a simplified pattern (no occurrences of ** or *.).
/// \param pattern - The pattern to check.
/// \param i - The pattern index to begin checking from.
/// \return True if the pattern is simplified, false otherwise.
bool is_simplified(const string& pattern, size_t i) {
    // pre:
    assert(i <= pattern.size());
    // post:
    // return value is true if pattern[i:] is a simplified pattern (** or *. does not occur).

    return pattern.find("**", i) == string::npos && pattern.find("*.") == string::npos;

    /* Recursive version
    if(pattern.size() - i < 2) return true;
    if(pattern[i] == '*' && pattern[i+1] == '*') return false;
    if(pattern[i] == '*' && pattern[i+1] == '.') return false;

    return is_simplified(pattern, ++i);
    */
}

bool match_pattern(const string&, size_t, const string&, size_t);

/// Check if \a pattern, starting at \a i, matches \a source, starting from any k, such that j &lt= k &lt \a source.size().
/// \param pattern - The simplified pattern to match.
/// \param i - The pattern index to begin checking from.
/// \param source - The source string to match against.
/// \param j - The lower bound of the source index k to begin checking from.
/// \return True if the pattern matches at any position, false otherwise.
bool match_pattern_anywhere(const string& pattern, size_t i, const string& source, size_t j) {
    // pre:
    assert(i < pattern.size());
    assert(is_simplified(pattern, i));
    // post:
    // return value is true if pattern[i:] matches source[k:], for any k such that j <= k < |source|.

    // Next character in pattern must be an exact character match, so consume source till that character occurs.
    size_t new_j = source.find(pattern[i], j);

    if(new_j == string::npos) return false; // If the character cannot be found, the pattern cannot possibly match.
    if(match_pattern(pattern, i, source, new_j)) return true;
    else return match_pattern_anywhere(pattern, i, source, ++new_j);
}

/// Check if \a pattern, starting at \a i, matches \a source, starting at \a j.
/// \param pattern - The simplified pattern to match.
/// \param i - The pattern index to begin checking from.
/// \param source - The source string to match against.
/// \param j - The source index to begin checking from.
/// \return True if the pattern matches, false otherwise.
bool match_pattern(const string& pattern, size_t i, const string& source, size_t j) {
    // pre:
    assert(is_simplified(pattern, i));
    // post:
    // return value is true if source[i:] matches pattern[j:], false otherwise.

    if(i == pattern.size() && j == source.size()) return true; // Pattern and source depleted.
    if(i >= pattern.size() || j >= source.size()) return false; // Pattern or source depleted, but not both.
    if(pattern[i] == '.') return match_pattern(pattern, ++i, source, ++j); // Single character wildcard.
    // Multi character wildcard. If pattern ends with * then always match, else try to find a match by consuming any
    // number of characters from source.
    if(pattern[i] == '*') return i + 1 == pattern.size() ? true : match_pattern_anywhere(pattern, ++i, source, j);
    else return pattern[i] == source[j] && match_pattern(pattern, ++i, source, ++j); // Exact character match.
}

/// Check if \a pattern matches \a source.
/// \param pattern - The pattern to match (does not have to be simplified).
/// \param source - The source string to match against.
/// \return True if the pattern matches, false otherwise.
bool match_pattern(const string& pattern, const string& source) {
    // pre:
    assert(true);
    // post:
    // return value is true if source matches pattern, false otherwise.

    return match_pattern(simplify(pattern), 0, source, 0);
}

// endregion

/// Application entry point.
/// \return Application exit code.
int main() {
    cout << "[Naive power]" << endl;
    cout << "2^0 = " << power(2, 0) << endl;
    cout << "2^1 = " << power(2, 1) << endl;
    cout << "2^2 = " << power(2, 2) << endl;
    cout << "2^3 = " << power(2, 3) << endl;
    cout << "2^4 = " << power(2, 4) << endl;
    cout << "3^3 = " << power(3, 3) << endl;
    cout << "sqrt(2)^2 = " << power(sqrt(2), 2) << endl;

    cout << "\n[Fast power]" << endl;
    cout << "2^0 = " << fast_power(2, 0) << endl;
    cout << "2^1 = " << fast_power(2, 1) << endl;
    cout << "2^2 = " << fast_power(2, 2) << endl;
    cout << "2^3 = " << fast_power(2, 3) << endl;
    cout << "2^4 = " << fast_power(2, 4) << endl;
    cout << "3^3 = " << fast_power(3, 3) << endl;
    cout << "sqrt(2)^2 = " << fast_power(sqrt(2), 2) << endl;

    cout << "\n[Palindrome1]" << endl;
    cout << "otto = " << palindrome1("otto", 0, 3) << endl;
    cout << "Otto = " << palindrome1("Otto", 0, 3) << endl;
    cout << "Madam, I'm Adam. = " << palindrome1("Madam, I'm Adam.", 0, 15) << endl;

    cout << "\n[Palindrome2]" << endl;
    cout << "otto = " << palindrome2("otto", 0, 3) << endl;
    cout << "Otto = " << palindrome2("Otto", 0, 3) << endl;
    cout << "Madam, I'm Adam. = " << palindrome2("Madam, I'm Adam.", 0, 15) << endl;

    cout << "\n[Palindrome3]" << endl;
    cout << "otto = " << palindrome3("otto", 0, 3) << endl;
    cout << "Otto = " << palindrome3("Otto", 0, 3) << endl;
    cout << "Madam, I'm Adam. = " << palindrome3("Madam, I'm Adam.", 0, 15) << endl;

    cout << "\n[Matching characters]" << endl;
    cout << "abc -> It is a bag of cards = " << match_chars("abc", 0, "It is a bag of cards", 0) << endl;
    cout << "abc -> It is a bag of books = " << match_chars("abc", 0, "It is a bag of books", 0) << endl;
    cout << "abc -> It is a classy bag = " << match_chars("abc", 0, "It is a classy bag", 0) << endl;

    cout << "\n[Bonus]" << endl;
    cout << "*.*.*.* simplified to " << simplify("*.*.*.*") << endl;
    cout << "*.ab*.aa*b.****ba**a*.***.**.a**a simplified to " << simplify("*.ab*.aa*b.****ba**a*.***.**.a**a") << endl;
    cout << "hu.t matches hurt? " << match_pattern("hu.t", "hurt") << endl;
    cout << "hu.t matches hunt? " << match_pattern("hu.t", "hunt") << endl;
    cout << "hu.t matches hut? " << match_pattern("hu.t", "hut") << endl;
    cout << "hu.t matches hurts? " << match_pattern("hu.t", "hurts") << endl;
    cout << "Cu*e matches Cue? " << match_pattern("Cu*e", "Cue") << endl;
    cout << "Cu*e matches Cure? " << match_pattern("Cu*e", "Cure") << endl;
    cout << "Cu*e matches Curve? " << match_pattern("Cu*e", "Curve") << endl;
    cout << "Cu*e matches A Cue? " << match_pattern("Cu*e", "A Cue") << endl;
    cout << "Cu*e matches A Cure? " << match_pattern("Cu*e", "A Cure") << endl;
    cout << "Cu*e matches A Curve? " << match_pattern("Cu*e", "A Curve") << endl;
    cout << "*ea* matches Dream Theater? " << match_pattern("*ea*", "Dream Theater") << endl;
    cout << "a*ba matches abbaba? " << match_pattern("a*ba", "abbaba") << endl;
    cout << "a*ba*ba matches abbababbba? " << match_pattern("a*ba*ba", "abbababbba") << endl;
    cout << "a*ba*a*a*ba matches abbababbba? " << match_pattern("a*ba*a*a*ba", "abbababbba") << endl;
    cout << "a*ba*a*a*ba matches abbababbaba? " << match_pattern("a*ba*a*a*ba", "abbababbaba") << endl;

    return EXIT_SUCCESS;
}