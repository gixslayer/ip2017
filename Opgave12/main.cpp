#define BONUS

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

struct WishList {
    uint32_t budget;
    vector<string> names;
};

struct StoreItem {
    uint32_t stock;
    uint32_t price;
};

map<string, StoreItem> gift_store;

// region Part 1

/// Check if the gift store has the gift, and that it is in stock.
/// \param name - The name of the gift.
/// \return - True if the gift can be granted, false otherwise.
bool store_has_gift(const string& name) {
    // pre:
    assert(true);
    // post:
    // return value is true if the gift can be granted, false otherwise.

    auto entry = gift_store.find(name);

    return entry != gift_store.end() && entry->second.stock != 0;
}

/// Check if a gift is on a wish list.
/// \param wish_list - The wish list to check.
/// \param name - The name of the gift.
/// \return True if the gift is on the wish list, false otherwise.
bool has_gift(const WishList& wish_list, const string& name) {
    // pre:
    assert(true);
    // post:
    // return value is true if the gift is on the wish list, false otherwise.

    const auto& n = wish_list.names;

    return find(n.cbegin(), n.cend(), name) != n.cend();
}

/// Add a gift to the wish list.
/// \param wish_list - The wish list to add the gift to.
/// \param name - The name of the gift.
/// \remark Store stock is only reduced during bonus builds.
void add_gift(WishList& wish_list, const string& name) {
    // pre:
    assert(true);
    // post:
    // gift is added to the wish list, and the store stock reduced if this is a bonus build.

    wish_list.names.push_back(name);
    wish_list.budget += gift_store[name].price;

#ifdef BONUS
    gift_store[name].stock -= 1;
#endif
}

/// Optimally grant gifts of the wish list.
/// \param wish_list - The wish list to grant.
/// \param index - Index of the current wish.
/// \param gift_list - List of gifts granted so far.
/// \return The optimal list of gifts granted so far.
WishList gifts(const WishList& wish_list, size_t index, const WishList& gift_list) {
    // pre:
    assert(index <= wish_list.names.size());
    // post:
    // return value is the optimal list of gifts granted from the wish list, starting at index.

    if(gift_list.budget > wish_list.budget) return {0};
    if(index == wish_list.names.size())     return gift_list;

    const string& name = wish_list.names[index];

    // Ignore wishes already granted or not in stock/store.
    if(has_gift(gift_list, name))   return gifts(wish_list, index + 1, gift_list);
    if(!store_has_gift(name))       return gifts(wish_list, index + 1, gift_list);

    WishList without_gift = gifts(wish_list, index + 1, gift_list);

    WishList new_gift_list = gift_list;
    add_gift(new_gift_list, name);
    WishList with_gift = gifts(wish_list, index + 1, new_gift_list);

#ifdef BONUS
    // If not granting the gift produced a better result, then restore the store stock.
    if(without_gift.budget > with_gift.budget) gift_store[name].stock += 1;
#endif

    return with_gift.budget >= without_gift.budget ? with_gift : without_gift;
}

/// Optimally grant gifts of the wish list.
/// \param wish_list - The wish list to grant.
/// \return The optimal list of gifts granted.
WishList gifts(const WishList& wish_list) {
    // pre:
    assert(true);
    // post:
    // return value is the optimal list of gifts granted from the wish list.

    WishList gift_list{};

    return gifts(wish_list, 0, gift_list);
}

// endregion

// region Part 2

/// Load the gift store from disk.
/// \param is - The file stream to load as a gift store.
void load_gift_store(ifstream& is) {
    // pre:
    assert(is.is_open());
    // post:
    // Gift store loaded from is and stored in gift_store.

    string line;

    while(getline(is, line)) {
        if(line.empty()) continue;

        auto sp1 = line.find(' ');
#ifdef BONUS
        auto sp2 = line.find(' ', sp1 + 1);
        uint32_t stock = static_cast<uint32_t>(stoi(line.substr(0, sp1)));
        uint32_t price = static_cast<uint32_t>(stoi(line.substr(sp1 + 1, sp2)));
#else
        auto sp2 = sp1;
        uint32_t stock = 1;
        uint32_t price = static_cast<uint32_t>(stoi(line.substr(0, sp1)));
#endif

        string name = line.substr(sp2 + 1);

        gift_store[name] = {stock, price};
    }
}

/// Load a wish list from disk.
/// \param is - The file stream to load as a wish list.
/// \return The loaded wish list.
WishList load_wish_list(ifstream& is) {
    // pre:
    assert(is.is_open());
    // post:
    // return value is a wish list loaded from is.

    WishList list;
    string line;

    getline(is, line);
    list.budget = static_cast<uint32_t>(stoi(line));

    while(getline(is, line)) {
        if(line.empty()) continue;

        list.names.push_back(line);
    }

    return list;
}

