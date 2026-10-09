#pragma once
#include <cstddef>
#include <iosfwd>
#include <vector>

namespace ppois {
/** Two-player square board. X starts; winning line spans the whole board. */
class TicTacToe {
public:
    static constexpr std::size_t default_size = 3;
    /** Practical allocation limit; sizes 1..1000 are supported. */
    static constexpr std::size_t max_size = 1000;
    /** Cell proxy keeps operator[] assignment subject to game rules. */
    class Cell {
    public:
        Cell(TicTacToe& game, std::size_t index);
        operator char() const;
        Cell& operator=(char mark);
        Cell& operator=(const Cell& other);
    private:
        TicTacToe& game_;
        std::size_t index_;
    };
    explicit TicTacToe(std::size_t size = default_size);
    TicTacToe(const TicTacToe&) = default;
    TicTacToe& operator=(const TicTacToe&) = default;
    ~TicTacToe() = default;
    std::size_t size() const;
    char current_player() const;
    /** Returns false for occupied/out-of-range cells or finished games. */
    bool can_place(std::size_t row, std::size_t column) const;
    void place(std::size_t row, std::size_t column, char mark);
    /** Flat zero-based index: row * size + column. */
    Cell operator[](std::size_t index);
    char operator[](std::size_t index) const;
    /** X, O, or '.' when no player has won. */
    char winner() const;
    bool draw() const;
    bool finished() const;
    void reset();
    bool operator==(const TicTacToe& other) const;
    bool operator!=(const TicTacToe& other) const;
    /** Lossless format: size, move count, then flat move indices in order. */
    friend std::ostream& operator<<(std::ostream& out, const TicTacToe& game);
    /** Replays moves; invalid input sets failbit and preserves the object. */
    friend std::istream& operator>>(std::istream& in, TicTacToe& game);
private:
    std::size_t size_;
    std::vector<char> board_;
    std::vector<std::size_t> moves_;
    char current_ = 'X';
    char winner_ = '.';
    void check_index(std::size_t index) const;
    bool winning_move(std::size_t row, std::size_t column, char mark) const;
    bool line_matches(std::size_t start, std::size_t step, char mark) const;
};
} // namespace ppois
