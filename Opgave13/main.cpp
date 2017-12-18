#define TRY_ALL_THREADED

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <thread>
#include <map>

using namespace std;

// Ciske Harsema - s1010048 - Informatica

enum class cell_type {
    wall, empty, worker, box
};

enum class direction {
    north, east, south, west
};

direction SEARCH_DIRECTIONS[] = { direction::north, direction::south, direction::west, direction::east };

struct Pos {
    size_t c;
    size_t r;
};

struct Cell {
    Cell() : type{}, destination{} {}
    explicit Cell(char c);

    cell_type type;
    bool destination;

    bool operator==(const Cell& other) const { return type == other.type && destination == other.destination; }
    bool operator!=(const Cell& other) const { return !(*this == other); }
};

class Puzzle {
public:
    Puzzle() : m_cells{}, m_width{}, m_height{}, m_worker{}, m_hash_valid{}, m_hashcode{} {}
    Puzzle(size_t width, vector<Cell>&& cells);

    Cell& operator[](int cell) { return (m_hash_valid = false, m_cells[cell]); }
    Cell& operator[](Pos pos) { return (m_hash_valid = false, m_cells[pos.c + pos.r * m_width]); }
    const Cell& operator[](int cell) const { return m_cells[cell]; }
    const Cell& operator[](Pos pos) const { return m_cells[pos.c + pos.r * m_width]; }
    bool operator==(const Puzzle& other) const;
    bool operator!=(const Puzzle& other) const { return !(*this == other); }

    bool completed() const;
    bool can_move(direction dir) const;
    void move(direction dir);
    uint64_t hashcode() const;