/// Load all application data from disk.
/// \param wish_lists - Output vector to hold all loaded named wish lists.
/// \return True if the loading was successful, false otherwise.
bool load_data(vector<pair<string, WishList>>& wish_lists) {
    // pre:
    assert(true);
    // post:
    // return value is true if all application data loaded successfully, false otherwise.

    vector<string> list_names {"Andrew", "Belle", "Chris", "Desiree", "Edward", "Fabienne"};
#ifdef BONUS
    string giftstore_path = "../giftstore_bounded.txt";
#else
    string giftstore_path = "../giftstore.txt";
#endif

    ifstream is_store{giftstore_path};

    if(!is_store) {
        cerr << "Could not open " << giftstore_path << endl;

        return false;
    }

    load_gift_store(is_store);

    for(const auto& name : list_names) {
        ifstream is_list{"../" + name + ".txt"};

        if(!is_list) {
            cerr << "Could not open " << name << endl;

            return false;
        }

        WishList wish_list = load_wish_list(is_list);

        wish_lists.emplace_back(name, wish_list);
    }

    return true;
}

// endregion

// region Part 3

/// Print \a wish_list to the standard output.
/// \param wish_list - Wish list to print.
void print_wish_list(const WishList& wish_list) {
    // pre:
    assert(true);
    // post:
    // wish_list has been printed to the standard output.

    cout << "Budget: " << wish_list.budget << endl;
    cout << "Items:  ";

    for(const auto& name : wish_list.names) {
        cout << name << "\n        ";
    }

    if(wish_list.names.empty()) cout << '\n';
}

/// Grant gifts of the wish list, and print the result to the standard output.
/// \param wish_list_of - Named wish list to grant and print.
void grant_gift(const pair<string, WishList>& wish_list_of) {
    // pre:
    assert(true);
    // post:
    // The wish list has been granted, and the results printed to the standard output.

    const auto& name = wish_list_of.first;
    const auto& list = wish_list_of.second;
    WishList gift_list = gifts(list);

    cout << "[Wish list of " << name << "]" << endl;
    print_wish_list(list);
    cout << endl;

    cout << "[Gifts given to " << name << "]" << endl;
    cout << "Change: " << list.budget - gift_list.budget << endl;
    print_wish_list(gift_list);
    cout << endl;
}

/// Test the granting of gifts for the mandatory assignment.
/// \param wish_lists - All named lists to grant.
void test_gifts(const vector<pair<string, WishList>>& wish_lists) {
    // pre:
    assert(true);
    // post:
    // All named lists have been granted, and the results printed to the standard output.

    for(const auto& wish_list : wish_lists) {
        grant_gift(wish_list);
    }
}

// endregion

// region Bonus

/// Return a lower case version of \a str.
/// \param str - The input string.
/// \return The lower case version of \a str.
string to_lower(const string& str) {
    // pre:
    assert(true);
    // post:
    // return value is the lower case version of str.

    string result;
    result.resize(str.size());

    transform(str.cbegin(), str.cend(), result.begin(), ::tolower );

    return result;
}

/// Driver for the bonus assignment.
/// \param wish_lists - All available named wish lists.
void test_bonus(const vector<pair<string, WishList>>& wish_lists) {
    // pre:
    assert(true);
    // post:
    // User granted gifts and has confirmed application exit.

    char quit = 'n';
    string input;
    map<string, pair<string, WishList>> lookup;

    // Case insensitive lookup table.
    for(const auto& wish_list : wish_lists) {
        string name = to_lower(wish_list.first);

        lookup[name] = wish_list;
    }

    do {
        cout << "Enter name: ";
        cin >> input;

        auto entry = lookup.find(to_lower(input));

        if(entry != lookup.end()) {
            grant_gift(entry->second);
        } else {
            cout << "Unknown name, valid names are:";
            for(const auto& list : wish_lists) {
                cout << " " << list.first;
            }
            cout << endl;

            cout << "Exit application? (y/n): ";
            cin >> quit;
        }

        cout << endl;
    } while(quit != 'y');
}

// endregion

/// Application entry point.
/// \return Application exit code (0 if successful, non-zero otherwise).
int main() {
    vector<pair<string, WishList>> wish_lists;

    if(!load_data(wish_lists)) {
        cerr << "Error: failed to load data" << endl;

        return EXIT_FAILURE;
    }

#ifdef BONUS
    test_bonus(wish_lists);
#else
    test_gifts(wish_lists);
#endif

    return EXIT_SUCCESS;
}

