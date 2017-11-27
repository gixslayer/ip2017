#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <functional>
#include <cassert>

using namespace std;
using namespace chrono;

// Ciske Harsema - s1010048 - Informatica
// Note: Debug builds can be really slow due to extensive assert usage to check pre/post conditions.

/// Seed to use for the sorting test.
constexpr int TESTING_SEED = 356857;

// region Heap sort

/// Test if the content of a vector is increasing.
/// \tparam T - Vector content type.
/// \param vector - The vector to test.
/// \return True if the content of the vector is increasing, false otherwise.
template<typename T>
bool is_sorted(const vector<T>& vector) {
    // pre:
    assert(true);
    // post:
    // result is true if the content of vector is increasing, false otherwise.

    for(size_t i = 0; i < vector.size() -1; ++i) {
        if(vector[i] > vector[i+1]) {
            return false;
        }
    }

    return true;
}

/// Test if the content of a vector forms a heap.
/// \tparam T - Vector content type.
/// \param vector - The vector to test.
/// \return True if the content of the vector forms a heap, false otherwise.
template<typename T>
bool is_heap(const vector<T>& vector) {
    // pre:
    assert(true);
    // post:
    // result is true if vector is a heap, false otherwise.

    for(auto i = vector.size() - 1; i > 0; --i) {
        auto parent = (i-1)/2;

        if(vector[i] > vector[parent]) {
            return false;
        }
    }

    return true;
}

/// Push the value at \a index up the tree.
/// \tparam T - Vector content type.
/// \param vector - The vector that contains the tree.
/// \param index - The index of the value to push up.
template<typename T>
void push_up(vector<T>& vector, size_t index) {
    // pre:
    assert(index < vector.size());
    // post:
    // Value at index has been pushed up.

    if(index > 0) {
        size_t parent_index = (index-1)/2;
        T value = vector[index];
        T parent_value = vector[parent_index];

        if(value > parent_value) {
            swap(vector[index], vector[parent_index]);

            push_up(vector, parent_index);
        }
    }
}

/// Reorder \a vector into a heap.
/// \tparam T - Vector content type.
/// \param vector - The vector to reorder.
template<typename T>
void build_heap(vector<T>& vector) {
    // pre:
    assert(true);
    // post:
    // vector is a heap (no node is larger than it's parent).

    for(size_t i = 1; i < vector.size(); ++i) {
        push_up(vector, i);
    }

    assert(is_heap(vector));
}

/// Push the value at \a index down the tree.
/// \tparam T  - Vector content type.
/// \param vector - The vector that contains the tree.
/// \param index - The index of the value to push down.
/// \param length - The number of elements in \a vector that are part of the tree.
template<typename T>
void push_down(vector<T>& vector, size_t index, size_t length) {
    // pre:
    assert(length <= vector.size());
    // post:
    // Value at index has been pushed down.

    if(index < length) {
        T value = vector[index];
        size_t lchild_index = index*2+1;
        size_t rchild_index = index*2+2;
        bool has_lchild = lchild_index < length;
        bool has_rchild = rchild_index < length;
        bool has_larger_child = (has_lchild && vector[lchild_index] > value) || (has_rchild && vector[rchild_index] > value);

        if(has_larger_child) {
            size_t swap_index = 0;

            if(has_lchild && has_rchild) {
                swap_index = vector[lchild_index] > vector[rchild_index] ? lchild_index : rchild_index;
            } else { // only has a left child
                swap_index = lchild_index;
            }

            swap(vector[index], vector[swap_index]);
            push_down(vector, swap_index, length);
        }
    }
}

/// Sort a heap in place in ascending order.
/// \tparam T - Vector content type.
/// \param vector - The vector that contains the heap to sort.
template<typename T>
void pick_heap(vector<T>& vector) {
    // pre:
    assert(is_heap(vector));
    // post:
    // vector is sorted.

    for(size_t i = vector.size(); i > 0; --i) {
        swap(vector[0], vector[i -1]);
        push_down(vector, 0, i-1);
    }

    assert(is_sorted(vector));
}

/// Sort a vector in place in ascending order.
/// \tparam T - Vector content type.
/// \param vector - The vector to sort.
template<typename T>
void heap_sort(vector<T>& vector) {
    // pre:
    assert(true);
    // post:
    // vector is sorted.

    build_heap(vector);
    pick_heap(vector);

    assert(is_sorted(vector));
}

