#include <algorithm>
#include <array>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <unordered_map>
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
    bool isGameOver();
} GAMEINFO;
GAMEINFO GameInfo;

class GAMECONTROL
{
public:
    int SearchDepth = 2;

    short ChessLine[9];

    short TypeCount[3][7];

    NODE Minimax(int Depth, bool maximizingPlayer);
    NODE AlphaBeta(int Depth, int a, int b, bool maximizingPlayer);

    int EvaluateBoard();
    void UpdateChessLine(POINT pt, short dir);
    void EvaluatePoint(POINT pt);
    POINT EvaluateChessLine(short Player, short Rival);

    void ReadBoard(ifstream &fin);
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

    GameControl.ReadBoard(fin);

#ifdef DEBUG
    GameControl.PrintBoard(fout);
    fout << GameControl.EvaluateBoard() << "\n";
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
    fout << '\n';
#endif

    if (GameInfo.EmptyChessCount != GAMEBOARD_SIZE * GAMEBOARD_SIZE)
    {
        NODE answer;
        for (int depth = 2; depth <= 3; depth++)
        {
            GameControl.SearchDepth = depth;

#ifdef DEBUG
            answer = GameControl.Minimax(GameControl.SearchDepth, true);
            fout << answer.second.second << ' ' << answer.second.first << '\n';
#endif

            answer = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true);
            fout << answer.second.second << ' ' << answer.second.first << '\n';

            fout.flush();
        }
    }
    else
    {
        // srand(time(NULL));
        // int x = rand() % GAMEBOARD_SIZE;
        // int y = rand() % GAMEBOARD_SIZE;
        // fout << x << ' ' << y << "\n";
        fout << "7 7\n";
        fout.flush();
    }

    fin.close();
    fout.close();
    return 0;
}

void GAMECONTROL::ReadBoard(ifstream &fin)
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
#ifdef DEBUG
    fout << "EMPTY : " << GameInfo.EmptyChessCount << "\n";
    fout << "X boundary : " << GameInfo.BoardStart.first << " " << GameInfo.BoardEnd.first << "\n";
    fout << "Y boundary : " << GameInfo.BoardStart.second << " " << GameInfo.BoardEnd.second << "\n";
#endif
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

void GAMECONTROL::EvaluatePoint(POINT pt)
{
    POINT CheckRange;
    for (int dir = 0; dir < 4; dir++)
    {
        if (!GameInfo.BoardState[pt.first][pt.second][dir])
        {
            UpdateChessLine(pt, dir);
            CheckRange = EvaluateChessLine(GameInfo.Board[pt.first][pt.second], 3 - GameInfo.Board[pt.first][pt.second]);
            for (int i = 4 - CheckRange.first; i >= 0; i--)
            {
                GameInfo.BoardState[pt.first + i * DirectionReverse[dir][0]][pt.second + i * DirectionReverse[dir][1]][dir] = true;
            }
            for (int i = CheckRange.second - 4; i > 0; i--)
            {
                GameInfo.BoardState[pt.first + i * Direction[dir][0]][pt.second + i * Direction[dir][1]][dir] = true;
            }
        }
    }
}

void GAMECONTROL::UpdateChessLine(POINT pt, short dir)
{
    short rival = 3 - GameInfo.Board[pt.first][pt.second];
    int x = pt.first + 4 * DirectionReverse[dir][0];
    int y = pt.second + 4 * DirectionReverse[dir][1];
    for (int i = 0; i < 9; i++)
    {
        if (x >= 0 && x < GAMEBOARD_SIZE && y >= 0 && y < GAMEBOARD_SIZE)
        {
            ChessLine[i] = GameInfo.Board[x][y];
        }
        else
        {
            ChessLine[i] = rival;
        }

        x += Direction[dir][0];
        y += Direction[dir][1];
    }
}

