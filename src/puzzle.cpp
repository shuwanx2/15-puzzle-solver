/**
 * @file puzzle.cpp
 * Implementation of the PuzzleState class and puzzle solvers.
 */

#include "puzzle.h"
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <functional>
#include <unordered_set>

// my default constructor: initializes the puzzle to the solved state
PuzzleState::PuzzleState()
    : state_({1, 2, 3, 4,
              5, 6, 7, 8,
              9, 10, 11, 12,
              13, 14, 15, 0}) {
    emptyIndex = 0;
}

// constructs a PuzzleState from a given array representation
PuzzleState::PuzzleState(const std::array<uint8_t, 16> state) {
    state_ = state;
}

// checks whether a puzzle state is valid
// make sure 1 - 15 must appear only once
bool PuzzleState::isValidState(const std::array<uint8_t, 16>& state) const {
    std::array<uint8_t, 16> count = {0}; // frequency counter

    for (int num : state) {
        if (num > 15) {
            return false; // value out of valid range
        }
        count[num]++;
    }

    for (int c : count) {
        if (c != 1) {
            return false; // duplicate or missing value
        }
    }
    return true;
}

// Returns the internal state as an array
std::array<uint8_t, 16> PuzzleState::asArray() const {
    return state_;
}

// Equality operator
bool PuzzleState::operator==(const PuzzleState &rhs) const {
    return state_ == rhs.state_;
}

// Inequality operator
bool PuzzleState::operator!=(const PuzzleState &rhs) const {
    return !(*this == rhs);
}

// Less-than operator to support ordered containers (e.g., std::map)
bool PuzzleState::operator<(const PuzzleState &rhs) const {
    return state_ < rhs.state_;
}

// Returns the neighboring state in a given direction
// if move is invalid - returns the current state
PuzzleState PuzzleState::getNeighbor(Direction direction) const {
    int emptyIndex = std::find(state_.begin(), state_.end(), 0) - state_.begin();
    int row = emptyIndex / 4;
    int col = emptyIndex % 4;
    int newRow = row, newCol = col;

    switch (direction) {
        case Direction::UP:
            if (row == 3) return *this;
            newRow = row + 1;
            break;
        case Direction::DOWN:
            if (row == 0) return *this;
            newRow = row - 1;
            break;
        case Direction::LEFT:
            if (col == 3) return *this;
            newCol = col + 1;
            break;
        case Direction::RIGHT:
            if (col == 0) return *this;
            newCol = col - 1;
            break;
    }

    std::array<uint8_t, 16> newState = state_;
    std::swap(newState[emptyIndex], newState[newRow * 4 + newCol]);

    return PuzzleState(newState);
}

// returns all valid neighboring states
std::vector<PuzzleState> PuzzleState::getNeighbors() const {
    std::vector<PuzzleState> neighbors;

    for (Direction dir : {Direction::UP, Direction::DOWN,
                          Direction::LEFT, Direction::RIGHT}) {
        PuzzleState neighbor = getNeighbor(dir);
        if (neighbor != *this) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

// computes the Manhattan distance from the current state to the desired state
int PuzzleState::manhattanDistance(const PuzzleState desiredState) const {
    int distance = 0;

    for (int i = 0; i < 16; ++i) {
        uint8_t tile = state_[i];
        if (tile != 0) {
            int goalPos = std::distance(
                desiredState.state_.begin(),
                std::find(desiredState.state_.begin(),
                          desiredState.state_.end(), tile));

            int currentRow = i / 4;
            int currentCol = i % 4;
            int goalRow = goalPos / 4;
            int goalCol = goalPos % 4;

            distance += std::abs(currentRow - goalRow) +
                        std::abs(currentCol - goalCol);
        }
    }
    return distance;
}

// custom hash function for PuzzleState to allow usage in unordered containers
namespace std {
    template <>
    struct hash<PuzzleState> {
        size_t operator()(const PuzzleState& state) const {
            const auto& arr = state.asArray();
            size_t hashValue = 0;
            for (auto& num : arr) {
                hashValue = hashValue * 31 + num;
            }
            return hashValue;
        }
    };
}

// A* search algorithm using manhattan distance as the heuristic
std::vector<PuzzleState> solveAstar(const PuzzleState& startState,
                                    const PuzzleState& desiredState,
                                    size_t *iterations) {

    // priority queue storing (priority, state)
    std::priority_queue<
        std::pair<int, PuzzleState>,
        std::vector<std::pair<int, PuzzleState>>,
        std::greater<std::pair<int, PuzzleState>>> frontier;

    // map each state to its predecessor
    std::unordered_map<PuzzleState, PuzzleState> cameFrom;

    // save cost to reach each state
    std::unordered_map<PuzzleState, int> costSoFar;

    frontier.emplace(0, startState);
    cameFrom[startState] = startState;
    costSoFar[startState] = 0;
    *iterations = 0;

    while (!frontier.empty()) {
        auto current = frontier.top();
        frontier.pop();
        PuzzleState currentState = current.second;

        (*iterations)++;

        if (currentState == desiredState) {
            break;
        }

        for (PuzzleState next : currentState.getNeighbors()) {
            int newCost = costSoFar[currentState] + 1;

            if (!costSoFar.count(next) || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                int priority =
                    newCost + next.manhattanDistance(desiredState);
                frontier.emplace(priority, next);
                cameFrom[next] = currentState;
            }
        }
    }

    // reconstruct path from goal to start
    std::vector<PuzzleState> path;
    if (cameFrom.find(desiredState) != cameFrom.end()) {
        for (PuzzleState state = desiredState;
             state != startState;
             state = cameFrom[state]) {
            path.push_back(state);
        }
        path.push_back(startState);
        std::reverse(path.begin(), path.end());
    }

    return path;
}

// Breadth-First Search (BFS) algorithm
std::vector<PuzzleState> solveBFS(const PuzzleState& startState,
                                  const PuzzleState& desiredState,
                                  size_t *iterations) {

    std::queue<PuzzleState> frontier;
    std::unordered_set<PuzzleState> visited;
    std::unordered_map<PuzzleState, PuzzleState> cameFrom;

    frontier.push(startState);
    visited.insert(startState);
    cameFrom[startState] = startState;
    *iterations = 0;

    while (!frontier.empty()) {
        PuzzleState current = frontier.front();
        frontier.pop();
        (*iterations)++;

        if (current == desiredState) {
            std::vector<PuzzleState> path;
            for (PuzzleState state = desiredState;
                 state != startState;
                 state = cameFrom[state]) {
                path.push_back(state);
            }
            path.push_back(startState);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (PuzzleState next : current.getNeighbors()) {
            if (visited.count(next) == 0) {
                frontier.push(next);
                visited.insert(next);
                cameFrom[next] = current;
            }
        }
    }

    // Return empty path if no solution is found
    return std::vector<PuzzleState>();
}
