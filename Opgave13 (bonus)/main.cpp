//#define PROFILE_ONLY

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <cassert>
#include <cstring>
#include <unordered_set>
#include <chrono>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

constexpr int PUZZLE_WIDTH = 4;
constexpr int PUZZLE_HEIGHT = 5;
constexpr int PUZZLE_SIZE = PUZZLE_WIDTH * PUZZLE_HEIGHT;
constexpr int SOLUTION_TILES[] = { 13, 14, 17, 18 };

// std::hash<T> requires a size_t return type, but as the hashing function needs 60 bits to guarantee perfect hashing
// this only works on x64 builds.
static_assert(sizeof(size_t) >= sizeof(uint64_t), "Please compile as x64");

enum class Cell {
    black, red, green, yellow, empty
};

enum class Direction {
    north, east, south, west
};

// Order might seem somewhat random here, but it will produce an identical solution to the given example.
Direction SEARCH_DIRECTIONS[] = { Direction::north, Direction::south, Direction::west, Direction::east };

struct Pos {
    unsigned int c;
    unsigned int r;
};

struct Move {
    Pos piece;
    Direction dir;
};

class Puzzle {
public:
    Puzzle() : m_cells{}, m_hash_valid{}, m_hashcode{} {}
    explicit Puzzle(const vector<Cell>& cells);

    Cell operator[](uint32_t cell) const { return m_cells[cell]; }
    Cell operator[](Pos pos) const { return m_cells[pos.c + pos.r * PUZZLE_WIDTH]; }
    bool operator==(const Puzzle& other) const;
    bool operator!=(const Puzzle& other) const { return !(*this == other); }

    void swap(uint32_t c1, uint32_t r1, uint32_t c2, uint32_t r2);
    Cell get(uint32_t c, uint32_t r) const;
    bool completed() const;
    void move(Move move);
    vector<Move> get_moves() const;
    vector<Pos> get_pieces() const;
    bool can_move(Pos piece, Direction dir) const;
    uint64_t hashcode() const;

private:
    Cell m_cells[PUZZLE_SIZE];
    mutable bool m_hash_valid;
    mutable uint64_t m_hashcode;
};

struct Solution {
    bool solved;
    Puzzle puzzle;
    vector<Puzzle> path;
};

struct Candidate {
    Puzzle puzzle;
    int parent;
};

// Inject specialization of std::hash<T>, so that it is used for standard containers (unordered_set in this case).
namespace std {
    template<>
    struct hash<Puzzle> {
        typedef Puzzle argument_type;
        typedef size_t result_type;

        result_type operator()(const argument_type& puzzle) const noexcept {
            return puzzle.hashcode();
        }
    };
}

using CandidateChecker = unordered_set<Puzzle>;

bool pos_can_move(const Pos& pos, Direction dir, size_t width, size_t height) {
    assert(true);
    // post: return value is true iff pos can move in dir and stay within bounds 0, 0, width, height.

    switch (dir) {
        case Direction::north:  return pos.r > 0;
        case Direction::east:   return pos.c + 1 < width;
        case Direction::south:  return pos.r + 1 < height;
        case Direction::west:   return pos.c > 0;
    }
}

Pos pos_move(const Pos& pos, Direction dir) {
    assert(!(pos.c == 0 && dir == Direction::west));
    assert(!(pos.r == 0 && dir == Direction::north));
    // post: return value is pos moved in dir.

    switch (dir) {
        case Direction::north:  return { pos.c, pos.r - 1 };
        case Direction::east:   return { pos.c + 1, pos.r };
        case Direction::south:  return { pos.c, pos.r + 1 };
        case Direction::west:   return { pos.c - 1, pos.r };
    }
}

Cell parse_cell(char c) {
    assert(c == 'b' || c == '.' || c =='g' || c == 'r' || c == 'y');
    // post: return value is a Cell of the correct type.

    switch (c) {
        case 'b': return Cell::black;
        case '.': return Cell::empty;
        case 'g': return Cell::green;
        case 'r': return Cell::red;
        case 'y': return Cell::yellow;
    }
}

// region Stream I/O

