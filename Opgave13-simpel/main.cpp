#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <cassert>

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
    cell_type type;
    bool destination;
};

struct Puzzle {
    vector<Cell> cells;
    Pos worker;
    size_t width;
    size_t height;
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

// region Pos//Cell/Puzzle impl

bool pos_can_move(const Pos& pos, direction dir, size_t width, size_t height) {
    assert(true);
    // post: return value is true iff pos can move in dir and stay within bounds 0, 0, width, height.

    switch (dir) {
        case direction::north:  return pos.r > 0;
        case direction::east:   return pos.c + 1 < width;
        case direction::south:  return pos.r + 1 < height;
        case direction::west:   return pos.c > 0;
    }
}

Pos pos_move(const Pos& pos, direction dir) {
    assert(!(pos.c == 0 && dir == direction::west));
    assert(!(pos.r == 0 && dir == direction::north));
    // post: return value is pos moved in dir.

    switch (dir) {
        case direction::north:  return { pos.c, pos.r - 1 };
        case direction::east:   return { pos.c + 1, pos.r };
        case direction::south:  return { pos.c, pos.r + 1 };
        case direction::west:   return { pos.c - 1, pos.r };
    }
}

// Define comparison operators, so that Pos structs can be compared using == and !=
bool operator==(const Pos& left, const Pos& right) {
    return left.c == right.c && left.r == right.r;
}

bool operator!=(const Pos& left, const Pos& right) {
    return !(left == right);
}

Cell cell_from_char(char c) {
    assert(c == '*' || c == ' ' || c == '.' || c == 'w' || c == 'W' || c == 'b' || c == 'B');
    // post: return value is a Cell of the correct type and destination.

    Cell cell;

    switch (c) {
        case '*': cell.type = cell_type::wall;   cell.destination = false; break;
        case ' ': cell.type = cell_type::empty;  cell.destination = false; break;
        case '.': cell.type = cell_type::empty;  cell.destination = true;  break;
        case 'w': cell.type = cell_type::worker; cell.destination = false; break;
        case 'W': cell.type = cell_type::worker; cell.destination = true;  break;
        case 'b': cell.type = cell_type::box;    cell.destination = false; break;
        case 'B': cell.type = cell_type::box;    cell.destination = true;  break;
    }

    return cell;
}

// Define comparison operators, so that Cell structs can be compared using == and !=
bool operator==(const Cell& left, const Cell& right) {
    return left.type == right.type && left.destination == right.destination;
}

bool operator!=(const Cell& left, const Cell& right) {
    return !(left == right);
}

Puzzle make_puzzle(size_t height, vector<Cell> cells) {
    assert(height != 0);
    // post: Puzzle constructed from the given cells.

    Puzzle puzzle;
    puzzle.cells = cells;
    puzzle.height = height;
    puzzle.width = cells.size() / height;

    // Find the position of the worker.
    bool found = false;
    for (size_t i = 0; i < cells.size() && !found; ++i) {
        if (cells[i].type == cell_type::worker) {
            puzzle.worker.c = i % puzzle.width;
            puzzle.worker.r = i / puzzle.width;
            found = true;
        }
    }

    return puzzle;
}

// Define comparison operators, so that Puzzle structs can be compared using == and !=
bool operator==(const Puzzle& left, const Puzzle& right) {
    assert(true);
    // post: return value is true iff this puzzle is identical to other.

    // Check if the position of the workers is equal. Not really needed, as the loop below will do the same, but this
    // method is performance critical (because it is called very often). Comparing the workers is a fast way to see
    // two puzzles cannot be the same, without having to compare each cell.
    if (left.worker != right.worker) return false;

    for (size_t i = 0; i < left.cells.size(); ++i) {
        if (left.cells[i] != right.cells[i]) return false;
    }

    return true;
}

bool operator!=(const Puzzle& left, const Puzzle& right) {
    return !(left == right);
}

bool puzzle_completed(const Puzzle& puzzle) {
    assert(true);
    // post: return value is true iff this puzzle is completed (all boxes on a destination cell).

    // Loop through all the cells in the puzzle, and see if each box is on a destination cell.
    for(Cell cell : puzzle.cells) {
        if(cell.type == cell_type::box && !cell.destination) return false;
    }

    return true;
}

Cell puzzle_at(const Puzzle& puzzle, Pos pos) {
    // return the cell at position 'pos' in puzzle.
    return puzzle.cells[pos.c + pos.r * puzzle.width];
}

void puzzle_set_type_at(Puzzle& puzzle, cell_type type, Pos pos) {
    // set the cell type at position 'pos' in puzzle.
    puzzle.cells[pos.c + pos.r * puzzle.width].type = type;
}

bool puzzle_can_move(const Puzzle& puzzle, direction dir) {
    assert(true);
    // post: return value is true iff worker can move in direction dir.

    // Check if the worker can move in the requested direction.
    if (!pos_can_move(puzzle.worker, dir, puzzle.width, puzzle.height)) return false;

    // Compute the position the worker wants to move to, and get the cell at that position.
    Pos new_pos = pos_move(puzzle.worker, dir);
    Cell cell = puzzle_at(puzzle, new_pos);

    if (cell.type == cell_type::wall) return false; // Worker cannot walk into a wall
    if (cell.type == cell_type::box) { // Check if the worker is walking into a box, and thus tries to push it.
        // Check if the box can be pushed in the same direction as the worker, while staying in the puzzle.
        if (!pos_can_move(new_pos, dir, puzzle.width, puzzle.height)) return false;

        // If the box can move, check if the place it is moved to is empty (as the worker cannot push multiple boxes, or
        // push boxes through walls etc).
        Pos new_box_pos = pos_move(new_pos, dir);
        Cell target_cell = puzzle_at(puzzle, new_box_pos);

        if (target_cell.type != cell_type::empty) return false;
    }

    return true;
}

void puzzle_move(Puzzle& puzzle, direction dir) {
    assert(puzzle_can_move(puzzle, dir));
    // post: worker moved in direction dir.

    // Compute the position the worker wants to move to, and get the cell at that position.
    Pos new_pos = pos_move(puzzle.worker, dir);
    Cell new_cell = puzzle_at(puzzle, new_pos);

    // If there is a box at the new position, it is pushed.
    if (new_cell.type == cell_type::box) {
        Pos new_box_pos = pos_move(new_pos, dir);

        puzzle_set_type_at(puzzle, cell_type::box, new_box_pos);
    }

    // Move the worker to the new position.
    puzzle_set_type_at(puzzle, cell_type::empty, puzzle.worker);
    puzzle_set_type_at(puzzle, cell_type::worker, new_pos);
    puzzle.worker = new_pos;
}

// endregion

// region Stream I/O

ostream& operator<<(ostream& os, const Cell& cell) {
    assert(true);
    // post: cell is printed to os.

    switch (cell.type) {
        case cell_type::wall:   return os << '*';
        case cell_type::empty:  return os << (cell.destination ? '.' : ' ');
        case cell_type::worker: return os << (cell.destination ? 'W' : 'w');
        case cell_type::box:    return os << (cell.destination ? 'B' : 'b');
    }
}

ostream& operator<<(ostream& os, const Puzzle& puzzle) {
    assert(true);
    // post: puzzle is printed to os.

    // Print all cells to the console.
    for (size_t i = 0; i < puzzle.cells.size(); ++i) {
        os << puzzle.cells[i];

        // Print end of line each time width cells have been printed.
        if ((i + 1) % puzzle.width == 0) os << endl;
    }

    return os;
}

ostream& operator<<(ostream& os, const Solution& solution) {
    assert(true);
    // post: each node in the solution path is printed to os.

    for (const Puzzle& node : solution.path) {
        os << node << endl;
    }

    return os;
}

istream& operator>>(istream& is, Puzzle& puzzle) {
    assert(true);
    // post: puzzle contains the puzzle loaded from is.

    string line;
    size_t lines = 0;
    vector<Cell> cells;

    // Loop through each line in the input stream.
    while (std::getline(is, line)) {
        if(line.empty() || line == "\r") continue; // Ignore empty lines

        lines += 1;

        // Loop through each character in the line.
        for(char c : line) {
            if(c == '\r') continue; // Fix for reading Windows line endings on a Unix platform.

            Cell cell = cell_from_char(c);
            cells.push_back(cell);
        }
    }

    puzzle = make_puzzle(lines, cells);

    return is;
}

// endregion

// region Breadth-first search

bool puzzle_present(const vector<Candidate>& c, const Puzzle& puzzle) {
    assert(true);
    // post: return value is true iff puzzle is in c.

    // NOTE: While this is the 'easy' way to check if puzzle is in c, it is also incredibly slow as c can be very, very
    // large. The main reason bread-first search is so slow is because of this loop here. You really want something that
    // searches through this list with logarithmic/constant time complexity (O(log n) or O(1)), rather than linear time
    // complexity (O(n)) like this code does. This can be done with hashing and/or binary trees (like I did in my
    // solution) for example.
    for(const Candidate& candidate : c) {
        if(candidate.puzzle == puzzle) return true;
    }

    return false;
}

void try_bfs(vector<Candidate>& c, int i, direction dir) {
    assert(i >= 0 && i < c.size());
    // post: move dir performed on puzzle at index i in c. Result is added to c iff it doesn't already contain it.

    // Perform the move on the i-th candidate.
    Puzzle puzzle = c[i].puzzle;
    puzzle_move(puzzle, dir);

    // Only add the new puzzle to the candidate list if it is not yet on there (to prevent endless loops).
    if (!puzzle_present(c, puzzle)) {
        c.push_back({ puzzle, i });
    }
}

vector<Puzzle>& build_path(const vector<Candidate>& c, int i, vector<Puzzle>& path) {
    assert(true);
    // post: path contains the solution path from element c[i], if it exists.

    if (i >= 0) {
        // First build the path how we got to the i-th candidate from the start.
        build_path(c, c[i].parent, path);

        // Then add the i-th candidate to the end.
        path.push_back(c[i].puzzle);
    }

    return path;
}

Solution solve_bfs(Puzzle start) {
    assert(true);
    // post: return value contains the result of trying to solve start using bfs.

    int i = 0;
    vector<Puzzle> path;
    vector<Candidate> c = { { start, -1 } }; // Initial candidate is the starting puzzle, which has no parent.
    Solution solution;

    // Keep trying candidates until the list is depleted, or a solution is found.
    for (; i < c.size() && !puzzle_completed(c[i].puzzle); ++i) {
        Puzzle puzzle = c[i].puzzle;

        // Try to move the worker of the current candidate in each direction (if possible).
        for (direction dir : SEARCH_DIRECTIONS) {
            if (puzzle_can_move(puzzle, dir)) try_bfs(c, i, dir);
        }
    }

    // If the candidate list wasn't completely depleted, a solution was found.
    solution.solved = i < c.size();
    if(solution.solved) {
        // Grab the solved state, and build the path from the starting puzzle that lead to the solution.
        solution.puzzle = c[i].puzzle;
        solution.path = build_path(c, i, path);
    }

    return solution;
}

// endregion

// region Depth-first search

void solve_dfs(vector<Puzzle>& p, vector<Puzzle>& b, size_t depth_limit);

bool has_puzzle(const vector<Puzzle>& p, const Puzzle& puzzle) {
    // Check if p contains puzzle.
    for(const Puzzle& element : p) {
        if(element == puzzle) return true;
    }

    return false;
}

void try_dfs(vector<Puzzle>& p, vector<Puzzle>& b, direction dir, size_t depth_limit) {
    assert(!p.empty());
    // post: performed move dir on the last element in p, and added the result to p iff it wasn't present. If it was
    // added, then perform a dfs from that new node.

    // Get the puzzle with the highest depth in the current tree path (last element in the vector).
    Puzzle puzzle = p.back();

    // Perform the move on the puzzle.
    puzzle_move(puzzle, dir);

    // Check if the resulting puzzle is already in the path (prevent cycles in the tree path, which would cause
    // infinite loops).
    if (!has_puzzle(p, puzzle)) {
        // Resulting puzzle wasn't in the path, so add it as the new deepest tree node.
        p.push_back(puzzle);

        // Try to solve from the node just added.
        solve_dfs(p, b, depth_limit);

        // After having tried each move from the node, remove it again (walk one node back up the tree).
        p.pop_back();
    }
}

void solve_dfs(vector<Puzzle>& p, vector<Puzzle>& b, size_t depth_limit) {
    assert(!p.empty());
    // post: p is copied to b iff p is solved with a shorter path than b. If p was not solved, then each possible move
    // was tested with dfs, from the last node in p.

    // Get the puzzle with the highest depth in the current tree path (last element in the vector).
    Puzzle puzzle = p.back();
    // Check if this puzzle could potentially be a better solution than the current solution (either current solution is
    // empty (thus does not exist), or this potential solution has a lower depth).
    bool better_potential_solution = b.empty() || p.size() < b.size();

    if (puzzle_completed(puzzle)) {
        if (better_potential_solution) {
            // If the last node in the path is indeed a solution, and the path to that solution is better/shorter than
            // the best known path, update the best known path to equal the current path.
            b = p;
        }
        // Only try to continue searching as long as the depth limit isn't reached, and the path could potentially be
        // better than the best known path.
    } else if (p.size() < depth_limit && better_potential_solution) {
        // Try to move the worker of the current puzzle in each direction (if possible), and continue the search from
        // there.
        for (direction dir : SEARCH_DIRECTIONS) {
            if (puzzle_can_move(puzzle, dir)) try_dfs(p, b, dir, depth_limit);
        }
    }
}

Solution solve_dfs(Puzzle puzzle, size_t depth_limit = 32) {
    assert(true);
    // post: return value is the result of trying to solve puzzle with dfs, for a given depth limit.

    vector<Puzzle> best;
    vector<Puzzle> p = { puzzle }; // Root of the search tree is the starting puzzle state.
    Solution solution;

    // Try to solve from the root node, and store the best found solution path in best.
    solve_dfs(p, best, depth_limit);

    solution.solved = !best.empty(); // If path isn't empty, then a solution was found.
    if(solution.solved) {
        // Last element of the solution path (deepest node) is the actual solution state. The path in best goes from the
        // tree root, to the solution.
        solution.puzzle = best.back();
        solution.path = best;
    }

    return solution;
}

// endregion

int main() {
    ifstream is{"../challenge1.txt"};
    Puzzle puzzle;
    is >> puzzle;

    Solution solution = solve_bfs(puzzle);
    //Solution solution = solve_dfs(puzzle, 35);

    cout << "Found solution: " << (solution.solved ? "Yes" : "No") << endl;
    if (solution.solved) {
        cout << "Steps: " << solution.path.size() - 1 << endl;
        cout << "\nSolution" << endl;
        cout << solution.puzzle << endl;
        cout << "Path" << endl;
        cout << solution;
    }

    return EXIT_SUCCESS;
}