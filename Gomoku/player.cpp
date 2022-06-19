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

enum POINT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

enum CHESSLINE_TYPE
{
    FiveInRow = 0,
    LiveFour = 1,
    DeadFour = 2,
    LiveThree = 3,
    DeadThree = 4,
    LiveTwo = 5,
    DeadTwo = 6
};

const short Direction[8][2] = {{1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};
const short DirectionReverse[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

using POINT = pair<short, short>;
using NODE = pair<int, POINT>;

typedef struct
{
    short Player;
    short Rival;
    POINT BoardStart{GAMEBOARD_SIZE, GAMEBOARD_SIZE};
    POINT BoardEnd{0, 0};

    array<array<short, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    array<array<array<bool, 4>, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> BoardState;
    short EmptyChessCount = 0;

    void BoardAddSpot(POINT pt, short chess);
    void BoardRemoveSpot(POINT pt);
} GAMEINFO;
GAMEINFO GameInfo;

class GAMECONTROL
{
public:
    int SearchDepth = 2;

    short ChessLine[9];

    short TypeCount[3][7];

    // NOTE : remember to remove the fout in Minimax, AlphaBeta and EvaluateBoard
    NODE Minimax(int Depth, bool maximizingPlayer, ofstream &fout);
    NODE AlphaBeta(int Depth, int a, int b, bool maximizingPlayer, ofstream &fout);

    int EvaluateBoard(ofstream &fout);
    void UpdateChessLine(POINT pt, short dir);
    void EvaluatePoint(POINT pt, ofstream &fout);
    void EvaluateChessLine(short Player, short Rival, ofstream &fout);

    void ReadBoard(ifstream &fin, ofstream &fout);
    void PrintBoard(ofstream &fout);
} GameControl;

short min(short a, short b)
{
    return (a < b) ? a : b;
}
short max(short a, short b)
{
    return (a < b) ? b : a;
}

int main(int, char **argv)
{
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    GameControl.ReadBoard(fin, fout);

    GameControl.UpdateChessLine(make_pair(4, 14), 0);
    GameControl.EvaluateChessLine(GameInfo.Board[4][14], 3 - GameInfo.Board[4][14], fout);

    for (int i = 0; i < 9; i++)
    {
        if (GameControl.ChessLine[i] == BLACK)
            fout << "O";
        else if (GameControl.ChessLine[i] == WHITE)
            fout << "X";
        else
            fout << ".";
    }
    fout << "\n\n";

    for (int i = 0; i < 7; i++)
    {
        fout << "Player Type " << i << " : " << GameControl.TypeCount[GameInfo.Player][i] << "\n";
    }
    fout << "\n";
    for (int i = 0; i < 7; i++)
    {
        fout << "Rival Type " << i << " : " << GameControl.TypeCount[GameInfo.Rival][i] << "\n";
    }

    // GameControl.EvaluatePoint(make_pair(6, 7), fout);

    // GameControl.EvaluateBoard(fout);

    // if (GameInfo.EmptyChessCount != GAMEBOARD_SIZE * GAMEBOARD_SIZE)
    // {
    //     // if (GameInfo.ChessCount[EMPTY] < 200)
    //     //     GameControl.SearchDepth = 2;
    //     // else
    //     //     GameControl.SearchDepth = 3;

    //     // NODE spot = GameControl.Minimax(GameControl.SearchDepth, true, fout);

    //     // GameControl.SearchDepth = 2;
    //     // NODE spot = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
    //     // fout << spot.second.second << " " << spot.second.first << '\n';
    //     // fout.flush();

    //     // GameControl.SearchDepth = 3;
    //     // spot = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
    //     // fout << spot.second.second << " " << spot.second.first << '\n';
    //     // fout.flush();
    // }
    // else
    // {
    //     fout << "6 6\n";
    //     fout.flush();
    // }

    fin.close();
    fout.close();
    return 0;
}

void GAMECONTROL::ReadBoard(ifstream &fin, ofstream &fout)
{
    fin >> GameInfo.Player;
    GameInfo.Rival = 3 - GameInfo.Player;

    bool BoardExistChestX[GAMEBOARD_SIZE] = {false};
    bool BoardExistChestY[GAMEBOARD_SIZE] = {false};

    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            fin >> GameInfo.Board[x][y];
            GameInfo.EmptyChessCount += (GameInfo.Board[x][y] == EMPTY);
            BoardExistChestX[x] = (BoardExistChestX[x] || GameInfo.Board[x][y] != EMPTY);
            BoardExistChestY[y] = (BoardExistChestY[y] || GameInfo.Board[x][y] != EMPTY);

            GameInfo.BoardState[x][y][0] = GameInfo.BoardState[x][y][1] = GameInfo.BoardState[x][y][2] = GameInfo.BoardState[x][y][3] = false;
        }
    }

    for (int i = 0; i < GAMEBOARD_SIZE; i++)
    {
        if (BoardExistChestX[i])
        {
            GameInfo.BoardStart.first = min(GameInfo.BoardStart.first, i);
            GameInfo.BoardEnd.first = max(GameInfo.BoardEnd.first, i);
        }
        if (BoardExistChestY[i])
        {
            GameInfo.BoardStart.second = min(GameInfo.BoardStart.second, i);
            GameInfo.BoardEnd.second = max(GameInfo.BoardEnd.second, i);
        }
    }

#ifdef DEBUG
    fout << "Read Board\n";
    PrintBoard(fout);
#endif
}

