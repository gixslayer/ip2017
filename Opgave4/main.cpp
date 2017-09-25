#include <iostream>
#include <sstream>
#include <iomanip>

// Ciske Harsema - s1010048 - Informatica

enum Month {January=1,February,March,April,May,June,July,August,September,October,November,December} ;

/*  The function easter_base uses the Meeus/Jones/Butcher formula to compute
    the number that is used to determine on which month (easter_month below)
    and day (easter_day below) Easter is in the given year.
*/
int easter_base ( int year )
{
    const int A     = year % 19 ;
    const int B     = year / 100 ;
    const int C     = year % 100 ;
    const int D     = B / 4 ;
    const int E     = B % 4 ;
    const int F     = (B + 8) / 25 ;
    const int G     = (B - F + 1) / 3 ;
    const int H     = (19 * A + B - D - G + 15) % 30 ;
    const int I     = C / 4 ;
    const int K     = C % 4 ;
    const int L     = (32 + 2 * E + 2 * I - H - K) % 7 ;
    const int M     = (A + 11 * H + 22 * L) / 451 ;
    return H + L - 7 * M + 114 ;
}

/*  The result of easter_year is the day number of the month in which Easter occurs in
    the given year.
*/
int easter_day ( int year )
{
    return (easter_base (year) % 31) + 1 ;
}

/*  The result of easter_month is the month in which Easter occurs in the given year.
*/
Month easter_month ( int year )
{
    return static_cast<Month> (easter_base (year) / 31) ;
}

//region Assignment part 1: Leap years

/// Determines whether a given \a year is a leap year.
/// \param year - The year.
/// \return True if the year is a leap year. False otherwise.
bool is_leap_year ( int year )
{
    return year % 100 == 0 ? year % 400 == 0 : year % 4 == 0;
}

/// Returns the number of days in a given \a month and \a year.
/// \param year - The year.
/// \param month - The month.
/// \return The number of days in the month.
int number_of_days_in_month ( int year, Month month )
{
    switch (month) {
        case January:   return 31;
        case February:  return is_leap_year(year) ? 29 : 28;
        case March:     return 31;
        case April:     return 30;
        case May:       return 31;
        case June:      return 30;
        case July:      return 31;
        case August:    return 31;
        case September: return 30;
        case October:   return 31;
        case November:  return 30;
        case December:  return 31;
    }
}

//endregion

//region Assignment part 2: Holy days based on Easter

const int DAYS_PER_WEEK = 7;
const int CARNIVAL_OFFSET = -7 * DAYS_PER_WEEK;
const int GOOD_FRIDAY_OFFSET = -2;
const int WHITSUNTIDE_OFFSET = 7 * DAYS_PER_WEEK;
const int ASCENSION_DAY_OFFSET = -10;

/// Returns the name of the given \a month.
/// \param month - The month.
/// \return The name of the month.
std::string name_of_month(Month month) {
    switch (month) {
        case January:   return "January";
        case February:  return "February";
        case March:     return "March";
        case April:     return "April";
        case May:       return "May";
        case June:      return "June";
        case July:      return "July";
        case August:    return "August";
        case September: return "September";
        case October:   return "October";
        case November:  return "November";
        case December:  return "December";
    }
}

/// Returns the day of the year of a given date.
/// \param day - The day of the month.
/// \param month - The month.
/// \param year - The year.
/// \return The day of the year (1-365).
int day_of_year(int day, Month month, int year) {
    int result = day;

    for(int m = 1; m < month; m++) {
        result += number_of_days_in_month(year, static_cast<Month>(m));
    }

    return result;
}

/// Extracts the date of the given day of year in a year.
/// \param doy - The day of the year.
/// \param year - The year.
/// \return A pair of (day of month, month).
std::pair<int, Month> extract_date(int doy, int year) {
    int remainder = doy;
    Month month = January;

    while(remainder > number_of_days_in_month(year, month)) {
        remainder -= number_of_days_in_month(year, month);

        month = static_cast<Month>(month + 1);
    }

    return {remainder, month};
};

/// Formats a date.
/// \param doy - The day of the year.
/// \param year - The year.
/// \return The formatted date string.
std::string format_date(int doy, int year) {
    // Probably using code not yet allowed, so here is the ugly inlined version that does the same.
    /*int remainder = doy;
    int m = 1;

    while(remainder > number_of_days_in_month(year, static_cast<Month>(m))) {
        remainder -= number_of_days_in_month(year, static_cast<Month>(m));

        m++;
    }

    int day = remainder;
    Month month = static_cast<Month>(m);*/
    auto date = extract_date(doy, year);
    int day = date.first;
    Month month = date.second;
    std::stringstream ss;

    // Format as 'dd-mm-yyyy'
    //ss << std::setw(2) << std::setfill('0') << day << '-';
    //ss << std::setw(2) << std::setfill('0') << month << '-';
    //ss << year;
    // Format as 'Month day, year'
    ss << name_of_month(month) << ' ' << day << ", " << year;

    return ss.str();
}

