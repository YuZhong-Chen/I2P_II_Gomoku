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
    int BoardStartX = GAMEBOARD_SIZE;
    int BoardStartY = GAMEBOARD_SIZE;
    int BoardEndX = 0;
    int BoardEndY = 0;
    bool BoardExistChestX[GAMEBOARD_SIZE] = {false};
    bool BoardExistChestY[GAMEBOARD_SIZE] = {false};
    array<array<int, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    array<int, 3> ChessCount;

    void BoardAddSpot(int x, int y, int chess);
    void BoardRemoveSpot(int x, int y);
} GAMEINFO;
GAMEINFO GameInfo;

using NODE = pair<int, pair<int, int> /**/>;

class GAMECONTROL
{
public:
    int SearchDepth = 2;

    NODE Minimax(int Depth, bool maximizingPlayer, ofstream &fout);
    NODE AlphaBeta(int Depth, int a, int b, bool maximizingPlayer, ofstream &fout);
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
        // if (GameInfo.ChessCount[EMPTY] < 200)
        //     GameControl.SearchDepth = 2;
        // else
        //     GameControl.SearchDepth = 3;

        // NODE spot = GameControl.Minimax(GameControl.SearchDepth, true, fout);

        GameControl.SearchDepth = 2;
        NODE spot = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        fout << spot.second.second << " " << spot.second.first << '\n';
        fout.flush();

        GameControl.SearchDepth = 3;
        spot = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        fout << spot.second.second << " " << spot.second.first << '\n';
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
            GameInfo.BoardExistChestX[x] = (GameInfo.BoardExistChestX[x] || GameInfo.Board[x][y] != EMPTY);
            GameInfo.BoardExistChestY[y] = (GameInfo.BoardExistChestY[y] || GameInfo.Board[x][y] != EMPTY);
        }
    }

    for (int i = 0; i < GAMEBOARD_SIZE; i++)
    {
        if (GameInfo.BoardExistChestX[i])
        {
            GameInfo.BoardStartX = min(GameInfo.BoardStartX, i);
            GameInfo.BoardEndX = max(GameInfo.BoardEndX, i);
        }
        if (GameInfo.BoardExistChestY[i])
        {
            GameInfo.BoardStartY = min(GameInfo.BoardStartY, i);
            GameInfo.BoardEndY = max(GameInfo.BoardEndY, i);
        }
    }

#ifdef DEBUG
    fout << "Read Board\n";
    PrintBoard(fout);
#endif
}

void GAMEINFO::BoardAddSpot(int x, int y, int chess)
{
    GameInfo.Board[x][y] = chess;
    GameInfo.ChessCount[chess] += 1;
    GameInfo.ChessCount[EMPTY] -= 1;
    GameInfo.BoardExistChestX[x] = true;
    GameInfo.BoardExistChestY[y] = true;

    GameInfo.BoardStartX = min(GameInfo.BoardStartX, x);
    GameInfo.BoardEndX = max(GameInfo.BoardEndX, x);
    GameInfo.BoardStartY = min(GameInfo.BoardStartY, y);
    GameInfo.BoardEndY = max(GameInfo.BoardEndY, y);
}

void GAMEINFO::BoardRemoveSpot(int x, int y)
{
    GameInfo.ChessCount[GameInfo.Board[x][y]] -= 1;
    GameInfo.ChessCount[EMPTY] += 1;
    GameInfo.Board[x][y] = EMPTY;

    GameInfo.BoardExistChestX[x] = GameInfo.BoardExistChestY[y] = false;
    for (int i = 0; i < GAMEBOARD_SIZE; i++)
    {
        if (GameInfo.Board[x][i] != EMPTY)
            GameInfo.BoardExistChestX[x] = true;
        if (GameInfo.Board[i][y] != EMPTY)
            GameInfo.BoardExistChestY[y] = true;
    }

    if (!GameInfo.BoardExistChestX[x])
    {
        if (x == GameInfo.BoardStartX)
        {
            for (int i = GameInfo.BoardStartX + 1; i < GAMEBOARD_SIZE; i++)
            {
                if (GameInfo.BoardExistChestX[i])
                {
                    GameInfo.BoardStartX = i;
                    break;
                }
            }
        }
        else if (x == GameInfo.BoardEndX)
        {
            for (int i = GameInfo.BoardEndX - 1; i >= 0; i--)
            {
                if (GameInfo.BoardExistChestX[i])
                {
                    GameInfo.BoardEndX = i;
                    break;
                }
            }
        }
    }
    if (!GameInfo.BoardExistChestY[y])
    {
        if (y == GameInfo.BoardStartY)
        {
            for (int i = GameInfo.BoardStartY + 1; i < GAMEBOARD_SIZE; i++)
            {
                if (GameInfo.BoardExistChestY[i])
                {
                    GameInfo.BoardStartY = i;
                    break;
                }
            }
        }
        else if (y == GameInfo.BoardEndY)
        {
            for (int i = GameInfo.BoardEndY - 1; i >= 0; i--)
            {
                if (GameInfo.BoardExistChestY[i])
                {
                    GameInfo.BoardEndY = i;
                    break;
                }
            }
        }
    }
}