ostream& operator<<(ostream& os, Cell cell) {
    assert(true);
    // post: cell is printed to os.

    switch (cell) {
        case Cell::empty:   return os << '.';
        case Cell::black:   return os << 'b';
        case Cell::green:   return os << 'g';
        case Cell::red:     return os << 'r';
        case Cell::yellow:  return os << 'y';
    }
}

ostream& operator<<(ostream& os, const Puzzle& puzzle) {
    assert(true);
    // post: puzzle is printed to os.

    for (auto i = 0; i < PUZZLE_SIZE; ++i) {
        os << puzzle[i];

        if ((i + 1) % PUZZLE_WIDTH == 0) os << endl;
    }

    return os;
}

ostream& operator<<(ostream& os, const Solution& solution) {
    assert(true);
    // post: each node in the solution path is printed to os.

    for (const auto& node : solution.path) {
        os << node << endl;
    }

    return os;
}

istream& operator>>(istream& is, Puzzle& puzzle) {
    assert(true);
    // post: puzzle contains the puzzle loaded from is.

    string line;
    vector<Cell> cells;

    while (std::getline(is, line)) {
        assert(line.size() == PUZZLE_WIDTH);

        transform(line.cbegin(), line.cend(), back_inserter(cells), parse_cell);
    }

    assert(cells.size() == PUZZLE_SIZE);

    puzzle = Puzzle{ cells };

    return is;
}

// endregion

// region Puzzle impl

Puzzle::Puzzle(const vector<Cell>& cells) : m_hashcode{}, m_hash_valid{} {
    assert(cells.size() == PUZZLE_SIZE);
    // post: Puzzle constructed from the given cells.

    memcpy(m_cells, cells.data(), PUZZLE_SIZE * sizeof(Cell));
}

bool Puzzle::operator==(const Puzzle& other) const {
    assert(true);
    // post: return value is true iff this puzzle is identical to other.

    if (hashcode() != other.hashcode()) return false;

    for (auto i = 0; i < PUZZLE_SIZE; ++i) {
        if (m_cells[i] != other.m_cells[i]) return false;
    }

    return true;
}

void Puzzle::swap(uint32_t c1, uint32_t r1, uint32_t c2, uint32_t r2) {
    assert(c1 < PUZZLE_WIDTH && c2 < PUZZLE_WIDTH);
    assert(r1 < PUZZLE_HEIGHT && r2 < PUZZLE_HEIGHT);
    // post: cells at position (c1,r1) and (c2,r2) are swapped.

    auto pos1 = c1 + r1 * PUZZLE_WIDTH;
    auto pos2 = c2 + r2 * PUZZLE_WIDTH;

    /*Cell cell1 = m_cells[pos1];
    Cell cell2 = m_cells[pos2];

    m_cells[pos1] = cell2;
    m_cells[pos2] = cell1;*/

    ::swap(m_cells[pos1], m_cells[pos2]);

    m_hash_valid = false;
}

Cell Puzzle::get(uint32_t c, uint32_t r) const {
    assert(c < PUZZLE_WIDTH && r < PUZZLE_HEIGHT);
    // post: return value is cell at (c,r).

    return m_cells[c + r * PUZZLE_WIDTH];
}

bool Puzzle::completed() const {
    assert(true);
    // post: return value is true iff the puzzle is completed (the sun can set).

    for (auto tile : SOLUTION_TILES) {
        if (m_cells[tile] != Cell::red) return false;
    }

    return true;
}