void GAMEINFO::BoardAddSpot(POINT pt, short chess)
{
    GameInfo.Board[pt.first][pt.second] = chess;
    GameInfo.EmptyChessCount--;

    GameInfo.BoardStart.first = min(GameInfo.BoardStart.first, pt.first);
    GameInfo.BoardEnd.first = max(GameInfo.BoardEnd.first, pt.first);
    GameInfo.BoardStart.second = min(GameInfo.BoardStart.second, pt.second);
    GameInfo.BoardEnd.second = max(GameInfo.BoardEnd.second, pt.second);
}

void GAMEINFO::BoardRemoveSpot(POINT pt)
{
    GameInfo.EmptyChessCount++;
    GameInfo.Board[pt.first][pt.second] = EMPTY;
}

void GAMECONTROL::PrintBoard(ofstream &fout)
{
    // fout << "EMPTY : " << GameInfo.EmptyChessCount << "\n";
    // fout << "X boundary : " << GameInfo.BoardStart.first << " " << GameInfo.BoardEnd.first << "\n";
    // fout << "Y boundary : " << GameInfo.BoardStart.second << " " << GameInfo.BoardEnd.second << "\n";
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

void GAMECONTROL::EvaluatePoint(POINT pt, ofstream &fout)
{
    for (int dir = 0; dir < 4; dir++)
    {
        if (!GameInfo.BoardState[pt.first][pt.second][dir])
        {
            UpdateChessLine(pt, dir);
            EvaluateChessLine(GameInfo.Board[pt.first][pt.second], 3 - GameInfo.Board[pt.first][pt.second], fout);
        }
    }
}

void GAMECONTROL::UpdateChessLine(POINT pt, short dir)
{
    short temp = 3 - GameInfo.Board[pt.first][pt.second];
    int x = pt.first + 5 * DirectionReverse[dir][0];
    int y = pt.second + 5 * DirectionReverse[dir][1];
    for (int i = 0; i < 9; i++)
    {
        x += Direction[dir][0];
        y += Direction[dir][1];

        if (x >= 0 && x < GAMEBOARD_SIZE && y >= 0 && y < GAMEBOARD_SIZE)
        {
            GameInfo.BoardState[x][y][dir] = true;
            ChessLine[i] = GameInfo.Board[x][y];
        }
        else
        {
            ChessLine[i] = temp;
        }
    }
}
void GAMECONTROL::EvaluateChessLine(short Player, short Rival, ofstream &fout)
{
    short L_continuous = 4;
    short R_continuous = 4;

    for (; L_continuous >= 0; L_continuous--)
        if (ChessLine[L_continuous - 1] != Player)
            break;

    for (; R_continuous <= 8; R_continuous++)
        if (ChessLine[R_continuous + 1] != Player)
            break;

    short total_continuous = R_continuous - L_continuous + 1;

    if (total_continuous >= 5)
    {
        TypeCount[Player][FiveInRow] += 1;
    }
    else if (total_continuous == 4)
    {
    }
    else if (total_continuous == 3)
    {
    }
    else if (total_continuous == 2)
    {
    }
    else if (total_continuous == 1)
    {
    }
}

int GAMECONTROL::EvaluateBoard(ofstream &fout)
{
    // TODO : modify here
    int PlayerValue = 0;
    int PlayerTypeRate[7] = {0, 0, 0, 0, 0, 0, 0};

    int RivalValue = 0;
    int RivalTypeRate[7] = {0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < 7; i++)
        GameControl.TypeCount[1][i] = GameControl.TypeCount[2][i] = 0;

    for (int y = GameInfo.BoardStart.second; y <= GameInfo.BoardEnd.second; y++)
        for (int x = GameInfo.BoardStart.first; x <= GameInfo.BoardEnd.first; x++)
            if (GameInfo.Board[x][y] != EMPTY)
                GameInfo.BoardState[x][y][0] = GameInfo.BoardState[x][y][1] = GameInfo.BoardState[x][y][2] = GameInfo.BoardState[x][y][3] = false;

    for (int y = GameInfo.BoardStart.second; y <= GameInfo.BoardEnd.second; y++)
    {
        for (int x = GameInfo.BoardStart.first; x <= GameInfo.BoardEnd.first; x++)
        {
            if (GameInfo.Board[x][y] == EMPTY)
                continue;

            EvaluatePoint(make_pair(x, y), fout);
        }
    }

    // TODO : Add value
    for (int i = 0; i < 7; i++)
    {
        PlayerValue += GameControl.TypeCount[GameInfo.Player][i] * PlayerTypeRate[i];
        RivalValue += GameControl.TypeCount[GameInfo.Rival][i] * RivalTypeRate[i];
    }

#ifdef DEBUG
    fout << "Evaluate Board\n";
    for (int i = 0; i < 7; i++)
    {
        fout << "Player Type " << i << " : " << GameControl.TypeCount[GameInfo.Player][i] << "\n";
    }
    fout << "\n";
    for (int i = 0; i < 7; i++)
    {
        fout << "Rival Type " << i << " : " << GameControl.TypeCount[GameInfo.Rival][i] << "\n";
    }
    fout << "\nValue : " << PlayerValue - RivalValue << "\n";
#endif

    return PlayerValue - RivalValue;
}

NODE GAMECONTROL::Minimax(int Depth, bool maximizingPlayer, ofstream &fout)
{
    if (Depth == 0 || GameInfo.EmptyChessCount == 0)
    {
        return make_pair(EvaluateBoard(fout), make_pair(7, 7));
    }

    int SearchXStart = 0;
    int SearchYStart = 0;
    int SearchXEnd = GAMEBOARD_SIZE;
    int SearchYEnd = GAMEBOARD_SIZE;

    if (GameInfo.BoardStart.first >= 2)
        SearchXStart = GameInfo.BoardStart.first - 2;
    if (GameInfo.BoardStart.second >= 2)
        SearchYStart = GameInfo.BoardStart.second - 2;
    if (GameInfo.BoardEnd.first < GAMEBOARD_SIZE - 3)
        SearchXEnd = GameInfo.BoardEnd.first + 3;
    if (GameInfo.BoardEnd.second < GAMEBOARD_SIZE - 3)
        SearchYEnd = GameInfo.BoardEnd.second + 3;

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
                    POINT tempStart = GameInfo.BoardStart;
                    POINT tempEnd = GameInfo.BoardEnd;
                    GameInfo.BoardAddSpot(make_pair(x, y), GameInfo.Player);

                    EvaluateValue = Minimax(Depth - 1, false, fout);
                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    GameInfo.BoardRemoveSpot(make_pair(x, y));
                    GameInfo.BoardStart = tempStart;
                    GameInfo.BoardEnd = tempEnd;
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
                    POINT tempStart = GameInfo.BoardStart;
                    POINT tempEnd = GameInfo.BoardEnd;
                    GameInfo.BoardAddSpot(make_pair(x, y), GameInfo.Rival);

                    EvaluateValue = Minimax(Depth - 1, false, fout);
                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    GameInfo.BoardRemoveSpot(make_pair(x, y));
                    GameInfo.BoardStart = tempStart;
                    GameInfo.BoardEnd = tempEnd;
                }
            }
        }

        return value;
    }
}

