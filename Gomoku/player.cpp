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
    bool isGameOver();
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
    POINT EvaluateChessLine(short Player, short Rival, ofstream &fout);

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

    // GameControl.UpdateChessLine(make_pair(4, 14), 0);
    // GameControl.EvaluateChessLine(GameInfo.Board[4][14], 3 - GameInfo.Board[4][14], fout);

    // for (int i = 0; i < 9; i++)
    // {
    //     if (GameControl.ChessLine[i] == BLACK)
    //         fout << "O";
    //     else if (GameControl.ChessLine[i] == WHITE)
    //         fout << "X";
    //     else
    //         fout << ".";
    // }
    // fout << "\n\n";

    // GameControl.EvaluatePoint(make_pair(6, 6), fout);

    // GameControl.PrintBoard(fout);
    // fout << GameControl.EvaluateBoard(fout) << "\n";
    // for (int i = 0; i < 7; i++)
    // {
    //     fout << "Player Type " << i + 1 << " : " << GameControl.TypeCount[GameInfo.Player][i] << "\n";
    // }
    // fout << "\n";
    // for (int i = 0; i < 7; i++)
    // {
    //     fout << "Rival Type " << i + 1 << " : " << GameControl.TypeCount[GameInfo.Rival][i] << "\n";
    // }
    // fout << "\n";

    if (GameInfo.EmptyChessCount != GAMEBOARD_SIZE * GAMEBOARD_SIZE)
    {
        // if (GameInfo.ChessCount[EMPTY] < 200)
        //     GameControl.SearchDepth = 2;
        // else
        //     GameControl.SearchDepth = 3;

        // NODE spot = GameControl.Minimax(GameControl.SearchDepth, true, fout);

        // GameControl.SearchDepth = 1;
        // // NODE spot5 = GameControl.Minimax(GameControl.SearchDepth, true, fout);
        // // fout << spot5.second.second << ' ' << spot5.second.first << '\n';
        // NODE spot6 = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        // fout << spot6.second.second << ' ' << spot6.second.first << '\n';
        // fout.flush();

        GameControl.SearchDepth = 2;
        // NODE spot1 = GameControl.Minimax(GameControl.SearchDepth, true, fout);
        // fout << spot1.second.second << ' ' << spot1.second.first << '\n';
        NODE spot2 = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        fout << spot2.second.second << ' ' << spot2.second.first << '\n';
        fout.flush();

        // GameControl.SearchDepth = 3;
        // // NODE spot3 = GameControl.Minimax(GameControl.SearchDepth, true, fout);
        // // fout << spot3.second.second << ' ' << spot3.second.first << '\n';
        // NODE spot4 = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        // fout << spot4.second.second << ' ' << spot4.second.first << '\n';
        // fout.flush();

        // GameControl.SearchDepth = 4;
        // NODE spot7 = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        // fout << spot7.second.second << ' ' << spot7.second.first << '\n';
        // fout.flush();

        // GameControl.SearchDepth = 5;
        // NODE spot8 = GameControl.AlphaBeta(GameControl.SearchDepth, INT_MIN, INT_MAX, true, fout);
        // fout << spot8.second.second << ' ' << spot8.second.first << '\n';
        // fout.flush();
    }
    else
    {
        srand(time(NULL));
        int x = rand() % GAMEBOARD_SIZE;
        int y = rand() % GAMEBOARD_SIZE;
        fout << x << ' ' << y << "\n";
        fout.flush();
    }

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
    POINT CheckRange;
    for (int dir = 0; dir < 4; dir++)
    {
        if (!GameInfo.BoardState[pt.first][pt.second][dir])
        {
            UpdateChessLine(pt, dir);
            CheckRange = EvaluateChessLine(GameInfo.Board[pt.first][pt.second], 3 - GameInfo.Board[pt.first][pt.second], fout);
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

POINT GAMECONTROL::EvaluateChessLine(short Player, short Rival, ofstream &fout)
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

    if (total_range < 5)
        return return_range;

    if (total_continuous >= 5) // OOOOO
    {
        TypeCount[Player][FiveInRow] += 1;
        return return_range;
    }

    bool L_open = (ChessLine[L_continuous - 1] != Rival);
    bool R_open = (ChessLine[R_continuous + 1] != Rival);
    if (total_continuous == 4)
    {
        if (L_open && R_open) // .OOOO.
            TypeCount[Player][LiveFour] += 1;
        else if (L_open || R_open) // XOOOO.
            TypeCount[Player][DeadFour] += 1;
    }
    else if (total_continuous == 3)
    {
        bool isFour = false;
        if (L_open && ChessLine[L_continuous - 2] == Player) // O.OOO
        {
            return_range.first -= 2;
            TypeCount[Player][DeadFour] += 1;
            isFour = true;
        }
        if (R_open && ChessLine[R_continuous + 2] == Player) // OOO.O
        {
            return_range.second += 2;
            TypeCount[Player][DeadFour] += 1;
            isFour = true;
        }

        if (!isFour)
        {
            if (L_open && R_open)
            {
                if (total_range == 5) // X.OOO.X
                {
                    TypeCount[Player][DeadThree] += 1;
                }
                else // ..OOO..
                {
                    TypeCount[Player][LiveThree] += 1;
                }
            }
            else if (L_open || R_open) // XOOO.
            {
                TypeCount[Player][DeadThree] += 1;
            }
        }
    }
    else if (total_continuous == 2)
    {
        bool isThree = false;
        if (L_open && ChessLine[L_continuous - 2] == Player)
        {
            return_range.first -= 2;
            if (ChessLine[L_continuous - 3] == EMPTY) // O.OO
            {
                if (ChessLine[R_continuous + 1] == EMPTY) // .O.OO.
                {
                    TypeCount[Player][LiveThree] += 1;
                }
                else // .O.OOX
                {
                    TypeCount[Player][DeadThree] += 1;
                }

                isThree = true;
            }
            else if (ChessLine[L_continuous - 3] == Rival && ChessLine[R_continuous + 1] == EMPTY) // XO.OO.
            {
                TypeCount[Player][DeadThree] += 1;
                isThree = true;
            }
        }
        if (R_open && ChessLine[R_continuous + 2] == Player) // OO.O
        {
            return_range.second += 2;
            if (ChessLine[R_continuous + 3] == EMPTY)
            {
                if (ChessLine[L_continuous - 1] == EMPTY) // .OO.O.
                {
                    TypeCount[Player][LiveThree] += 1;
                }
                else // XOO.O.
                {
                    TypeCount[Player][DeadThree] += 1;
                }

                isThree = true;
            }
            else if (ChessLine[R_continuous + 3] == Rival && ChessLine[L_continuous - 1] == EMPTY) // .OO.OX
            {
                TypeCount[Player][DeadThree] += 1;
                isThree = true;
            }
        }

        if (!isThree)
        {
            if (L_open && R_open) // .OO.
            {
                TypeCount[Player][LiveTwo] += 1;
            }
            else if (L_open || R_open) // XOO.
            {
                TypeCount[Player][DeadTwo] += 1;
            }
        }
    }
    else if (total_continuous == 1)
    {
        bool isTwo = false;
        // .O.OX
        if (ChessLine[L_continuous - 1] == EMPTY && ChessLine[L_continuous - 2] == Player && ChessLine[L_continuous - 3] == EMPTY && ChessLine[R_continuous + 1] == Rival)
        {
            TypeCount[Player][DeadTwo] += 1;
            isTwo = true;
        }

        if (ChessLine[R_continuous + 1] == EMPTY && ChessLine[R_continuous + 2] == Player && ChessLine[R_continuous + 3] == EMPTY)
        {
            if (!isTwo) // XO.O.
            {
                TypeCount[Player][DeadTwo] += 1;
            }
            else // .O.O.
            {
                TypeCount[Player][LiveTwo] += 1;
            }
        }
        else if (ChessLine[R_continuous + 1] == EMPTY && ChessLine[R_continuous + 2] == EMPTY && ChessLine[R_continuous + 3] == Player && ChessLine[R_continuous + 4] == EMPTY)
        {
            // .O..O.
            TypeCount[Player][LiveTwo] += 1;
        }
    }

    return return_range;
}

int GAMECONTROL::EvaluateBoard(ofstream &fout)
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
                EvaluatePoint(make_pair(x, y), fout);

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
    fout << "\nValue : " << Value[GameInfo.Player] - Value[GameInfo.Rival] << "\n";
#endif

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
                    EvaluateValue.first = GameInfo.isGameOver() ? INT_MIN : EvaluateValue.first;
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

                    EvaluateValue = Minimax(Depth - 1, true, fout);
                    EvaluateValue.first = GameInfo.isGameOver() ? INT_MAX : EvaluateValue.first;
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

                    EvaluateValue = AlphaBeta(Depth - 1, A, B, false, fout);
                    EvaluateValue.first = GameInfo.isGameOver() ? 1000000 : EvaluateValue.first;

                    if (EvaluateValue.first > value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    if (value.first >= B)
                    {
                        GameInfo.BoardRemoveSpot(make_pair(x, y));
                        GameInfo.BoardStart = tempStart;
                        GameInfo.BoardEnd = tempEnd;
                        return value;
                    }
                    A = max(A, value.first);

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

                    EvaluateValue = AlphaBeta(Depth - 1, A, B, true, fout);
                    // EvaluateValue.first = GameInfo.isGameOver() ? -1000000 : EvaluateValue.first;

                    if (EvaluateValue.first < value.first)
                    {
                        value.first = EvaluateValue.first;
                        value.second.first = x;
                        value.second.second = y;
                    }

                    if (value.first <= A)
                    {
                        GameInfo.BoardRemoveSpot(make_pair(x, y));
                        GameInfo.BoardStart = tempStart;
                        GameInfo.BoardEnd = tempEnd;
                        return value;
                    }

                    B = min(B, value.first);

                    GameInfo.BoardRemoveSpot(make_pair(x, y));
                    GameInfo.BoardStart = tempStart;
                    GameInfo.BoardEnd = tempEnd;
                }
            }
        }

        return value;
    }
}