POINT GAMECONTROL::EvaluateChessLine(short Player, short Rival) // .... . ....
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

    short L_end = L_continuous;
    short R_end = R_continuous;

    for (; L_end >= 0; L_end--)
        if (ChessLine[L_end - 1] == Rival)
            break;

    for (; R_end <= 8; R_end++)
        if (ChessLine[R_end + 1] == Rival)
            break;

    short total_range = R_end - L_end + 1;
    POINT return_range = make_pair(L_continuous, R_continuous);

    if (total_range < 5) // Impossible point for answer
        return return_range;

    if (total_continuous >= 5) // OOOOO
    {
        TypeCount[Player][FiveInRow] += 1;
        return return_range;
    }

    bool L_open = (ChessLine[L_continuous - 1] != Rival);
    bool R_open = (ChessLine[R_continuous + 1] != Rival);
    if (total_continuous == 1)
    {
        bool isTwo = false;
        if (ChessLine[L_continuous - 1] == EMPTY && ChessLine[L_continuous - 2] == Player && ChessLine[L_continuous - 3] == EMPTY && ChessLine[R_continuous + 1] == Rival)
        {
            TypeCount[Player][DeadTwo] += 1; // .O.OX
            isTwo = true;
        }

        if (ChessLine[R_continuous + 1] == EMPTY && ChessLine[R_continuous + 2] == Player && ChessLine[R_continuous + 3] == EMPTY)
        {
            if (!isTwo)
                TypeCount[Player][DeadTwo] += 1; // XO.O.
            else
                TypeCount[Player][LiveTwo] += 1; // .O.O.
        }
        else if (ChessLine[R_continuous + 1] == EMPTY && ChessLine[R_continuous + 2] == EMPTY && ChessLine[R_continuous + 3] == Player && ChessLine[R_continuous + 4] == EMPTY)
            TypeCount[Player][LiveTwo] += 1; // .O..O.
    }
    else if (total_continuous == 2)
    {
        bool isThree = false;
        if (L_open && ChessLine[L_continuous - 2] == Player)
        {
            return_range.first -= 2;
            if (ChessLine[L_continuous - 3] == EMPTY) // O.OO
            {
                if (ChessLine[R_continuous + 1] == EMPTY)
                    TypeCount[Player][LiveThree] += 1; // .O.OO.
                else
                    TypeCount[Player][DeadThree] += 1; // .O.OOX

                isThree = true;
            }
            else if (ChessLine[L_continuous - 3] == Rival && ChessLine[R_continuous + 1] == EMPTY)
            {
                TypeCount[Player][DeadThree] += 1; // XO.OO.
                isThree = true;
            }
        }
        if (R_open && ChessLine[R_continuous + 2] == Player) // OO.O
        {
            return_range.second += 2;
            if (ChessLine[R_continuous + 3] == EMPTY)
            {
                if (ChessLine[L_continuous - 1] == EMPTY)
                    TypeCount[Player][LiveThree] += 1; // .OO.O.
                else
                    TypeCount[Player][DeadThree] += 1; // XOO.O.

                isThree = true;
            }
            else if (ChessLine[R_continuous + 3] == Rival && ChessLine[L_continuous - 1] == EMPTY)
            {
                TypeCount[Player][DeadThree] += 1; // .OO.OX
                isThree = true;
            }
        }

        if (!isThree)
        {
            if (L_open && R_open)
                TypeCount[Player][LiveTwo] += 1; // .OO.
            else if (L_open || R_open)
                TypeCount[Player][DeadTwo] += 1; // XOO.
        }
    }
    else if (total_continuous == 3)
    {
        bool isFour = false;
        if (L_open && ChessLine[L_continuous - 2] == Player)
        {
            return_range.first -= 2;
            TypeCount[Player][DeadFour] += 1; // O.OOO
            isFour = true;
        }
        if (R_open && ChessLine[R_continuous + 2] == Player)
        {
            return_range.second += 2;
            TypeCount[Player][DeadFour] += 1; // OOO.O
            isFour = true;
        }

        if (!isFour)
        {
            if (L_open && R_open)
            {
                if (total_range == 5)
                    TypeCount[Player][DeadThree] += 1; // X.OOO.X
                else
                    TypeCount[Player][LiveThree] += 1; // ..OOO..
            }
            else if (L_open || R_open)
                TypeCount[Player][DeadThree] += 1; // XOOO.
        }
    }
    else if (total_continuous == 4)
    {
        if (L_open && R_open)
            TypeCount[Player][LiveFour] += 1; // .OOOO.
        else if (L_open || R_open)
            TypeCount[Player][DeadFour] += 1; // XOOOO.
    }

    return return_range;
}