void GAMECONTROL::PrintBoard(ofstream &fout)
{
    fout << "BLACK : " << GameInfo.ChessCount[BLACK] << "\n";
    fout << "WHITE : " << GameInfo.ChessCount[WHITE] << "\n";
    fout << "EMPTY : " << GameInfo.ChessCount[EMPTY] << "\n";
    fout << "X : " << GameInfo.BoardStartX << " " << GameInfo.BoardEndX << "\n";
    fout << "Y : " << GameInfo.BoardStartY << " " << GameInfo.BoardEndY << "\n";
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
    for (int y = GameInfo.BoardStartY; y <= GameInfo.BoardEndY; y++)
    {
        Direction[1] = (y >= 4);
        Direction[3] = (y < GAMEBOARD_SIZE - 4);
        for (int x = GameInfo.BoardStartX; x <= GameInfo.BoardEndX; x++)
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

    int SearchXStart = 0;
    int SearchYStart = 0;
    int SearchXEnd = GAMEBOARD_SIZE;
    int SearchYEnd = GAMEBOARD_SIZE;

    if (GameInfo.BoardStartX >= 2)
        SearchXStart = GameInfo.BoardStartX - 2;
    if (GameInfo.BoardStartY >= 2)
        SearchYStart = GameInfo.BoardStartY - 2;
    if (GameInfo.BoardEndX < GAMEBOARD_SIZE - 3)
        SearchXEnd = GameInfo.BoardEndX + 3;
    if (GameInfo.BoardEndY < GAMEBOARD_SIZE - 3)
        SearchYEnd = GameInfo.BoardEndY + 3;

    if (maximizingPlayer)
    {
        NODE EvaluateValue;
        NODE value(INT_MIN, make_pair(7, 7));

        for (int y = SearchYStart; y < SearchYEnd; y++)
        {
            for (int x = SearchXStart; x < SearchXEnd; x++)
            {
                if (GameInfo.Board[x][y] == EMPTY)
                {
                    GameInfo.BoardAddSpot(x, y, GameInfo.Player);

                    EvaluateValue = Minimax(Depth - 1, false, fout);
                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    GameInfo.BoardRemoveSpot(x, y);
                }
            }
        }

        return value;
    }
    else /* minimizing player */
    {
        NODE EvaluateValue;
        NODE value(INT_MAX, make_pair(7, 7));

        for (int y = SearchYStart; y < SearchYEnd; y++)
        {
            for (int x = SearchXStart; x < SearchXEnd; x++)
            {
                if (GameInfo.Board[x][y] == EMPTY)
                {
                    GameInfo.BoardAddSpot(x, y, GameInfo.Rival);

                    EvaluateValue = Minimax(Depth - 1, false, fout);
                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    GameInfo.BoardRemoveSpot(x, y);
                }
            }
        }

        return value;
    }
}

NODE GAMECONTROL::AlphaBeta(int Depth, int a, int b, bool maximizingPlayer, ofstream &fout)
{
    if (Depth == 0 || GameInfo.ChessCount[EMPTY] == 0)
    {
        return make_pair(EvaluateBoard(fout), make_pair(7, 7));
    }

    int SearchXStart = 0;
    int SearchYStart = 0;
    int SearchXEnd = GAMEBOARD_SIZE;
    int SearchYEnd = GAMEBOARD_SIZE;

    if (GameInfo.BoardStartX >= 1)
        SearchXStart = GameInfo.BoardStartX - 1;
    if (GameInfo.BoardStartY >= 1)
        SearchYStart = GameInfo.BoardStartY - 1;
    if (GameInfo.BoardEndX < GAMEBOARD_SIZE - 2)
        SearchXEnd = GameInfo.BoardEndX + 2;
    if (GameInfo.BoardEndY < GAMEBOARD_SIZE - 2)
        SearchYEnd = GameInfo.BoardEndY + 2;

    if (maximizingPlayer)
    {
        NODE EvaluateValue;
        NODE value(INT_MIN, make_pair(7, 7));

        for (int y = SearchYStart; y < SearchYEnd; y++)
        {
            for (int x = SearchXStart; x < SearchXEnd; x++)
            {
                if (GameInfo.Board[x][y] == EMPTY)
                {
                    GameInfo.BoardAddSpot(x, y, GameInfo.Player);

                    EvaluateValue = AlphaBeta(Depth - 1, a, b, false, fout);
                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }
                    if (value.first >= b)
                    {
                        GameInfo.BoardRemoveSpot(x, y);
                        break;
                    }

                    a = max(a, value.first);

                    GameInfo.BoardRemoveSpot(x, y);
                }
            }
        }

        return value;
    }
    else /* minimizing player */
    {
        NODE EvaluateValue;
        NODE value(INT_MAX, make_pair(7, 7));

        for (int y = SearchYStart; y < SearchYEnd; y++)
        {
            for (int x = SearchXStart; x < SearchXEnd; x++)
            {
                if (GameInfo.Board[x][y] == EMPTY)
                {
                    GameInfo.BoardAddSpot(x, y, GameInfo.Rival);

                    EvaluateValue = AlphaBeta(Depth - 1, a, b, false, fout);
                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    if (value.first <= a)
                    {
                        GameInfo.BoardRemoveSpot(x, y);
                        break;
                    }

                    b = min(b, value.first);

                    GameInfo.BoardRemoveSpot(x, y);
                }
            }
        }

        return value;
    }
}