/// Shows the holy days in the given \a year.
/// \param year - The year for which to show the holy days.
void show_holy_days(int year) {
    int easter_doy = day_of_year(easter_day(year), easter_month(year), year);
    int carnival_doy = easter_doy + CARNIVAL_OFFSET;
    int good_friday_doy = easter_doy + GOOD_FRIDAY_OFFSET;
    int whitsuntide_doy = easter_doy + WHITSUNTIDE_OFFSET;
    int ascension_day_doy = whitsuntide_doy + ASCENSION_DAY_OFFSET;

    std::cout << "[Holy days in " << year << ']' << std::endl;
    std::cout << "Easter (pasen):              " << format_date(easter_doy, year) << std::endl;
    std::cout << "Carnival (carnaval):         " << format_date(carnival_doy, year) << std::endl;
    std::cout << "Good Friday (Goede Vrijdag): " << format_date(good_friday_doy, year) << std::endl;
    std::cout << "Whitsuntide (Pinksteren):    " << format_date(whitsuntide_doy, year) << std::endl;
    std::cout << "Ascension Day (Hemelvaart):  " << format_date(ascension_day_doy, year) << std::endl;
}

/// Driver for assignment part 2.
void show_holy_days ()
{
    int year;
    char keep_running;

    do {
        std::cout << "Please enter a year: ";
        std::cin >> year;

        std::cout << std::endl;
        show_holy_days(year);
        std::cout << std::endl;

        std::cout << "Continue? (y/n): ";
        std::cin >> keep_running;
    } while(keep_running == 'y');
}

//endregion

//region Bonus

enum Day {Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday} ;

/// Returns the short name of the given \a day.
/// \param day - The day of the week.
/// \return The short 2 character name string of the day.
std::string name_of_day(Day day) {
    switch (day) {
        case Monday:    return "Mo";
        case Tuesday:   return "Tu";
        case Wednesday: return "We";
        case Thursday:  return "Th";
        case Friday:    return "Fr";
        case Saturday:  return "Sa";
        case Sunday:    return "Su";
    }
}

/// Returns the day of the week of a given date.
/// \param day - The day of the month.
/// \param month - The month.
/// \param year - The year.
/// \return The day of the week.
Day day_of_week(int day, Month month, int year) {
    // Calculate the day of the week by comparing against a known reference point, which in this case is the first day
    // of Easter (as it always on a sunday).
    int doy = day_of_year(day, month, year);
    int ref_doy = day_of_year(easter_day(year), easter_month(year), year);
    int difference = doy - ref_doy;

    if(difference >= 0) {
        return static_cast<Day>((difference + 6) % DAYS_PER_WEEK);
    } else {
        return static_cast<Day>( 6 - (-difference % DAYS_PER_WEEK));
    }
}

/// Output a calendar like display, showing all the days of a given \a month.
/// \param month - The month to display.
/// \param year - The year to display.
void show_month(Month month, int year) {
    int num_days = number_of_days_in_month(year, month);
    Day first_day = day_of_week(1, month, year);

    // Output the day name header.
    for(int day = 0; day < DAYS_PER_WEEK; day++) {
        std::cout << name_of_day(static_cast<Day>(day)) << " ";
    }
    std::cout << std::endl;

    // Output empty fill space till the first day of the month.
    for(int day = 0; day < first_day; day++) {
        std::cout << "   ";
    }

    // Output days of the month.
    for(int day = 1; day < num_days + 1; day++) {
        std::cout << std::setw(2) << day << ' ';

        // Output a new line after each sunday.
        if((day + first_day - 1) % DAYS_PER_WEEK == Sunday) {
            std::cout << std::endl;
        }
    }

    // Make sure the formatting stays consistent in case the month doesn't end on a sunday.
    if(day_of_week(num_days, month, year) != Sunday) {
        std::cout << std::endl;
    }
}

/// Driver for the bonus assignment.
void show_months() {
    int month;
    int year;
    char keep_running;

    do {
        std::cout << "Please enter a month (1-12, 1 being January): ";
        std::cin >> month;
        std::cout << "Please enter a year: ";
        std::cin >> year;

        std::cout << std::endl;
        show_month(static_cast<Month>(month), year);
        std::cout << std::endl;

        std::cout << "Continue? (y/n): ";
        std::cin >> keep_running;
    } while(keep_running == 'y');
}

//endregion

/// Application entry point.
int main()
{
    show_holy_days() ;
    //show_months();

    return EXIT_SUCCESS;
}