    size_t width() const { return m_width; }
    size_t size() const { return m_cells.size(); }

private:
    vector<Cell> m_cells;
    size_t m_width;
    size_t m_height;
    Pos m_worker;
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

bool pos_can_move(const Pos& pos, direction dir, size_t width, size_t height) {
    switch (dir) {
        case direction::north:  return pos.r > 0;
        case direction::east:   return pos.c + 1 < width;
        case direction::south:  return pos.r + 1 < height;
        case direction::west:   return pos.c > 0;
    }
}

Pos pos_move(const Pos& pos, direction dir) {
    switch (dir) {
        case direction::north:  return { pos.c, pos.r - 1 };
        case direction::east:   return { pos.c + 1, pos.r };
        case direction::south:  return { pos.c, pos.r + 1 };
        case direction::west:   return { pos.c - 1, pos.r };
    }
}

// region Stream I/O

ostream& operator<<(ostream& os, const Cell& cell) {
    switch (cell.type) {
        case cell_type::wall:   return os << '*';
        case cell_type::empty:  return os << (cell.destination ? '.' : ' ');
        case cell_type::worker: return os << (cell.destination ? 'W' : 'w');
        case cell_type::box:    return os << (cell.destination ? 'B' : 'b');
    }
}

ostream& operator<<(ostream& os, const Puzzle& puzzle) {
    for (size_t i = 0; i < puzzle.size(); ++i) {
        os << puzzle[i];

        if ((i + 1) % puzzle.width() == 0) os << endl;
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
    size_t width = 0;
    vector<Cell> cells;

    while (std::getline(is, line)) {
        bool trim_end = line[line.size() - 1] == '\r';
        width = trim_end ? line.size() - 1 : line.size();
        auto first = line.cbegin();
        auto last = trim_end ? line.cend() - 1 : line.cend();
        auto result = back_inserter(cells);

        transform(first, last, result, [](const char c) -> Cell { return Cell{ c }; });
    }

    puzzle = { width, ::move(cells) };

    return is;
}

// endregion

// region Cell/Puzzle impl

Cell::Cell(char c) {
    switch (c) {
        case '*': type = cell_type::wall;   destination = false; break;
        case ' ': type = cell_type::empty;  destination = false; break;
        case '.': type = cell_type::empty;  destination = true;  break;
        case 'w': type = cell_type::worker; destination = false; break;
        case 'W': type = cell_type::worker; destination = true;  break;
        case 'b': type = cell_type::box;    destination = false; break;
        case 'B': type = cell_type::box;    destination = true;  break;
        default: throw invalid_argument{ "c" };
    }
}

Puzzle::Puzzle(size_t width, vector<Cell>&& cells) : m_cells{ cells }, m_width{ width }, m_worker{}, m_hashcode{}, m_hash_valid{} {
    m_height = m_cells.size() / m_width;

    bool found = false;
    for (size_t i = 0; i < m_cells.size() && !found; ++i) {
        if (m_cells[i].type == cell_type::worker) {
            m_worker = { i % m_width, i / m_width };

            found = true;
        }
    }
}

bool Puzzle::operator==(const Puzzle& other) const {
    if (m_width != other.m_width) return false;
    if (m_height != other.m_height) return false;
    if (m_cells.size() != other.m_cells.size()) return false;
    if (hashcode() != other.hashcode()) return false;

    for (size_t i = 0; i < m_cells.size(); ++i) {
        if (m_cells[i] != other.m_cells[i]) return false;
    }

    return true;
}

bool Puzzle::completed() const {
    return none_of(m_cells.cbegin(), m_cells.cend(), [](Cell c) { return c.type == cell_type::box && !c.destination; });
}

bool Puzzle::can_move(direction dir) const {
    if (!pos_can_move(m_worker, dir, m_width, m_height)) return false;

    Pos new_pos = pos_move(m_worker, dir);
    const Cell& cell = (*this)[new_pos];

    if (cell.type == cell_type::wall) return false;
    if (cell.type == cell_type::box) {
        if (!pos_can_move(new_pos, dir, m_width, m_height)) return false;

        Pos new_box_pos = pos_move(new_pos, dir);
        const Cell& target_cell = (*this)[new_box_pos];

        if (target_cell.type != cell_type::empty) return false;
    }

    return true;
}

void Puzzle::move(direction dir) {
    Pos new_pos = pos_move(m_worker, dir);
    Cell& cur_cell = (*this)[m_worker];
    Cell& new_cell = (*this)[new_pos];

    if (new_cell.type == cell_type::box) {
        Pos new_box_pos = pos_move(new_pos, dir);
        Cell& new_box_cell = (*this)[new_box_pos];

        new_box_cell.type = cell_type::box;
    }

    m_worker = new_pos;
    cur_cell.type = cell_type::empty;
    new_cell.type = cell_type::worker;
}

uint64_t Puzzle::hashcode() const {
    if (m_hash_valid) return m_hashcode;

    uint64_t hashcode = 0;

    for (const auto& cell : m_cells) {
        hashcode |= static_cast<uint8_t>(cell.type);
        hashcode <<= (cell.destination ? 2 : 1);
        hashcode ^= 0x356fba4336be26d2;
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

void try_bfs(vector<Candidate>& c, map<uint64_t, vector<Puzzle>>& cm, int i, direction dir) {
    Puzzle puzzle = c[i].puzzle;
    puzzle.move(dir);

    if (!puzzle_present(c, cm, puzzle)) {
        cm[puzzle.hashcode()].push_back(puzzle);
        c.push_back({ ::move(puzzle), i });
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
    vector<Candidate> c = { { ::move(start), -1 } };
    map<uint64_t, vector<Puzzle>> cm;

    for (; i < c.size() && !c[i].puzzle.completed(); ++i) {
        Puzzle puzzle = c[i].puzzle;

        for (direction dir : SEARCH_DIRECTIONS) {
            if (puzzle.can_move(dir)) try_bfs(c, cm, i, dir);
        }
    }

    return (i == c.size() ? Solution{ false } : Solution{ true, c[i].puzzle, build_path(c, i, path) });
}

// endregion

// region Depth-first search

void solve_dfs(vector<Puzzle>& p, vector<Puzzle>& b, size_t depth_limit);

void try_dfs(vector<Puzzle>& p, vector<Puzzle>& b, direction dir, size_t depth_limit) {
    Puzzle puzzle = p.back();

    puzzle.move(dir);

    if (find(p.cbegin(), p.cend(), puzzle) == p.cend()) {
        p.push_back(puzzle);

        solve_dfs(p, b, depth_limit);

        p.pop_back();
    }
}

void solve_dfs(vector<Puzzle>& p, vector<Puzzle>& b, size_t depth_limit) {
    Puzzle puzzle = p.back();

    if (puzzle.completed()) {
        if (b.empty() || p.size() < b.size()) {
            b.resize(p.size());
            copy(p.cbegin(), p.cend(), b.begin());
        }
    } else if (p.size() < depth_limit && (b.empty() || p.size() < b.size())) {
        for (direction dir : SEARCH_DIRECTIONS) {
            if (puzzle.can_move(dir)) try_dfs(p, b, dir, depth_limit);
        }
    }
}

Solution solve_dfs(Puzzle puzzle, size_t depth_limit = 32) {
    vector<Puzzle> best;
    vector<Puzzle> p = { ::move(puzzle) };

    solve_dfs(p, best, depth_limit);

    return best.empty() ? Solution{ false } : Solution{ true, best.back(), best };
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

void solve_bfs_thr(Puzzle puzzle, Solution& bfs) {
    bfs = solve_bfs(::move(puzzle));
}

void solve_dfs_thr(Puzzle puzzle, Solution& dfs, size_t max_depth) {
    dfs = solve_dfs(::move(puzzle), max_depth);
}

thread dispatch_bfs(const Puzzle& puzzle, Solution& bfs) {
    return thread{ solve_bfs_thr, puzzle, ref(bfs) };
}

thread dispatch_dfs(const Puzzle& puzzle, Solution& dfs, size_t max_depth) {
    return thread{ solve_dfs_thr, puzzle, ref(dfs), max_depth };
}

void dispatch(const Puzzle& puzzle, Solution& bfs, Solution& dfs, size_t max_depth, vector<thread>& threads) {
    threads.push_back(dispatch_bfs(puzzle, bfs));
    threads.push_back(dispatch_dfs(puzzle, dfs, max_depth));
}

constexpr size_t CHALLENGES = 5;
size_t MAX_DEPTH[] = { 35, 0, 24, 32, 26 }; // BFS results: 35, 35, 24, 32, 26
// NOTE: DFS for challenge 1 disabled as it takes an obscene amount of time (but will eventually solve correctly).
// NOTE: While the BFS threads finish relatively quickly, they can peak out memory consumption >700MB.

int main() {
    cout << "Loading challenges..." << endl;

    auto puzzles = load_puzzles(CHALLENGES);
#ifdef TRY_ALL_THREADED
    Solution bfs_solutions[CHALLENGES];
    Solution dfs_solutions[CHALLENGES];

    vector<thread> threads;

    cout << "Spawning threads..." << endl;

    for (size_t i = 0; i < CHALLENGES; ++i) {
        dispatch(puzzles[i], bfs_solutions[i], dfs_solutions[i], MAX_DEPTH[i], threads);
    }

    cout << "Waiting on threads..." << endl;

    for (auto& thr : threads) { thr.join(); }

    for (size_t i = 0; i < CHALLENGES; ++i) {
        auto bfs = bfs_solutions[i];
        auto dfs = dfs_solutions[i];

        cout << "[Challenge " << i << ']' << endl;
        cout << "bfs solved? " << (bfs.solved ? "yes" : "no") << endl;
        if (bfs.solved) {
            cout << "steps: " << bfs.path.size() << endl;
            cout << "solution" << endl;
            cout << bfs.puzzle;
        }
        cout << endl;
        cout << "dfs solved? " << (dfs.solved ? "yes" : "no") << endl;
        if (dfs.solved) {
            cout << "steps: " << dfs.path.size() << endl;
            cout << "solution" << endl;
            cout << dfs.puzzle;
        }
        cout << endl;
    }
#else
    auto res = solve_bfs(puzzles[0]);

    cout << "Found solution: " << (res.solved ? "Yes" : "No") << endl;
    if (res.solved) {
        cout << "Steps: " << res.path.size() << endl;
        cout << "\nSolution" << endl;
        cout << res.puzzle << endl;
        cout << "Path" << endl;
        cout << res;
    }
#endif

    return EXIT_SUCCESS;
}