int GAMECONTROL::EvaluateBoard()
{
    for (int i = 0; i < 7; i++)
        GameControl.TypeCount[1][i] = GameControl.TypeCount[2][i] = 0;

    for (int y = GameInfo.BoardStart.second; y <= GameInfo.BoardEnd.second; y++)
        for (int x = GameInfo.BoardStart.first; x <= GameInfo.BoardEnd.first; x++)
            if (GameInfo.Board[x][y] != EMPTY)
                GameInfo.BoardState[x][y][0] = GameInfo.BoardState[x][y][1] = GameInfo.BoardState[x][y][2] = GameInfo.BoardState[x][y][3] = false;

    for (int y = GameInfo.BoardStart.second; y <= GameInfo.BoardEnd.second; y++)
        for (int x = GameInfo.BoardStart.first; x <= GameInfo.BoardEnd.first; x++)
            if (GameInfo.Board[x][y] != EMPTY)
                EvaluatePoint(make_pair(x, y));

    int Value[3];
    float Rate[3];
    Rate[GameInfo.Player] = 1.0;
    Rate[GameInfo.Rival] = 1.2;

    for (int i = 1; i <= 2; i++)
    {
        if (TypeCount[i][FiveInRow] >= 1)
            Value[i] = 1000000 * Rate[i];
        else if (TypeCount[i][LiveFour] >= 2)
            Value[i] = 8000 * Rate[i];
        else if (TypeCount[i][LiveFour] == 1)
            Value[i] = 6000 * Rate[i];
        else if (TypeCount[i][DeadFour] + TypeCount[i][LiveThree] >= 2)
            Value[i] = 3500 * Rate[i];
        else if (TypeCount[i][DeadFour] == 1)
            Value[i] = 1500 * Rate[i];
        else if (TypeCount[i][LiveThree] == 1)
            Value[i] = 1000 * Rate[i];
        else
            Value[i] = 100 * TypeCount[i][LiveTwo] * Rate[i] + 50 * TypeCount[i][DeadThree] * Rate[i] + 30 * TypeCount[i][DeadTwo] * Rate[i];
    }

    return Value[GameInfo.Player] - Value[GameInfo.Rival];
}

bool GAMEINFO::isGameOver()
{
    for (int y = GameInfo.BoardStart.second; y <= GameInfo.BoardEnd.second; y++)
    {
        for (int x = GameInfo.BoardStart.first; x <= GameInfo.BoardEnd.first; x++)
        {
            if (GameInfo.Board[x][y] == EMPTY)
                continue;

            if (x + 4 < GAMEBOARD_SIZE)
            {
                if (GameInfo.Board[x][y] == GameInfo.Board[x + 1][y] && GameInfo.Board[x][y] == GameInfo.Board[x + 2][y] && GameInfo.Board[x][y] == GameInfo.Board[x + 3][y] && GameInfo.Board[x][y] == GameInfo.Board[x + 4][y])
                {
                    return true;
                }

                if (y + 4 < GAMEBOARD_SIZE)
                {
                    if (GameInfo.Board[x][y] == GameInfo.Board[x + 1][y + 1] && GameInfo.Board[x][y] == GameInfo.Board[x + 2][y + 2] && GameInfo.Board[x][y] == GameInfo.Board[x + 3][y + 3] && GameInfo.Board[x][y] == GameInfo.Board[x + 4][y + 4])
                    {
                        return true;
                    }
                }
            }
            if (y + 4 < GAMEBOARD_SIZE)
            {
                if (GameInfo.Board[x][y] == GameInfo.Board[x][y + 1] && GameInfo.Board[x][y] == GameInfo.Board[x][y + 2] && GameInfo.Board[x][y] == GameInfo.Board[x][y + 3] && GameInfo.Board[x][y] == GameInfo.Board[x][y + 4])
                {
                    return true;
                }

                if (x >= 4)
                {
                    if (GameInfo.Board[x][y] == GameInfo.Board[x - 1][y + 1] && GameInfo.Board[x][y] == GameInfo.Board[x - 2][y + 2] && GameInfo.Board[x][y] == GameInfo.Board[x - 3][y + 3] && GameInfo.Board[x][y] == GameInfo.Board[x - 4][y + 4])
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

NODE GAMECONTROL::Minimax(int Depth, bool maximizingPlayer)
{
    if (Depth == 0 || GameInfo.EmptyChessCount == 0)
    {
        return make_pair(EvaluateBoard(), make_pair(7, 7));
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

                    EvaluateValue.first = GameInfo.isGameOver() ? INT_MIN : (Minimax(Depth - 1, false)).first;
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

                    EvaluateValue.first = GameInfo.isGameOver() ? INT_MAX : (Minimax(Depth - 1, true)).first;
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

NODE GAMECONTROL::AlphaBeta(int Depth, int a, int b, bool maximizingPlayer)
{
    if (Depth == 0 || GameInfo.EmptyChessCount == 0)
        return make_pair(EvaluateBoard(), make_pair(7, 7));

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

    int A = a, B = b;

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

                    EvaluateValue.first = GameInfo.isGameOver() ? 10000000 : (AlphaBeta(Depth - 1, A, B, false)).first;

                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    A = max(A, value.first);

                    if (A >= B)
                    {
                        GameInfo.BoardRemoveSpot(make_pair(x, y));
                        GameInfo.BoardStart = tempStart;
                        GameInfo.BoardEnd = tempEnd;
                        return value;
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

                    EvaluateValue.first = GameInfo.isGameOver() ? -10000000 : (AlphaBeta(Depth - 1, A, B, true)).first;

                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    B = min(B, value.first);

                    if (B <= A)
                    {
                        GameInfo.BoardRemoveSpot(make_pair(x, y));
                        GameInfo.BoardStart = tempStart;
                        GameInfo.BoardEnd = tempEnd;
                        return value;
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