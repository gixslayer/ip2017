#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <cassert>
#include <cstring>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

constexpr int PUZZLE_WIDTH = 4;
constexpr int PUZZLE_HEIGHT = 5;
constexpr int PUZZLE_SIZE = PUZZLE_WIDTH * PUZZLE_HEIGHT;
constexpr int EMPTY_TILES = 2;
constexpr int SOLUTION_TILES[] = { 13, 14, 17, 18 };

enum class Cell {
    black, red, green, yellow, empty
};

enum class Direction {
    north, east, south, west
};

Direction SEARCH_DIRECTIONS[] = { Direction::north, Direction::south, Direction::east, Direction::west };

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
    Puzzle() : m_cells{}, m_empty{}, m_hash_valid{}, m_hashcode{} {}
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
    Pos m_empty[EMPTY_TILES];
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

bool pos_can_move(const Pos& pos, Direction dir, size_t width, size_t height) {
    switch (dir) {
        case Direction::north:  return pos.r > 0;
        case Direction::east:   return pos.c + 1 < width;
        case Direction::south:  return pos.r + 1 < height;
        case Direction::west:   return pos.c > 0;
    }
}

Pos pos_move(const Pos& pos, Direction dir) {
    switch (dir) {
        case Direction::north:  return { pos.c, pos.r - 1 };
        case Direction::east:   return { pos.c + 1, pos.r };
        case Direction::south:  return { pos.c, pos.r + 1 };
        case Direction::west:   return { pos.c - 1, pos.r };
    }
}

Cell parse_cell(char c) {
    switch (c) {
        case 'b': return Cell::black;
        case '.': return Cell::empty;
        case 'g': return Cell::green;
        case 'r': return Cell::red;
        case 'y': return Cell::yellow;
        default: throw invalid_argument{ "c" };
    }
}

// region Stream I/O

ostream& operator<<(ostream& os, const Cell& cell) {
    switch (cell) {
        case Cell::empty:   return os << '.';
        case Cell::black:   return os << 'b';
        case Cell::green:   return os << 'g';
        case Cell::red:     return os << 'r';
        case Cell::yellow:  return os << 'y';
    }
}

ostream& operator<<(ostream& os, const Puzzle& puzzle) {
    for (auto i = 0; i < PUZZLE_SIZE; ++i) {
        os << puzzle[i];

        if ((i + 1) % PUZZLE_WIDTH == 0) os << endl;
    }

    return os;
}

ostream& operator<<(ostream& os, const Solution& solution) {
    for (const auto& node : solution.path) {
        os << node << endl;
    }

    return os;
}

istream& operator>>(istream& is, Puzzle& puzzle) {
    string line;
    vector<Cell> cells;

    while (std::getline(is, line)) {
        assert(line.size() >= PUZZLE_WIDTH);

        auto first = line.cbegin();
        auto last = first + PUZZLE_WIDTH;
        auto result = back_inserter(cells);

        transform(first, last, result, parse_cell);
    }

    puzzle = Puzzle{ cells };

    return is;
}

// endregion

// region Puzzle impl

Puzzle::Puzzle(const vector<Cell>& cells) : m_hashcode{}, m_hash_valid{} {
    assert(cells.size() == PUZZLE_SIZE);

    memcpy(m_cells, cells.data(), PUZZLE_SIZE * sizeof(Cell));
    auto empty_count = 0u;

    for (auto i = 0u; i < PUZZLE_SIZE && empty_count < EMPTY_TILES; ++i) {
        if (m_cells[i] == Cell::empty) {
            m_empty[empty_count++] = { i % PUZZLE_WIDTH, i / PUZZLE_WIDTH };
        }
    }

    assert(empty_count == EMPTY_TILES);
}

bool Puzzle::operator==(const Puzzle& other) const {
    if (hashcode() != other.hashcode()) return false;

    for (auto i = 0; i < PUZZLE_SIZE; ++i) {
        if (m_cells[i] != other.m_cells[i]) return false;
    }

    return true;
}

void Puzzle::swap(uint32_t c1, uint32_t r1, uint32_t c2, uint32_t r2) {
    Cell cell1 = m_cells[c1 + r1 * PUZZLE_WIDTH];
    Cell cell2 = m_cells[c2 + r2 * PUZZLE_WIDTH];

    m_cells[c1 + r1 * PUZZLE_WIDTH] = cell2;
    m_cells[c2 + r2 * PUZZLE_WIDTH] = cell1;
    m_hash_valid = false;
}

