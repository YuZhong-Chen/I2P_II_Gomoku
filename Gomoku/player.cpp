#include <algorithm>
#include <array>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#define GAMEBOARD_SIZE 15

#define DEBUG

enum SPOT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

typedef struct
{
    int Player;
    int Rival;
    std::array<std::array<int, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    std::array<int, 3> ChessCount;
} GAMEINFO;
GAMEINFO GameInfo;

class GAMECONTROL
{
public:
    int EvaluateBoard(std::ofstream &fout);
    void ReadBoard(std::ifstream &fin, std::ofstream &fout);
    void WriteValidSpot(std::ofstream &fout);
    void PrintBoard(std::ofstream &fout);
} GameControl;

int main(int, char **argv)
{
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);

    GameControl.ReadBoard(fin, fout);
    GameControl.WriteValidSpot(fout);

    fin.close();
    fout.close();
    return 0;
}

void GAMECONTROL::ReadBoard(std::ifstream &fin, std::ofstream &fout)
{
    GameInfo.ChessCount[EMPTY] = GameInfo.ChessCount[BLACK] = GameInfo.ChessCount[WHITE] = 0;

    fin >> GameInfo.Player;
    GameInfo.Rival = 3 - GameInfo.Player;

    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            fin >> GameInfo.Board[x][y];
            GameInfo.ChessCount[GameInfo.Board[x][y]] += 1;
        }
    }

#ifdef DEBUG
    fout << "Read Board\n";
    fout << "BLACK : " << GameInfo.ChessCount[BLACK] << "\n";
    fout << "WHITE : " << GameInfo.ChessCount[WHITE] << "\n";
    fout << "EMPTY : " << GameInfo.ChessCount[EMPTY] << "\n";
    PrintBoard(fout);
#endif
}

void GAMECONTROL::PrintBoard(std::ofstream &fout)
{
    fout << "===============================\n";
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        fout << "|";
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            if (GameInfo.Board[x][y] == BLACK)
                fout << "X";
            else if (GameInfo.Board[x][y] == WHITE)
                fout << "O";
            else
                fout << ".";

            if (x + 1 != GAMEBOARD_SIZE)
                fout << " ";
        }
        fout << "|\n";
    }
    fout << "===============================\n\n";
}

int GAMECONTROL::EvaluateBoard(std::ofstream &fout)
{
    int PlayerValue = 0;
    int PlayerLevelCount[6] = {0};
    int PlayerLevelRate[6] = {0, 1, 1, 2, 4, 10};

    int RivalValue = 0;
    int RivalLevelCount[6] = {0};
    int RivalLevelRate[6] = {0, 1, 1, 2, 4, 10};

    int LevelCount;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            if (GameInfo.Board[x][y] == EMPTY)
                continue;

            if (GameInfo.Board[x][y] == GameInfo.Player)
            {
                if (x + 4 < GAMEBOARD_SIZE)
                {
                    // =====================================================
                    // type one
                    LevelCount = 1;
                    for (int i = 1; i <= 4; i++)
                    {
                        if (GameInfo.Board[x + i][y] == GameInfo.Player)
                        {
                            LevelCount += 1;
                        }
                        else if (GameInfo.Board[x + i][y] == GameInfo.Rival)
                        {
                            LevelCount = -1;
                            break;
                        }
                    }
                    if (LevelCount != -1)
                        PlayerLevelCount[LevelCount] += 1;
                    // =====================================================

                    if (y + 4 < GAMEBOARD_SIZE)
                    {
                    }
                }
                else if (y + 4 > GAMEBOARD_SIZE)
                {
                }
            }
            else /* GameInfo.Rival **/
            {
            }
        }
    }

    for (int i = 1; i <= 5; i++)
    {
        PlayerValue = PlayerLevelCount[i] * PlayerLevelRate[i];
        RivalValue = RivalLevelCount[i] * RivalLevelRate[i];
    }

#ifdef DEBUG
    fout << "Evaluate Board\n";
    for (int i = 1; i <= 5; i++)
    {
        fout << "Player Level " << i << " : " << PlayerLevelCount[i] << "\n";
    }
    fout << "\n";
    for (int i = 1; i <= 5; i++)
    {
        fout << "Rival Level " << i << " : " << RivalLevelCount[i] << "\n";
    }

#endif

    return PlayerValue - RivalValue;
}

void GAMECONTROL::WriteValidSpot(std::ofstream &fout)
{
    EvaluateBoard(fout);

    // srand(time(NULL));
    // int x, y;
    // Keep updating the output until getting killed.
    // while (true)
    // {
    //     // Choose a random spot.
    //     x = (rand() % SIZE);
    //     y = (rand() % SIZE);
    //     if (board[x][y] == EMPTY)
    //     {
    //         fout << x << " " << y << std::endl;
    //         // Remember to flush the output to ensure the last action is written to file.
    //         fout.flush();
    //         break;
    //     }
    // }
}