/*
 * Output for part 3

[Wish list of Andrew]
Budget: 5000
Items:  cd A Darker Shade Of White van Navarone
        sport Skateboard
        spel Mens erger je niet!
        spel De kolonisten van Catan
        boek In de ban van de ring - reisgenoten van J.R.R. Tolkien
        boek In de ban van de ring - de twee torens van J.R.R. Tolkien
        boek In de ban van de ring - terugkeer van de koning van J.R.R. Tolkien
        cd Synthesized van Junkie XL
        cd Concrete and Gold van Foo Fighters

[Gifts given to Andrew]
Change: 2
Budget: 4998
Items:  spel Mens erger je niet!
        spel De kolonisten van Catan

[Wish list of Belle]
Budget: 2500
Items:  speelgoed klei
        speelgoed kleurpotloden
        speelgoed stiften
        speelgoed tekenblok
        spel Jenga
        spel Mens erger je niet!
        spel Kolonisten van Catan Junior
        sport kinderfiets
        Wii U

[Gifts given to Belle]
Change: 102
Budget: 2398
Items:  speelgoed kleurpotloden
        speelgoed tekenblok
        spel Mens erger je niet!

[Wish list of Chris]
Budget: 3000
Items:  Playstation 4
        Xbox One
        Wii U
        spel Mens erger je niet!
        cd Nothing but the beat van David Guetta
        cd Synthesized van Junkie XL
        cd The 2nd law van Muse
        cd Concrete and Gold van Foo Fighters
        boek Het leven van een loser deel 1 van J. Kinney
        boek Het leven van een loser deel 2 van J. Kinney
        boek Het leven van een loser deel 3 van J. Kinney
        boek Het leven van een loser deel 4 van J. Kinney
        boek Het leven van een loser deel 5 van J. Kinney
        boek Het leven van een loser deel 6 van J. Kinney
        boek Weaveworld van C. Barker
        spel kaarten
        spel Kolonisten van Catan Junior

[Gifts given to Chris]
Change: 3
Budget: 2997
Items:  spel Mens erger je niet!
        cd The 2nd law van Muse
        spel kaarten

[Wish list of Desiree]
Budget: 7500
Items:  boek Fiese Ferien van J. Till
        boek Fette Ferien van J. Till
        boek Bitterschokolade van M. Pressler
        boek Crazy van B. Lebert
        dvd Brave van Pixar
        dvd Wall-E van Pixar
        dvd Up van Pixar
        cd Concrete and Gold van Foo Fighters
        boek Het spel van de engel van C.R. Zafon
        make-up lippenstift
        make-up mascara
        make-up nagellak
        spel Mens erger je niet!
        spel De kolonisten van Catan

[Gifts given to Desiree]
Change: 3
Budget: 7497
Items:  dvd Wall-E van Pixar
        dvd Up van Pixar
        spel De kolonisten van Catan

[Wish list of Edward]
Budget: 45000
Items:  dvd Avatar van James Cameron
        blu-ray Inside Out van Pixar
        cd Different Shades Of Blue van Joe Bonamassa
        cd Concrete and Gold van Foo Fighters
        boek De schaduw van de wind van C.R. Zafon
        boek Het spel van de engel van C.R. Zafon
        boek 10 vrolijke verhalen van S. King
        sport volwassen fiets
        boek The Hobbit van J.R.R. Tolkien
        boek The Silmarillion van J.R.R. Tolkien
        sport hardloopschoenen
        spel Monopoly
        spel Mens erger je niet!
        spel De kolonisten van Catan
        Playstation 4
        boek In de ban van de ring - reisgenoten van J.R.R. Tolkien
        boek In de ban van de ring - de twee torens van J.R.R. Tolkien
        boek In de ban van de ring - terugkeer van de koning van J.R.R. Tolkien

[Gifts given to Edward]
Change: 4
Budget: 44996
Items:  cd Concrete and Gold van Foo Fighters
        boek The Silmarillion van J.R.R. Tolkien
        spel De kolonisten van Catan
        Playstation 4
        boek In de ban van de ring - reisgenoten van J.R.R. Tolkien
        boek In de ban van de ring - de twee torens van J.R.R. Tolkien

[Wish list of Fabienne]
Budget: 20000
Items:  sport langlaufski
        spel Mens erger je niet!
        spel De kolonisten van Catan
        cd Marco van Marco Borsato
        cd Als geen ander van Marco Borsato
        cd De waarheid van Marco Borsato
        cd De bestemming van Marco Borsato
        cd Luid en duidelijk van Marco Borsato
        cd Onderweg van Marco Borsato
        cd Dromen durven delen van Marco Borsato
        cd Duizend spiegels van Marco Borsato
        cd Evenwicht van Marco Borsato
        sport mountainbike
        sport racefiets
        sport handbal
        Playstation 4

[Gifts given to Fabienne]
Change: 103
Budget: 19897
Items:  sport langlaufski
        spel De kolonisten van Catan
        cd Evenwicht van Marco Borsato
        sport handbal

 */