NODE GAMECONTROL::AlphaBeta(int Depth, int a, int b, bool maximizingPlayer, ofstream &fout)
{
    if (Depth == 0 || GameInfo.EmptyChessCount == 0)
    {
        return make_pair(EvaluateBoard(fout), make_pair(7, 7));
    }

    int SearchXStart = 0;
    int SearchYStart = 0;
    int SearchXEnd = GAMEBOARD_SIZE;
    int SearchYEnd = GAMEBOARD_SIZE;

    if (GameInfo.BoardStart.first >= 2)
        SearchXStart = GameInfo.BoardStart.first - 2;
    if (GameInfo.BoardStart.second >= 2)
        SearchYStart = GameInfo.BoardStart.second - 2;
    if (GameInfo.BoardEnd.first < GAMEBOARD_SIZE - 3)
        SearchXEnd = GameInfo.BoardEnd.first + 3;
    if (GameInfo.BoardEnd.second < GAMEBOARD_SIZE - 3)
        SearchYEnd = GameInfo.BoardEnd.second + 3;

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
                    POINT tempStart = GameInfo.BoardStart;
                    POINT tempEnd = GameInfo.BoardEnd;
                    GameInfo.BoardAddSpot(make_pair(x, y), GameInfo.Player);

                    EvaluateValue = AlphaBeta(Depth - 1, a, b, false, fout);
                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }
                    if (value.first >= b)
                    {
                        GameInfo.BoardRemoveSpot(make_pair(x, y));
                        GameInfo.BoardStart = tempStart;
                        GameInfo.BoardEnd = tempEnd;
                        break;
                    }

                    a = max(a, value.first);

                    GameInfo.BoardRemoveSpot(make_pair(x, y));
                    GameInfo.BoardStart = tempStart;
                    GameInfo.BoardEnd = tempEnd;
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
                    POINT tempStart = GameInfo.BoardStart;
                    POINT tempEnd = GameInfo.BoardEnd;
                    GameInfo.BoardAddSpot(make_pair(x, y), GameInfo.Rival);

                    EvaluateValue = AlphaBeta(Depth - 1, a, b, false, fout);
                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    if (value.first <= a)
                    {
                        GameInfo.BoardRemoveSpot(make_pair(x, y));
                        GameInfo.BoardStart = tempStart;
                        GameInfo.BoardEnd = tempEnd;
                        break;
                    }

                    b = min(b, value.first);

                    GameInfo.BoardRemoveSpot(make_pair(x, y));
                    GameInfo.BoardStart = tempStart;
                    GameInfo.BoardEnd = tempEnd;
                }
            }
        }

        return value;
    }
}