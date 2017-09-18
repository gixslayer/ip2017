#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>

// Ciske Harsema - s1010048 - Informatica

//region Mandatory

// Part 1.1
/*
 * Test cases (with epsilon = 0.1):
 *
 * Inclusion (v=0)
 * i=0, a=0, x=0, b=1
 *
 * Inclusion (v=1)
 * i=0, a=1, x=1, b=2
 *
 * Inclusion (v=0.5)
 * i=0, a=0,   x=0.5,  b=1
 * i=1, a=0.5, x=0.75, b=1
 *
 * Inclusion (v=4)
 * i=0, a=2, x=2, b=3
 *
 * Inclusion (v=13)
 * i=0, a=3,      x=3.5,     b=4
 * i=1, a=3.5,    x=3.75,    b=4
 * i=2, a=3.5,    x=3.625,   b=3.75
 * i=3, a=3.5,    x=3.5625,  b=3.625
 * i=4, a=3.5625, x=3.59375, b=3.625
 *
 * Newton-Raphson (v=0)
 * i=0, x=1
 * i=1, x=0.5
 * i=2, x=0.25
 *
 * Newton-Raphson (v=1)
 * i=0, x=1
 *
 * Newton-Raphson (v=0.5)
 * i=0, x=1
 * i=1, x=0.75
 *
 * Newton-Raphson (v=4)
 * i=0, x=2
 *
 * Newton-Raphson (v=13)
 * i=0, x=3
 * i=1, x=3.66667
 * i=2, x=3.60606
 */

// Part 1.4
/*
 * Both algorithms have identical performance (1 step) in case v is a perfect square.
 * With the exception of v=0, Newton-Raphson has identical or better performance (less steps) compared to the inclusion
 * algorithm. This is especially true for larger (>10) v values.
 */

/// Find the largest natural number n >= 0, for which n*n &lt= \a v holds.
/// \param v - The real number for which to find the root.
/// \return The natural number n.
static int find_largest_root(double v) {
    int n = 0;

    while(n * n <= v) {
        n++;
    }

    return n - 1;
}

// Part 1.2
/// Approximates the square root of \a v, to within the desired precision \a epsilon, using the inclusion algorithm.
/// \param epsilon - The desired precision.
/// \param v - The real number for which to approximate the root.
static void inclusion(double epsilon, double v) {
    int a_0 = find_largest_root(v);
    int b_0 = a_0 + 1;

    if(a_0 * a_0 == v) {
        // Special case where a_0 is already the desired value.
        std::cout << 0 << '\t' << a_0 << '\t' << a_0 << '\t' << b_0 << std::endl;
    } else {
        double a_i = a_0;
        double b_i = b_0;
        double x_i = (a_i + b_i) / 2.0;
        int i = 0;

        while (fabs(x_i * x_i - v) > epsilon) {
            // Display values of the current iteration.
            std::cout << i << '\t' << a_i << '\t' << x_i << '\t' << b_i << std::endl;

            // Update variables for the next iteration.
            if (x_i * x_i < v) {
                a_i = x_i;
            } else {
                b_i = x_i;
            }

            x_i = (a_i + b_i) / 2.0;
            i++;
        }

        // Algorithm completed, display final result.
        std::cout << i << '\t' << a_i << '\t' << x_i << '\t' << b_i << std::endl;
    }
}

// Part 1.3
/// Approximates the square root of \a v, to within the desired precision \a epsilon, using the Newton-Raphson algorithm.
/// \param epsilon - The desired precision.
/// \param v - The real number for which to approximate the root.
static void newtonraphson(double epsilon, double v) {
    double x_i = v < 1 ? 1 : find_largest_root(v);
    int i = 0;

    while (fabs(x_i * x_i - v) > epsilon) {
        // Display values of the current iteration.
        std::cout << i << '\t' << x_i << std::endl;

        // Update variables for the next iteration.
        x_i = x_i - (x_i * x_i - v) / (2 * x_i);
        i++;
    }

    // Algorithm completed, display final result.
    std::cout << i << '\t' << x_i << std::endl;
}

/// Driver for the mandatory part of the assignment.
static void square_root() {
    bool keep_running = true;
    double epsilon = 0.1;
    double v;

    while(keep_running) {
        std::cout << "Enter v (or a negative number to quit): ";
        std::cin >> v;

        if(v < 0) {
            keep_running = false;
        } else {
            std::cout << "[Inclusion]" << std::endl;
            inclusion(epsilon, v);
            std::cout << std::endl;

            std::cout << "[Newton-Raphson]" << std::endl;
            newtonraphson(epsilon, v);
            std::cout << std::endl;
        }
    }
}

//endregion

//region Bonus

const std::string BINARY_CHARS = "01";
const std::string OCTAL_CHARS = "01234567";
const std::string DECIMAL_CHARS = "0123456789";
const std::string HEX_CHARS = "0123456789ABCDEF";

/// Returns the string of valid characters for a given number base.
/// \param base - The number base (either 2, 8, 10 or 16).
/// \throws invalid_argument - If \a base has any value not specified by \a base.
/// \return The string of valid characters.
static const std::string& get_charset_for_base(int base) {
    switch (base) {
        case 2:     return BINARY_CHARS;
        case 8:     return OCTAL_CHARS;
        case 10:    return DECIMAL_CHARS;
        case 16:    return HEX_CHARS;
        default:    throw std::invalid_argument{"unknown base"};
    }
}

/// Parses the number in the string \a str using the number base specified by \a base.
/// \param str - The string to parse.
/// \param base - The number base (either 2, 8, 10 or 16).
/// \throws invalid_argument - If \a str contains any invalid characters for the given \a base.
/// \return The parsed number.
static int parse_number(const std::string& str, int base) {
    int result = 0;
    const std::string& charset = get_charset_for_base(base);

    for(int i = 0; i < str.length(); i++) {
        char c = str[str.length() - i - 1];
        size_t index = charset.find(c);

        // Abort if the number is badly formatted.
        if(index == std::string::npos) {
            throw std::invalid_argument{"invalid character"};
        }

        result += index * pow(base, i);
    }

    return result;
}

/// Formats a number in the specified number base.
/// \param number - The number to format.
/// \param base - The number base (either 2, 8, 10 or 16).
/// \return The formatted number string.
static std::string format_number(int number, int base) {
    const std::string& charset = get_charset_for_base(base);
    std::stringstream ss;
    int remainder = number;
    int i = 0;

    do {
        int weight = static_cast<int>(pow(base, i));
        int next_weight = static_cast<int>(pow(base, i + 1));
        int mod = remainder % next_weight;
        char c = charset[mod / weight];

        ss << c;
        remainder -= mod;
        i++;
    } while(remainder != 0);

    // Since the algorithm works back to front, the final string has to be reversed.
    std::string result = ss.str();
    std::reverse(result.begin(), result.end());

    return result;
}

/// Driver for the bonus part of the assignment.
static void bonus() {
    int input_base;
    int output_base;
    std::string input;

    std::cout << "Enter input base: ";
    std::cin >> input_base;
    std::cout << "Enter output base: ";
    std::cin >> output_base;
    std::cout << "Enter input: ";
    std::cin >> input;

    // Convert to upper case in case user input contains lower case hex characters.
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);
    int number = parse_number(input, input_base);
    std::string output = format_number(number, output_base);

    std::cout << "Output: " << output << std::endl;
}

//endregion

/// Application entry point.
int main() {
    //bonus();
    square_root();

    return EXIT_SUCCESS;
}