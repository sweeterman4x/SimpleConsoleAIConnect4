#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <ctime>
#include <cstdlib>

// Constants
const int ROWS = 6;
const int COLS = 7;
const char EMPTY = '.';
const char PLAYER_PIECE = 'X';
const char AI_PIECE = 'O';
const int MAX_DEPTH = 4; // Limit depth of Minimax

// Colors
const std::string RED = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string RESET = "\033[0m";

// Taunts
const std::vector<std::string> AI_COMMENTS = {
    "Meow! I'm making a purrfect move!",
    "Paws and think... Done!",
    "This will be a claw-some win!",
    "Let's see if you can handle my feline finesse!",
    "I'm pouncing on this move!",
    "My whiskers are twitching for victory!",
    "Watch out! Here comes the cat's paw!"
};

// Game board
std::vector<std::vector<char>> board(ROWS, std::vector<char>(COLS, EMPTY));

// Function to print the board
void printBoard() {
    std::cout << "+";
    for (int col = 0; col < COLS; ++col) {
        std::cout << "---+";
    }
    std::cout << '\n';

    for (int row = ROWS - 1; row >= 0; --row) {
        std::cout << "|";
        for (int col = 0; col < COLS; ++col) {
            char piece = board[row][col];
            if (piece == PLAYER_PIECE) {
                std::cout << RED << ' ' << piece << ' ' << RESET;
            }
            else if (piece == AI_PIECE) {
                std::cout << YELLOW << ' ' << piece << ' ' << RESET;
            }
            else {
                std::cout << ' ' << piece << ' ';
            }
            std::cout << '|';
        }
        std::cout << '\n';

        std::cout << "+";
        for (int col = 0; col < COLS; ++col) {
            std::cout << "---+";
        }
        std::cout << '\n';
    }

    std::cout << " ";
    for (int col = 0; col < COLS; ++col) {
        std::cout << " " << col << " ";
    }
    std::cout << '\n';
}

// Function to drop a piece into a column
bool dropPiece(int col, char piece) {
    if (col < 0 || col >= COLS) {
        std::cerr << "Column out of bounds!\n";
        return false;
    }

    for (int row = 0; row < ROWS; ++row) {
        if (board[row][col] == EMPTY) {
            board[row][col] = piece;
            return true;
        }
    }

    std::cerr << "Column is full!\n";
    return false;
}

// Function to get the lowest available row in a column
int getAvailableRow(int col) {
    for (int row = 0; row < ROWS; ++row) {
        if (board[row][col] == EMPTY) {
            return row;
        }
    }
    return -1; // Column is full
}

