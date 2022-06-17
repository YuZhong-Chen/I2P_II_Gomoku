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
using namespace std;

#define GAMEBOARD_SIZE 15
// #define DEBUG

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
    array<array<int, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    array<int, 3> ChessCount;
} GAMEINFO;
GAMEINFO GameInfo;

using NODE = pair<int, pair<int, int> /**/>;

class GAMECONTROL
{
public:
    const int MinimaxDepth = 2;

    NODE Minimax(int Depth, bool maximizingPlayer, ofstream &fout);
    int EvaluateBoard(ofstream &fout);
    void CountLevel(int Player, int Rival, int *LevelCount, int x, int y, bool *Direction);
    void CountLevelByDirection(int Player, int Rival, int *LevelCount, int x, int y, int Dir);
    void ReadBoard(ifstream &fin, ofstream &fout);
    void PrintBoard(ofstream &fout);
} GameControl;

int main(int, char **argv)
{
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    GameControl.ReadBoard(fin, fout);

    // GameControl.EvaluateBoard(fout);

    if (GameInfo.ChessCount[EMPTY] != GAMEBOARD_SIZE * GAMEBOARD_SIZE)
    {
        NODE spot = GameControl.Minimax(GameControl.MinimaxDepth, true, fout);

        // NOTE: Remember
        fout << spot.second.second << " " << spot.second.first << "\n";

        // GameInfo.Board[spot.second.first][spot.second.second] = GameInfo.Player;
        // GameControl.PrintBoard(fout);

        fout.flush();
    }
    else
    {
        fout << "7 7\n";
        fout.flush();
    }

    fin.close();
    fout.close();
    return 0;
}

void GAMECONTROL::ReadBoard(ifstream &fin, ofstream &fout)
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
    PrintBoard(fout);
#endif
}

void GAMECONTROL::PrintBoard(ofstream &fout)
{
    fout << "BLACK : " << GameInfo.ChessCount[BLACK] << "\n";
    fout << "WHITE : " << GameInfo.ChessCount[WHITE] << "\n";
    fout << "EMPTY : " << GameInfo.ChessCount[EMPTY] << "\n";
    fout << "===============================\n";
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        fout << "|";
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            if (GameInfo.Board[x][y] == BLACK)
                fout << "O";
            else if (GameInfo.Board[x][y] == WHITE)
                fout << "X";
            else
                fout << ".";

            if (x + 1 != GAMEBOARD_SIZE)
                fout << " ";
        }
        fout << "|\n";
    }
    fout << "===============================\n\n";
}

const int CountLevelDirection[8][2] = {{1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};
void GAMECONTROL::CountLevelByDirection(int Player, int Rival, int *PlayerLevelCount, int x, int y, int Dir)
{
    int LevelCount = 1;
    for (int i = 1; i <= 4; i++)
    {
        if (GameInfo.Board[x + i * CountLevelDirection[Dir][0]][y + i * CountLevelDirection[Dir][1]] == Player)
        {
            LevelCount += 1;
        }
        else if (GameInfo.Board[x + i * CountLevelDirection[Dir][0]][y + i * CountLevelDirection[Dir][1]] == Rival)
        {
            LevelCount = -1;
            break;
        }
        else
        {
            break;
        }
    }
    if (LevelCount != -1)
        PlayerLevelCount[LevelCount] += 1;
}

void GAMECONTROL::CountLevel(int Player, int Rival, int *PlayerLevelCount, int x, int y, bool *Direction)
{
    // NOTE : type 1
    if (Direction[0])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 0);
    // NOTE : type 2
    if (Direction[0] && Direction[1])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 1);
    // NOTE : type 3
    if (Direction[1])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 2);
    // NOTE : type 4
    if (Direction[1] && Direction[2])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 3);
    // NOTE : type 5
    if (Direction[2])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 4);
    // NOTE : type 6
    if (Direction[2] && Direction[3])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 5);
    // NOTE : type 7
    if (Direction[3])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 6);
    // NOTE : type 8
    if (Direction[0] && Direction[3])
        CountLevelByDirection(Player, Rival, PlayerLevelCount, x, y, 7);
}

int GAMECONTROL::EvaluateBoard(ofstream &fout)
{
    int PlayerValue = 0;
    int PlayerLevelCount[6] = {0};
    int PlayerLevelRate[6] = {0, 0, 2, 6, 10, 100000};

    int RivalValue = 0;
    int RivalLevelCount[6] = {0};
    int RivalLevelRate[6] = {0, 1, 5, 20, 30, 2000};

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

NODE GAMECONTROL::Minimax(int Depth, bool maximizingPlayer, ofstream &fout)
{
    if (Depth == 0 || GameInfo.ChessCount[EMPTY] == 0)
    {
        return make_pair(EvaluateBoard(fout), make_pair(7, 7));
    }

    if (maximizingPlayer)
    {
        NODE EvaluateValue;
        NODE value(INT_MIN, make_pair(7, 7));

        for (int y = 0; y < GAMEBOARD_SIZE; y++)
        {
            for (int x = 0; x < GAMEBOARD_SIZE; x++)
            {
                if (GameInfo.Board[x][y] == EMPTY)
                {
                    GameInfo.Board[x][y] = GameInfo.Player;
                    GameInfo.ChessCount[EMPTY] -= 1;
                    GameInfo.ChessCount[GameInfo.Player] += 1;

                    EvaluateValue = Minimax(Depth - 1, false, fout);
                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    GameInfo.Board[x][y] = EMPTY;
                    GameInfo.ChessCount[EMPTY] += 1;
                    GameInfo.ChessCount[GameInfo.Player] -= 1;
                }
            }
        }

        return value;
    }
    else /* minimizing player */
    {
        NODE EvaluateValue;
        NODE value(INT_MAX, make_pair(7, 7));

        for (int y = 0; y < GAMEBOARD_SIZE; y++)
        {
            for (int x = 0; x < GAMEBOARD_SIZE; x++)
            {
                if (GameInfo.Board[x][y] == EMPTY)
                {
                    GameInfo.Board[x][y] = GameInfo.Rival;
                    GameInfo.ChessCount[EMPTY] -= 1;
                    GameInfo.ChessCount[GameInfo.Rival] += 1;

                    EvaluateValue = Minimax(Depth - 1, false, fout);
                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    GameInfo.Board[x][y] = EMPTY;
                    GameInfo.ChessCount[EMPTY] += 1;
                    GameInfo.ChessCount[GameInfo.Rival] -= 1;
                }
            }
        }

        return value;
    }
}