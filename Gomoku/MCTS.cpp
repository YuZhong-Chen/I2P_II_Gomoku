#include <algorithm>
#include <array>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits> // numeric_limits<DataType>::max()
#include <map>
#include <queue>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

#define GAMEBOARD_SIZE 15
#define GAMEBOARD_TOTALPOINT_COUNT 225
// #define DEBUG

enum POINT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

using POINT = pair<short, short>;

const short Direction[4][2] = {{1, 0}, {1, -1}, {0, -1}, {-1, -1}};
const short DirectionReverse[4][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}};

void ShowPoint(POINT pt, ofstream &fout);

class NODE
{
public:
    POINT BoardStart;
    POINT BoardEnd;
    short BoardSizeX;
    short BoardSizeY;

    short BoardChessCount;
    map<POINT, short> ChessPoints;

    vector<NODE> children;

    NODE()
    {
        BoardStart = make_pair(GAMEBOARD_SIZE - 1, GAMEBOARD_SIZE - 1);
        BoardEnd = make_pair(0, 0);

        BoardSizeX = 15;
        BoardSizeY = 15;
        BoardChessCount = 0;
    }

    short Selection();
    bool RolloutSimulation(bool PlayerTurn);
    bool isGameOver();
};

class GAMECONTROL
{
public:
    short PLAYER;
    short RIVAL;

    NODE root;
    POINT answer;

    void Selection(ofstream &fout);
    void Search(POINT pt);
    void ReadBoardFromFile(ifstream &fin);
    void PrintBoard(ofstream &fout);
    POINT GetRandomPoint(POINT begin, POINT end);
} GameControl;

int main(int, char **argv)
{
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    GameControl.ReadBoardFromFile(fin);

    if (GameControl.root.BoardChessCount == 0)
    {
        fout << "7 7\n";
        fout.flush();
    }
    else
    {
        srand(time(NULL));

        GameControl.PrintBoard(fout);

        for (int i = 0; i < 10; i++)
            fout << GameControl.root.RolloutSimulation(true) << "\n";
    }

    fin.close();
    fout.close();
    return 0;
}

void ShowPoint(POINT pt, ofstream &fout)
{
    fout << pt.second << ' ' << pt.first << '\n';
}

void GAMECONTROL::Selection(ofstream &fout)
{
}

void GAMECONTROL::ReadBoardFromFile(ifstream &fin)
{
    fin >> PLAYER;
    RIVAL = 3 - PLAYER;

    short input_state;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            fin >> input_state;
            if (input_state != EMPTY)
            {
                root.ChessPoints.insert(make_pair(make_pair(x, y), input_state));
            }
        }
    }

    root.BoardChessCount = root.ChessPoints.size();

    for (auto it : root.ChessPoints)
    {
        root.BoardStart.first = min(it.first.first, root.BoardStart.first);
        root.BoardStart.second = min(it.first.second, root.BoardStart.second);
        root.BoardEnd.first = max(it.first.first, root.BoardEnd.first);
        root.BoardEnd.second = max(it.first.second, root.BoardEnd.second);
    }
}

void GAMECONTROL::Search(POINT pt)
{
}

void GAMECONTROL::PrintBoard(ofstream &fout)
{
    array<array<short, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
            Board[x][y] = EMPTY;

    for (auto it : root.ChessPoints)
        Board[it.first.first][it.first.second] = it.second;

    fout << "===============================\n";
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        fout << "|";
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            if (Board[x][y] == BLACK)
                fout << "O";
            else if (Board[x][y] == WHITE)
                fout << "X";
            else
                fout << ".";

            if (x + 1 != GAMEBOARD_SIZE)
                fout << " ";
        }
        fout << "|\n";
    }
    fout << "===============================\n";
}

POINT GAMECONTROL::GetRandomPoint(POINT begin, POINT end)
{
    return make_pair(rand() % (end.first - begin.first + 1) + begin.first, rand() % (end.second - begin.second + 1) + begin.second);
}

bool NODE::RolloutSimulation(bool PlayerTurn)
{
    array<array<short, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
            Board[x][y] = EMPTY;

    for (auto it : ChessPoints)
        Board[it.first.first][it.first.second] = it.second;

    short turn = PlayerTurn ? GameControl.PLAYER : GameControl.RIVAL;
    POINT pt;
    short L_count, R_count;
    short totalChess = BoardChessCount;

    POINT BoardRangeStart = BoardStart;
    POINT BoardRangeEnd = BoardEnd;
    BoardRangeStart.first = (BoardStart.first >= 2) ? BoardStart.first - 2 : 0;
    BoardRangeStart.second = (BoardStart.second >= 2) ? BoardStart.second - 2 : 0;
    BoardRangeEnd.first = (BoardEnd.first < GAMEBOARD_SIZE - 2) ? BoardEnd.first + 2 : GAMEBOARD_SIZE - 1;
    BoardRangeEnd.second = (BoardEnd.second < GAMEBOARD_SIZE - 2) ? BoardEnd.second + 2 : GAMEBOARD_SIZE - 1;

    while (true)
    {
        if (totalChess >= GAMEBOARD_TOTALPOINT_COUNT)
        {
            return false;
        }

        while (true)
        {
            pt = GameControl.GetRandomPoint(BoardRangeStart, BoardRangeEnd);

            if (Board[pt.first][pt.second] == EMPTY)
            {
                Board[pt.first][pt.second] = turn;
                totalChess++;
                BoardStart.first = min(pt.first, BoardStart.first);
                BoardStart.second = min(pt.second, BoardStart.second);
                BoardEnd.first = max(pt.first, BoardEnd.first);
                BoardEnd.second = max(pt.second, BoardEnd.second);

                BoardRangeStart.first = (BoardStart.first >= 2) ? BoardStart.first - 2 : 0;
                BoardRangeStart.second = (BoardStart.second >= 2) ? BoardStart.second - 2 : 0;
                BoardRangeEnd.first = (BoardEnd.first < GAMEBOARD_SIZE - 2) ? BoardEnd.first + 2 : GAMEBOARD_SIZE - 1;
                BoardRangeEnd.second = (BoardEnd.second < GAMEBOARD_SIZE - 2) ? BoardEnd.second + 2 : GAMEBOARD_SIZE - 1;
                break;
            }
        }

        for (short dir = 0; dir < 4; dir++)
        {
            L_count = R_count = 0;
            for (short x = pt.first + Direction[dir][0], y = pt.second + Direction[dir][1]; x >= 0 && x < GAMEBOARD_SIZE && y >= 0 && y < GAMEBOARD_SIZE; x += Direction[dir][0], y += Direction[dir][1])
            {
                if (Board[x][y] == Board[pt.first][pt.second])
                    R_count++;
                else
                    break;
            }
            for (short x = pt.first + DirectionReverse[dir][0], y = pt.second + DirectionReverse[dir][1]; x >= 0 && x < GAMEBOARD_SIZE && y >= 0 && y < GAMEBOARD_SIZE; x += DirectionReverse[dir][0], y += DirectionReverse[dir][1])
            {
                if (Board[x][y] == Board[pt.first][pt.second])
                    R_count++;
                else
                    break;
            }
            if (L_count + R_count >= 4)
                return (turn == GameControl.PLAYER);
        }

        turn = 3 - turn;
    }

    return true;
}