// endregion

/// Test the Heap sort algorithm by sorting random sequences.
/// \param iterations - The number of sequences.
/// \param length - The length of each sequence.
/// \return True if all sequences sorted successfully, false otherwise.
bool test_sorting(size_t iterations, size_t length) {
    // pre:
    assert(iterations > 0);
    assert(length > 0);
    // post:
    // Tested the sorting of iterations vectors of length elements. Returns true if the testing was successful.

    vector<int> vector;
    default_random_engine engine(TESTING_SEED);
    uniform_int_distribution<int> distribution(numeric_limits<int>::min(), numeric_limits<int>::max());
    auto rng = bind(distribution, engine);
    bool all_sorted = true;

    vector.reserve(length);

    for(size_t i = 0; i < iterations && all_sorted; ++i) {
        vector.clear();
        for(size_t j = 0; j < length; ++j) {
            vector.push_back(rng());
        }

        heap_sort(vector);
        all_sorted &= is_sorted(vector);

        if(!all_sorted) {
            cout << "Iteration " << i << " failed to sort" << endl;
        }
    }

    return all_sorted;
}

/// Application entry point.
/// \return Application exit code.
int main() {
    auto now = high_resolution_clock::now();
    bool result = test_sorting(1000, 1000);
    auto end = high_resolution_clock::now();
    auto duration = end-now;

    cout << "Sorted in " << duration_cast<milliseconds>(duration).count() << " ms" << endl;
    cout << "Result: " << (result ? "PASS" : "FAIL") << endl;

    return EXIT_SUCCESS;
}

/*
 * [1a]
 * input: year
 * order: O(1)
 *
 * Algorithm performs a constant finite number of operations, thus does not depend on the input, hence order O(1).
 *
 * [1b]
 * input: x
 * order: O(sqrt(x))
 *
 * Worst case is sqrt(x)-2 loop iterations of a constant number of operations, hence (sqrt(x)-2)O(1) = O(sqrt(x)).
 *
 * [2a]
 *      15
 *     /  \
 *    40  42
 *   / \  | \
 * -15 30 35 5
 *
 *      40
 *     /  \
 *    15  42
 *   / \  | \
 * -15 30 35 5
 *
 *      42
 *     /  \
 *    15  40
 *   / \  | \
 * -15 30 35 5
 *
 *      42
 *     /  \
 *    30  40
 *   / \  | \
 * -15 15 35 5
 *
 * {42, 30, 40, -15, 15, 35, 5} // initial heap
 * {5, 30, 40, -15, 15, 35, 42} // swap
 * {40, 30, 35, -15, 15, 5, 42} // push_down
 * {5, 30, 35, -15, 15, 40, 42} // swap
 * {35, 30, 5, -15, 15, 40, 42} // push_down
 * {15, 30, 5, -15, 35, 40, 42} // swap
 * {30, 15, 5, -15, 35, 40, 42} // push_down
 * {-15, 15, 5, 30, 35, 40, 42} // swap
 * {15, -15, 5, 30, 35, 40, 42} // push_down
 * {5, -15, 15, 30, 35, 40, 42} // swap
 * {5, -15, 15, 30, 35, 40, 42} // push_down (no change)
 * {-15, 5, 15, 30, 35, 40, 42} // swap
 * {-15, 5, 15, 30, 35, 40, 42} // push_down (no change) -> done
 *
 * [2b]
 * push_up: O(log n)
 * Worst case of tree depth-1 number of pushes, which is ceil(log n)-2, thus O(log n).
 *
 * build_heap: O(n log n)
 * Performs n-1 number of push_up calls, thus (n-1)*O(log n) = O(n log n).
 *
 * push_down: O(log n)
 * Worst case of tree depth-1 number of pushes, which is ceil(log n)-2, thus O(log n).
 *
 * pick_heap: O(n log n)
 * Performs n times swap (order 1) and push_down (order log n), thus n*O(log n)*O(1) = O(n log n).
 *
 * Heap Sort: O(n log n)
 * Performs build_heap (order n log n) and pick_heap (order n log n), thus O(n log n)+O(n log n) = 2*O(n log n) = O(n log n).
 */