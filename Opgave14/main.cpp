//#define USE_RWOB_DNF // Use red, white, orange, blue ordering for DNF

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

constexpr size_t VECTOR_SIZE = 256;
constexpr size_t ITERATIONS  = 256;

// region Quicksort + DNF

template<typename T>
void dnf_rowb(vector<T>& data, int first, int last, int& red, int& blue) {
    assert(first >= 0 && last < data.size());
    // post: data is partitioned in a dnf manner, where red is index of last red and blue index of first blue.

    red = first - 1; // index last red
    blue = last + 1; // index first blue
    int white = last + 1; // index first white
    const T PIVOT = data[first + (last - first) / 2];

    while (red < white - 1) { // orange not empty
        const int NEXT = white - 1; // last orange

        if (data[NEXT] < PIVOT) { // belongs in red
            red++; // swap with first orange
            swap(data[red], data[NEXT]);
        } else if (data[NEXT] == PIVOT) { // belongs in white
            white--; // white area one larger
        } else { // belongs in blue
            white--;
            blue--; // swap with last white
            swap(data[NEXT], data[blue]);
        }
    }
}

template<typename T>
void dnf_rwob(vector<T>& data, int first, int last, int& red, int& blue) {
    assert(first >= 0 && last < data.size());
    // post: data is partitioned in a dnf manner, where red is index of last red and blue index of first blue.

    red = first - 1; // index last red
    blue = last + 1; // index first blue
    int white = first - 1; // index last white
    const T PIVOT = data[first + (last - first) / 2];

    while (white < blue - 1) { // orange not empty
        const int NEXT = blue - 1; // last orange

        if (data[NEXT] < PIVOT) { // belongs in red
            red++; // swap with first white/orange
            white++;
            swap(data[red], data[NEXT]);
            if(white != red) { // swap with first orange if a swap with white just occurred
                swap(data[white], data[NEXT]);
            }
        } else if (data[NEXT] == PIVOT) { // belongs in white
            white++; // swap with first orange
            swap(data[NEXT], data[white]);
        } else { // belongs in blue
            blue--; // blue area one larger
        }
    }
}

template<typename T>
void dnf(vector<T>& data, int first, int last, int& red, int& blue) {
    // trampoline function for either rwob or rowb dnf method.
#ifdef USE_RWOB_DNF
    dnf_rwob(data, first, last, red, blue);
#else
    dnf_rowb(data, first, last, red, blue);
#endif
}

template<typename T>
void quicksort(vector<T>& data, int first, int last) {
    assert (first >= 0 && last < static_cast<int>(data.size()));
    // post: data[first] ... data[last] is sorted

    if (first >= last) return;

    int red, blue;
    dnf(data, first, last, red, blue);
    quicksort(data, first, red);
    quicksort(data, blue, last);
}

template<typename T>
void quicksort(vector<T>& data) {
    // post: data is sorted.

    quicksort(data, 0, static_cast<int>(data.size()) - 1);
}

// endregion

// region Transform to iterative

template<typename T>
int largest(vector<T>& v, int low, int up) {
    assert(low >= 0 && up < v.size());
    // post: return value is index of largest element in range v[low, up]

    /*
    // Ugly slide translation
    int largest = low;

    while(low <= up) {
        largest = v[low] > v[largest] ? low : largest;
        ++low;
    }

    return largest;*/

    int largest = low;

    for(int i = low; i <= up; ++i) {
        largest = v[i] > v[largest] ? i : largest;
    }

    return largest;
}

template<typename T>
void sort(vector<T>& v, int n) {
    assert(n == v.size());
    // post: v is sorted (bubble sort).

    /*
    // Ugly slide translation
    while(n > 1) {
        const int POS = largest(v, 0, n - 1);

        swap(v[POS], v[n - 1]);
        --n;
    }*/

    for(int i = n - 1; i > 0; --i) {
        swap(v[i], v[largest(v, 0, i)]);
    }
}

// endregion

void test_quicksort() {
    // Sort a random vector of size VECTOR_SIZE ITERATIONS times and check if the sorted vector is indeed sorted.

    vector<int> vec(VECTOR_SIZE);

    // Use static seed for debugging purposes.
    srand(0xfeb25a02);

    for(auto i = 0; i < ITERATIONS; ++i) {
        generate_n(vec.begin(), vec.size(), rand);

        quicksort(vec);

        if(!is_sorted(vec.cbegin(), vec.cend())) {
            cerr << "Sort failed on iteration " << i << endl;

            for(auto x : vec) {
                cerr << x << endl;
            }

            exit(EXIT_FAILURE);
        }
    }

    cout << "All iterations sorted successfully" << endl;
}

void test_sort() {
    // Sort a random vector of size VECTOR_SIZE ITERATIONS times and check if the sorted vector is indeed sorted.

    vector<int> vec(VECTOR_SIZE);

    // Use static seed for debugging purposes.
    srand(0xfeb25a02);

    for(auto i = 0; i < ITERATIONS; ++i) {
        generate_n(vec.begin(), vec.size(), rand);

        sort(vec, static_cast<int>(vec.size()));

        if(!is_sorted(vec.cbegin(), vec.cend())) {
            cerr << "Sort failed on iteration " << i << endl;

            for(auto x : vec) {
                cerr << x << endl;
            }

            exit(EXIT_FAILURE);
        }
    }

    cout << "All iterations sorted successfully" << endl;
}

int main() {
    cout << "Testing quicksort" << endl;
    test_quicksort();

    cout << "Testing sort" << endl;
    test_sort();

    return EXIT_SUCCESS;
}