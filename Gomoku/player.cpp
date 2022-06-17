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
    void CountLevel(int Player, int Rival, int *LevelCount, int x, int y, bool *Direction);
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

void GAMECONTROL::CountLevel(int Player, int Rival, int *PlayerLevelCount, int x, int y, bool *Direction)
{
    int LevelCount;

    // NOTE : type 1
    if (Direction[0])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x + i][y] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x + i][y] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 2
    if (Direction[0] && Direction[1])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x + i][y - i] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x + i][y - i] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 3
    if (Direction[1])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x][y - i] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x][y - i] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 4
    if (Direction[1] && Direction[2])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x - i][y - i] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x - i][y - i] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 5
    if (Direction[2])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x - i][y] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x - i][y] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 6
    if (Direction[2] && Direction[3])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x - i][y + i] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x - i][y + i] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 7
    if (Direction[3])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x][y + i] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x][y + i] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
    // NOTE : type 8
    if (Direction[0] && Direction[3])
    {
        LevelCount = 1;
        for (int i = 1; i <= 4; i++)
        {
            if (GameInfo.Board[x + i][y + i] == Player)
            {
                LevelCount += 1;
            }
            else if (GameInfo.Board[x + i][y + i] == Rival)
            {
                LevelCount = -1;
                break;
            }
        }
        if (LevelCount != -1)
            PlayerLevelCount[LevelCount] += 1;
    }
}

int GAMECONTROL::EvaluateBoard(std::ofstream &fout)
{
    int PlayerValue = 0;
    int PlayerLevelCount[6] = {0};
    int PlayerLevelRate[6] = {0, 0, 1, 2, 4, 10};

    int RivalValue = 0;
    int RivalLevelCount[6] = {0};
    int RivalLevelRate[6] = {0, 0, 1, 2, 4, 10};

    bool Direction[4];
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        Direction[1] = (y >= 4);
        Direction[3] = (y < GAMEBOARD_SIZE - 4);
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            if (GameInfo.Board[x][y] == EMPTY)
                continue;

            Direction[0] = (x < GAMEBOARD_SIZE - 4);
            Direction[2] = (x >= 4);

            if (GameInfo.Board[x][y] == GameInfo.Player)
                CountLevel(GameInfo.Player, GameInfo.Rival, PlayerLevelCount, x, y, Direction);
            else /* GameInfo.Rival **/
                CountLevel(GameInfo.Rival, GameInfo.Player, RivalLevelCount, x, y, Direction);
        }
    }

    for (int i = 1; i <= 5; i++)
    {
        PlayerValue += PlayerLevelCount[i] * PlayerLevelRate[i];
        RivalValue += RivalLevelCount[i] * RivalLevelRate[i];
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
    fout << "\nValue : " << PlayerValue - RivalValue << "\n";
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