Cell Puzzle::get(uint32_t c, uint32_t r) const {
    return m_cells[c + r * PUZZLE_WIDTH];
}

bool Puzzle::completed() const {
    for (auto tile : SOLUTION_TILES) {
        if (m_cells[tile] != Cell::red) return false;
    }

    return true;
}

void Puzzle::move(Move move) {
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
    vector<Move> moves;

    for (auto piece : get_pieces()) {
        for (auto dir : SEARCH_DIRECTIONS) {
            if (can_move(piece, dir)) moves.push_back(Move{ piece, dir });
        }
    }

    return moves;
}

vector<Pos> Puzzle::get_pieces() const {
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
    if (!pos_can_move(piece, dir, PUZZLE_WIDTH, PUZZLE_HEIGHT)) return false;

    Cell type = m_cells[piece.c + piece.r * PUZZLE_WIDTH];
    Pos new_pos = pos_move(piece, dir);
    int width = (type == Cell::red || type == Cell::green) ? 2 : 1;
    int height = (type == Cell::red || type == Cell::black) ? 2 : 1;

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
    if (m_hash_valid) return m_hashcode;

    uint64_t hashcode = 0;

    for (const auto& cell : m_cells) {
        hashcode |= static_cast<uint8_t>(cell);
        hashcode <<= 3;
    }

    m_hashcode = hashcode;
    m_hash_valid = true;

    return hashcode;
}

// endregion

// region Breadth-first search

bool puzzle_present(const vector<Candidate>& c, map<uint64_t, vector<Puzzle>>& cm, const Puzzle& puzzle) {
    auto entry = cm.find(puzzle.hashcode());

    if (entry == cm.end()) return false;

    return any_of(entry->second.cbegin(), entry->second.cend(), [&puzzle](const Puzzle& p) { return p == puzzle; });
}

void try_bfs(vector<Candidate>& c, map<uint64_t, vector<Puzzle>>& cm, int i, Move move) {
    Puzzle puzzle = c[i].puzzle;
    puzzle.move(move);

    if (!puzzle_present(c, cm, puzzle)) {
        cm[puzzle.hashcode()].push_back(puzzle);
        c.push_back({ puzzle, i });
    }
}

vector<Puzzle>& build_path(const vector<Candidate>& c, int i, vector<Puzzle>& path) {
    if (i >= 0) {
        build_path(c, c[i].parent, path);
        path.push_back(c[i].puzzle);
    }

    return path;
}

Solution solve_bfs(Puzzle start) {
    int i = 0;
    vector<Puzzle> path;
    vector<Candidate> c = { { start, -1 } };
    map<uint64_t, vector<Puzzle>> cm;

    for (; i < c.size() && !c[i].puzzle.completed(); ++i) {
        Puzzle puzzle = c[i].puzzle;

        for (Move move : puzzle.get_moves()) {
            try_bfs(c, cm, i, move);
        }
    }

    return (i == c.size() ? Solution{ false } : Solution{ true, c[i].puzzle, build_path(c, i, path) });
}

// endregion

vector<Puzzle> load_puzzles(size_t count) {
    vector<Puzzle> puzzles;

    for (size_t i = 0; i < count; ++i) {
        stringstream name;
        Puzzle puzzle;

        name << "../challenge" << i << ".txt";

        ifstream is{ name.str() };

        if (!is) {
            cerr << "Failed to open challenge " << i << endl;

            exit(EXIT_FAILURE);
        }

        is >> puzzle;
        puzzles.push_back(puzzle);
    }

    return puzzles;
}

int main() {
    cout << "Loading challenges..." << endl;

    auto puzzles = load_puzzles(1);

    auto res = solve_bfs(puzzles[0]);

    cout << "Found solution: " << (res.solved ? "Yes" : "No") << endl;
    if (res.solved) {
        cout << "Steps: " << res.path.size() << endl;
        cout << "\nSolution" << endl;
        cout << res.puzzle << endl;
        cout << "Path" << endl;
        cout << res;
    }

    return EXIT_SUCCESS;
}