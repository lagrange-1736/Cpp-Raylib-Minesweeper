#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <utility>

using namespace std;

const int ROWS = 9;
const int COLS = 9;
const int MINE_COUNT = 10;
const int CELL_SIZE = 40;

struct Cell {
    bool isMine = false;
    int adjacentMines = 0;
    bool isRevealed = false;
    bool isFlagged = false;
};

// BFS 展开
void revealCell(vector<vector<Cell>>& board, int startR, int startC) {
    if (board[startR][startC].isMine || board[startR][startC].isRevealed) return;

    queue<pair<int, int>> q;
    q.push({startR, startC});
    board[startR][startC].isRevealed = true;

    while (!q.empty()) {
        int r = q.front().first;
        int c = q.front().second;
        q.pop();

        if (board[r][c].adjacentMines != 0) continue;

        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                    if (!board[nr][nc].isRevealed && !board[nr][nc].isFlagged) {
                        board[nr][nc].isRevealed = true;
                        q.push({nr, nc});
                    }
                }
            }
        }
    }
}

// 初始化棋盘
void initBoard(vector<vector<Cell>>& board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = Cell(); // 重置所有属性
        }
    }

    int placed = 0;
    while (placed < MINE_COUNT) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (!board[r][c].isMine) {
            board[r][c].isMine = true;
            placed++;
        }
    }

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (board[r][c].isMine) continue;
            int count = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc].isMine)
                        count++;
                }
            }
            board[r][c].adjacentMines = count;
        }
    }
}

// 检查是否胜利
bool checkWin(const vector<vector<Cell>>& board) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (!board[r][c].isMine && !board[r][c].isRevealed) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    srand(time(0));
    vector<vector<Cell>> board(ROWS, vector<Cell>(COLS));
    initBoard(board);

    InitWindow(COLS * CELL_SIZE, ROWS * CELL_SIZE, "Raylib Minesweeper");
    SetTargetFPS(60);

    bool gameOver = false;
    bool gameWin = false;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        int hoverC = mousePos.x / CELL_SIZE;
        int hoverR = mousePos.y / CELL_SIZE;
        bool isHoverValid = (hoverR >= 0 && hoverR < ROWS && hoverC >= 0 && hoverC < COLS);

        // 更新逻辑
        if (!gameOver && !gameWin) {
            // 左键操作
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHoverValid) {
                if (!board[hoverR][hoverC].isRevealed) {
                    if (board[hoverR][hoverC].isMine) {
                        gameOver = true;
                    } else {
                        revealCell(board, hoverR, hoverC);
                    }
                } else if (board[hoverR][hoverC].adjacentMines > 0) {
                    // 核心：和弦机制（Chord）
                    int flagCount = 0;
                    for (int dr = -1; dr <= 1; ++dr) {
                        for (int dc = -1; dc <= 1; ++dc) {
                            if (dr == 0 && dc == 0) continue;
                            int nr = hoverR + dr, nc = hoverC + dc;
                            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc].isFlagged)
                                flagCount++;
                        }
                    }
                    
                    // 如果插旗数等于数字，周围全开
                    if (flagCount == board[hoverR][hoverC].adjacentMines) {
                        for (int dr = -1; dr <= 1; ++dr) {
                            for (int dc = -1; dc <= 1; ++dc) {
                                if (dr == 0 && dc == 0) continue;
                                int nr = hoverR + dr, nc = hoverC + dc;
                                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                                    if (!board[nr][nc].isFlagged && !board[nr][nc].isRevealed) {
                                        if (board[nr][nc].isMine) gameOver = true;
                                        else revealCell(board, nr, nc);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 右键操作
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && isHoverValid) {
                if (!board[hoverR][hoverC].isRevealed) {
                    board[hoverR][hoverC].isFlagged = !board[hoverR][hoverC].isFlagged;
                }
            }

            if (checkWin(board)) {
                gameWin = true;
            }
        } else {
            // 游戏结束后按 R 键重开
            if (IsKeyPressed(KEY_R)) {
                initBoard(board);
                gameOver = false;
                gameWin = false;
            }
        }

        // 绘制
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                int x = c * CELL_SIZE;
                int y = r * CELL_SIZE;

                Color bgColor = LIGHTGRAY;
                if (board[r][c].isRevealed) {
                    bgColor = RAYWHITE;
                } else if (isHoverValid && r == hoverR && c == hoverC && !gameOver && !gameWin) {
                    bgColor = GRAY; // 鼠标悬停高亮
                }

                if (gameOver && board[r][c].isMine) bgColor = RED;

                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, bgColor);
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, DARKGRAY);

                if (board[r][c].isRevealed) {
                    if (board[r][c].isMine) {
                        DrawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, 10, BLACK);
                    } else if (board[r][c].adjacentMines > 0) {
                        Color numColor = BLUE;
                        if (board[r][c].adjacentMines == 2) numColor = GREEN;
                        if (board[r][c].adjacentMines == 3) numColor = RED;
                        DrawText(TextFormat("%d", board[r][c].adjacentMines), x + 12, y + 8, 24, numColor);
                    }
                } else if (board[r][c].isFlagged) {
                    // 1. 画黑色的旗杆 
                    DrawRectangle(x + 18, y + 8, 2, 24, BLACK); 
                    // 2. 画红色的旗面 
                    DrawRectangle(x + 20, y + 8, 12, 10, RED);
                    // 3. 画底座 
                    DrawRectangle(x + 14, y + 32, 10, 2, BLACK);
                }
            }
        }

        // 结算文字
        if (gameOver) {
            DrawRectangle(0, COLS*CELL_SIZE/2 - 20, ROWS*CELL_SIZE, 40, Fade(BLACK, 0.8f));
            DrawText("BOOM! Press R to Restart", 30, COLS*CELL_SIZE/2 - 10, 20, WHITE);
        } else if (gameWin) {
            DrawRectangle(0, COLS*CELL_SIZE/2 - 20, ROWS*CELL_SIZE, 40, Fade(BLACK, 0.8f));
            DrawText("YOU WIN! Press R to Restart", 20, COLS*CELL_SIZE/2 - 10, 20, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}