void Puzzle::move(Move move) {
    assert(can_move(move.piece, move.dir));
    // post: piece at move.piece moved in direction move.dir.

    Pos old_pos = move.piece;
    Pos new_pos = pos_move(move.piece, move.dir);
    Cell type = m_cells[old_pos.c + old_pos.r * PUZZLE_WIDTH];

    switch (type) {
        case Cell::yellow:
            swap(old_pos.c, old_pos.r, new_pos.c, new_pos.r);
            break;
        case Cell::green:
            if (move.dir == Direction::north || move.dir == Direction::south) {
                swap(old_pos.c, old_pos.r, new_pos.c, new_pos.r);
                swap(old_pos.c + 1, old_pos.r, new_pos.c + 1, new_pos.r);
            } else if (move.dir == Direction::east) {
                swap(old_pos.c, old_pos.r, new_pos.c + 1, new_pos.r);
            } else { // Direction::west
                swap(old_pos.c + 1, old_pos.r, new_pos.c, new_pos.r);
            }
            break;
        case Cell::black:
            if (move.dir == Direction::east || move.dir == Direction::west) {
                swap(old_pos.c, old_pos.r, new_pos.c, new_pos.r);
                swap(old_pos.c, old_pos.r + 1, new_pos.c, new_pos.r + 1);
            } else if (move.dir == Direction::north) {
                swap(old_pos.c, old_pos.r + 1, new_pos.c, new_pos.r);
            } else { // Direction::south
                swap(old_pos.c, old_pos.r, new_pos.c, new_pos.r + 1);
            }
            break;
        case Cell::red:
            if (move.dir == Direction::north) {
                swap(old_pos.c, old_pos.r + 1, new_pos.c, new_pos.r);
                swap(old_pos.c + 1, old_pos.r + 1, new_pos.c + 1, new_pos.r);
            } else if (move.dir == Direction::south) {
                swap(old_pos.c, old_pos.r, new_pos.c, new_pos.r + 1);
                swap(old_pos.c + 1, old_pos.r, new_pos.c + 1, new_pos.r + 1);
            } else if (move.dir == Direction::east) {
                swap(old_pos.c, old_pos.r, new_pos.c + 1, new_pos.r);
                swap(old_pos.c, old_pos.r + 1, new_pos.c + 1, new_pos.r + 1);
            } else { // Direction::west
                swap(old_pos.c + 1, old_pos.r, new_pos.c, new_pos.r);
                swap(old_pos.c + 1, old_pos.r + 1, new_pos.c, new_pos.r + 1);
            }
            break;
    }
}

vector<Move> Puzzle::get_moves() const {
    assert(true);
    // post: return value is the set of all possible valid moves.

    vector<Move> moves;

    for (auto piece : get_pieces()) {
        for (auto dir : SEARCH_DIRECTIONS) {
            if (can_move(piece, dir)) moves.push_back(Move{ piece, dir });
        }
    }

    return moves;
}

vector<Pos> Puzzle::get_pieces() const {
    assert(true);
    // post: return value is the set of all the top left cells of each piece.

    vector<Pos> pieces;
    bool taken[PUZZLE_WIDTH][PUZZLE_HEIGHT] = {};

    for (auto r = 0u; r < PUZZLE_HEIGHT; ++r) {
        for (auto c = 0u; c < PUZZLE_WIDTH; ++c) {
            if (taken[c][r]) continue;

            Cell type = m_cells[c + r * PUZZLE_WIDTH];

            if (type == Cell::empty) continue;

            if (type == Cell::black) {
                assert(r + 1 < PUZZLE_HEIGHT);

                taken[c][r + 1] = true;
            } else if (type == Cell::green) {
                assert(c + 1 < PUZZLE_WIDTH);

                taken[c + 1][r] = true;
            } else if (type == Cell::red) {
                assert(c + 1 < PUZZLE_WIDTH);
                assert(r + 1 < PUZZLE_HEIGHT);

                taken[c + 1][r] = true;
                taken[c][r + 1] = true;
                taken[c + 1][r + 1] = true;
            }

            pieces.push_back(Pos{ c, r });
        }
    }

    return pieces;
}