// Function to check if a player has won
bool checkWin(char piece) {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (board[row][col] == piece) {
                // Horizontal
                if (col + 3 < COLS &&
                    board[row][col + 1] == piece &&
                    board[row][col + 2] == piece &&
                    board[row][col + 3] == piece) {
                    return true;
                }
                // Vertical
                if (row + 3 < ROWS &&
                    board[row + 1][col] == piece &&
                    board[row + 2][col] == piece &&
                    board[row + 3][col] == piece) {
                    return true;
                }
                // Diagonal (down-right)
                if (row + 3 < ROWS && col + 3 < COLS &&
                    board[row + 1][col + 1] == piece &&
                    board[row + 2][col + 2] == piece &&
                    board[row + 3][col + 3] == piece) {
                    return true;
                }
                // Diagonal (down-left)
                if (row + 3 < ROWS && col - 3 >= 0 &&
                    board[row + 1][col - 1] == piece &&
                    board[row + 2][col - 2] == piece &&
                    board[row + 3][col - 3] == piece) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Function to evaluate the board and score potential moves
int evaluateBoard(char piece) {
    int score = 0;

    if (checkWin(piece)) {
        score += 1000; // High score for winning
    }

    char opponentPiece = (piece == AI_PIECE) ? PLAYER_PIECE : AI_PIECE;
    if (checkWin(opponentPiece)) {
        score -= 1000; // Low score for losing
    }

    // Add more scoring logic if needed
    return score;
}

// Minimax algorithm with alpha-beta pruning
int minimax(int depth, bool isMaximizingPlayer, int alpha, int beta) {
    char currentPiece = isMaximizingPlayer ? AI_PIECE : PLAYER_PIECE;

    if (checkWin(AI_PIECE)) return 1000; // AI wins
    if (checkWin(PLAYER_PIECE)) return -1000; // Player wins
    if (depth == 0) return evaluateBoard(currentPiece); // Evaluate board

    int bestScore = isMaximizingPlayer ? -std::numeric_limits<int>::max() : std::numeric_limits<int>::max();

    for (int col = 0; col < COLS; ++col) {
        int row = getAvailableRow(col);
        if (row != -1) {
            board[row][col] = currentPiece;
            int score = minimax(depth - 1, !isMaximizingPlayer, alpha, beta);
            board[row][col] = EMPTY; // Undo move

            if (isMaximizingPlayer) {
                bestScore = std::max(score, bestScore);
                alpha = std::max(alpha, bestScore);
            }
            else {
                bestScore = std::min(score, bestScore);
                beta = std::min(beta, bestScore);
            }

            if (beta <= alpha) break; // Alpha-Beta Pruning
        }
    }

    return bestScore;
}

// Function to get AI's best column choice
int getBestMove() {
    int bestCol = -1;
    int bestScore = -std::numeric_limits<int>::max();

    // Weight towards the center
    std::vector<int> cols(COLS);
    for (int i = 0; i < COLS; ++i) {
        cols[i] = i;
    }
    std::sort(cols.begin(), cols.end(), [](int a, int b) {
        return std::abs(a - COLS / 2) < std::abs(b - COLS / 2);
        });

    for (int col : cols) {
        int row = getAvailableRow(col);
        if (row != -1) {
            board[row][col] = AI_PIECE;
            int score = minimax(MAX_DEPTH, false, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
            board[row][col] = EMPTY; // Undo move

            if (score > bestScore) {
                bestScore = score;
                bestCol = col;
            }
        }
    }

    return bestCol;
}

// Function to print a random cat comment
void printCatComment() {
    int index = rand() % AI_COMMENTS.size();
    std::cout << AI_COMMENTS[index] << '\n';
}

// Function to reset the board for a new game
void resetBoard() {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            board[r][c] = EMPTY;
        }
    }
}

// Function to play the game
void playGame() {
    std::srand(std::time(nullptr)); // Seed the random number generator

    char currentPlayer = AI_PIECE;
    bool gameWon = false;
    int playerWins = 0;
    int aiWins = 0;

    while (true) {
        printBoard();
        int col;

        if (currentPlayer == AI_PIECE) {
            col = getBestMove();
            std::cout << "AI chooses column " << col << '\n';
            dropPiece(col, AI_PIECE);
            printCatComment();
            currentPlayer = PLAYER_PIECE;
        }
        else {
            std::cout << "Player, enter your column (0-6): ";
            std::cin >> col;

            if (!dropPiece(col, PLAYER_PIECE)) {
                std::cerr << "Invalid move. Try again.\n";
                continue;
            }
            currentPlayer = AI_PIECE;
        }

        if (checkWin(AI_PIECE)) {
            printBoard();
            std::cout << "AI wins!\n";
            aiWins++;
            gameWon = true;
        }
        else if (checkWin(PLAYER_PIECE)) {
            printBoard();
            std::cout << "Player wins!\n";
            playerWins++;
            gameWon = true;
        }
        else if (std::all_of(board[0].begin(), board[0].end(), [](char c) { return c != EMPTY; })) {
            printBoard();
            std::cout << "It's a draw!\n";
            gameWon = true;
        }

        if (gameWon) {
            std::cout << "Score - Player: " << playerWins << ", AI: " << aiWins << '\n';
            std::cout << "Play again? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'n' || choice == 'N') break;
            resetBoard();
            currentPlayer = AI_PIECE; // AI starts first again
            gameWon = false;
        }
    }
}

int main() {
    playGame();
    return 0;
}
