#include "TicTacToe.hpp"
#include <algorithm>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace ppois {
TicTacToe::TicTacToe(std::size_t size) : size_(size) {
    if (size == 0 || size > max_size) throw std::invalid_argument("Invalid board size");
    board_.assign(size * size, '.');
}
std::size_t TicTacToe::size() const { return size_; }
char TicTacToe::current_player() const { return current_; }
void TicTacToe::check_index(std::size_t index) const {
    if (index >= board_.size()) throw std::out_of_range("Cell is outside the board");
}
bool TicTacToe::can_place(std::size_t row, std::size_t column) const {
    return row < size_ && column < size_ && !finished() && board_[row * size_ + column] == '.';
}
void TicTacToe::place(std::size_t row, std::size_t column, char mark) {
    if (row >= size_ || column >= size_) throw std::out_of_range("Cell is outside the board");
    if (mark != current_) throw std::invalid_argument("Wrong player's turn");
    if (!can_place(row, column)) throw std::logic_error("Move is not allowed");
    const auto index = row * size_ + column;
    moves_.push_back(index);
    board_[index] = mark;
    if (winning_move(row, column, mark)) winner_ = mark;
    current_ = current_ == 'X' ? 'O' : 'X';
}
bool TicTacToe::line_matches(std::size_t start, std::size_t step, char mark) const {
    for (std::size_t offset = 0; offset < size_; ++offset)
        if (board_[start + offset * step] != mark) return false;
    return true;
}
bool TicTacToe::winning_move(std::size_t row, std::size_t column, char mark) const {
    return line_matches(row * size_, 1, mark) || line_matches(column, size_, mark)
        || (row == column && line_matches(0, size_ + 1, mark))
        || (row + column == size_ - 1 && line_matches(size_ - 1, size_ - 1, mark));
}
TicTacToe::Cell::Cell(TicTacToe& game, std::size_t index) : game_(game), index_(index) {}
TicTacToe::Cell::operator char() const {
    game_.check_index(index_);
    return game_.board_[index_];
}
TicTacToe::Cell& TicTacToe::Cell::operator=(char mark) {
    game_.place(index_ / game_.size_, index_ % game_.size_, mark);
    return *this;
}
TicTacToe::Cell& TicTacToe::Cell::operator=(const Cell& other) {
    return *this = static_cast<char>(other);
}
TicTacToe::Cell TicTacToe::operator[](std::size_t index) {
    check_index(index);
    return Cell(*this, index);
}
char TicTacToe::operator[](std::size_t index) const {
    check_index(index);
    return board_[index];
}
char TicTacToe::winner() const { return winner_; }
bool TicTacToe::draw() const { return winner_ == '.' && moves_.size() == board_.size(); }
bool TicTacToe::finished() const { return winner_ != '.' || moves_.size() == board_.size(); }
void TicTacToe::reset() {
    std::fill(board_.begin(), board_.end(), '.');
    moves_.clear();
    current_ = 'X';
    winner_ = '.';
}
bool TicTacToe::operator==(const TicTacToe& other) const {
    return size_ == other.size_ && board_ == other.board_ && current_ == other.current_;
}
bool TicTacToe::operator!=(const TicTacToe& other) const { return !(*this == other); }
std::ostream& operator<<(std::ostream& out, const TicTacToe& game) {
    out << game.size_ << ' ' << game.moves_.size();
    for (const auto index : game.moves_) out << ' ' << index;
    return out;
}
std::istream& operator>>(std::istream& in, TicTacToe& game) {
    std::size_t size = 0, count = 0;
    if (!(in >> size >> count)) return in;
    if (size == 0 || size > TicTacToe::max_size || count > size * size) {
        in.setstate(std::ios::failbit);
        return in;
    }
    TicTacToe restored(size);
    for (std::size_t move = 0; move < count; ++move) {
        std::size_t index = 0;
        if (!(in >> index)) return in;
        try { restored[index] = restored.current_player(); }
        catch (const std::exception&) { in.setstate(std::ios::failbit); return in; }
    }
    game = restored;
    return in;
}
} // namespace ppois