bool Puzzle::can_move(Pos piece, Direction dir) const {
    assert(true);
    // post: return value is true iff the piece at 'piece' can move in direction dir.

    if (!pos_can_move(piece, dir, PUZZLE_WIDTH, PUZZLE_HEIGHT)) return false;

    Cell type = m_cells[piece.c + piece.r * PUZZLE_WIDTH];

    assert(type != Cell::empty);

    int width = (type == Cell::red || type == Cell::green) ? 2 : 1;
    int height = (type == Cell::red || type == Cell::black) ? 2 : 1;
    Pos new_pos = pos_move(piece, dir);

    if (new_pos.c + width > PUZZLE_WIDTH) return false;
    if (new_pos.r + height > PUZZLE_HEIGHT) return false;

    switch (dir) {
        case Direction::north:
            for (auto i = 0; i < width; ++i) {
                if (get(new_pos.c + i, new_pos.r) != Cell::empty) return false;
            }
            break;
        case Direction::south:
            for (auto i = 0; i < width; ++i) {
                if (get(new_pos.c + i, piece.r + height) != Cell::empty) return false;
            }
            break;
        case Direction::east:
            for (auto i = 0; i < height; ++i) {
                if (get(piece.c + width, piece.r + i) != Cell::empty) return false;
            }
            break;
        case Direction::west:
            for (auto i = 0; i < height; ++i) {
                if (get(new_pos.c, piece.r + i) != Cell::empty) return false;
            }
            break;
    }

    return true;
}

uint64_t Puzzle::hashcode() const {
    assert(true);
    // post: return value is the hash code of the current puzzle state.

    // Each cell has 3 bits of information, and since the board has 20 cells, that is 60 bits of information. As this
    // just fits into a 64 bit int, it can be used as a perfect hash function for very fast lookups/compares.

    if (m_hash_valid) return m_hashcode;

    uint64_t hashcode = 0;

    for (const auto& cell : m_cells) {
        hashcode |= static_cast<uint64_t>(cell);
        hashcode <<= 3;
    }

    m_hashcode = hashcode;
    m_hash_valid = true;

    return hashcode;
}

// endregion

// region Breadth-first search

bool puzzle_present(const vector<Candidate>& c, const CandidateChecker& cm, const Puzzle& puzzle) {
    assert(true);
    // post: return value is true iff puzzle is in c.

    return cm.find(puzzle) != cm.cend();
}

void try_bfs(vector<Candidate>& c, CandidateChecker & cm, int i, Move move) {
    assert(i >= 0 && i < c.size());
    // post: move dir performed on puzzle at index i in c. Result is added to c iff it doesn't already contain it.

    Puzzle puzzle = c[i].puzzle;
    puzzle.move(move);

    if (!puzzle_present(c, cm, puzzle)) {
        cm.insert(puzzle);
        c.push_back({ puzzle, i });
    }
}

vector<Puzzle>& build_path(const vector<Candidate>& c, int i, vector<Puzzle>& path) {
    assert(true);
    // post: path contains the solution path from element c[i], if it exists.

    if (i >= 0) {
        build_path(c, c[i].parent, path);
        path.push_back(c[i].puzzle);
    }

    return path;
}

Solution solve_bfs(Puzzle start) {
    assert(true);
    // post: return value contains the result of trying to solve start using bfs.

    int i = 0;
    vector<Puzzle> path;
    vector<Candidate> c = { { start, -1 } };
    CandidateChecker cm;

    for (; i < c.size() && !c[i].puzzle.completed(); ++i) {
        Puzzle puzzle = c[i].puzzle;

        for (Move move : puzzle.get_moves()) {
            try_bfs(c, cm, i, move);
        }
    }

    return (i == c.size() ? Solution{ false } : Solution{ true, c[i].puzzle, build_path(c, i, path) });
}

// endregion

int main() {
    stringstream ss{"brrb\nbrrb\n.gg.\nbyyb\nbyyb"};
    Puzzle puzzle;
    ss >> puzzle;

    auto now = chrono::high_resolution_clock::now();

    auto res = solve_bfs(puzzle);

    auto end = chrono::high_resolution_clock::now();
    auto dif = end - now;
    auto dif_ms = chrono::duration_cast<chrono::milliseconds>(dif);

    cout << "Completed in " << dif_ms.count() << " ms" << endl;

#ifndef PROFILE_ONLY
    cout << "Found solution: " << (res.solved ? "Yes" : "No") << endl;
    if (res.solved) {
        cout << "Steps: " << res.path.size() - 1 << endl;
        cout << "\nSolution" << endl;
        cout << res.puzzle << endl;
        cout << "Path" << endl;
        cout << res;
    }
#endif

    return EXIT_SUCCESS;
}