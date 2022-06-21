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

int totalSimulationCount = 0;
POINT OverPoint;

void ShowPoint(POINT pt, ofstream &fout);

short min(short a, short b)
{
    return (a < b) ? a : b;
}
short max(short a, short b)
{
    return (a < b) ? b : a;
}

class NODE
{
public:
    POINT Move;
    POINT BoardStart;
    POINT BoardEnd;
    short BoardChessCount;
    map<POINT, short> ChessPoints;

    int depth;

    long long WinCount;
    int totalSearch;
    vector<NODE> children;

    NODE()
    {
        BoardStart = make_pair(GAMEBOARD_SIZE - 1, GAMEBOARD_SIZE - 1);
        BoardEnd = make_pair(0, 0);

        BoardChessCount = WinCount = totalSearch = depth = 0;
    }

    void Expansion(ofstream &fout);
    bool Selection(ofstream &fout);
    double EvaluateNode(ofstream &fout);
    bool RolloutSimulation(ofstream &fout);
    void PrintBoard(ofstream &fout);
    bool isOver();
};

class GAMECONTROL
{
public:
    short PLAYER;
    short RIVAL;

    short TreeDepth = 0;
    NODE root;
    POINT answer;

    void Search(ofstream &fout);
    void ReadBoardFromFile(ifstream &fin);
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

        // int Win = 0, Lose = 0;

        // while (Win + Lose < 100000)
        // {
        //     if (GameControl.root.RolloutSimulation(fout))
        //         Win++;
        //     else
        //         Lose++;
        // }
        // fout << Win << ' ' << Lose << " " << Win / (double)(Win + Lose) << endl;

        int total = 0;
        int t = 0;
        while (true)
        {
            if (++total < 5000)
            {
                GameControl.root.Selection(fout);
            }
            else
            {
                GameControl.Search(fout);
                ShowPoint(GameControl.answer, fout);
                fout.flush();
                total = 0;
                t++;
            }
        }

        // fout << "root : " << GameControl.root.WinCount << " " << GameControl.root.totalSearch << " " << GameControl.root.EvaluateNode(fout) << endl;

        // fout << endl;
        // for (int i = 0; i < GameControl.root.children.size(); i++)
        // {
        //     fout << i << " : " << GameControl.root.children[i].WinCount << " " << GameControl.root.children[i].totalSearch << " " << GameControl.root.children[i].EvaluateNode(fout) << endl;
        //     ShowPoint(GameControl.root.children[i].Move, fout);
        // }
    }

    fin.close();
    fout.close();
    return 0;
}

void ShowPoint(POINT pt, ofstream &fout)
{
    fout << pt.second << ' ' << pt.first << '\n';
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

void GAMECONTROL::Search(ofstream &fout)
{
    root.Selection(fout);

    short size = root.children.size();
    int ChooseChild;
    double ChooseValue = -1;
    double value;

    for (int i = 0; i < size; i++)
    {
        // value = root.children[i].totalSearch;
        value = root.children[i].EvaluateNode(fout);

        if (value > ChooseValue)
        {
            ChooseChild = i;
            ChooseValue = value;
        }
    }

    answer = root.children[ChooseChild].Move;
}

bool NODE::Selection(ofstream &fout)
{
    if (children.empty())
    {
        Expansion(fout);
        if (children.empty())
            return 0;

        if (children[0].RolloutSimulation(fout))
            children[0].WinCount += 1;

        children[0].totalSearch++;
        totalSearch++;
        WinCount += children[0].WinCount;

        return (children[0].WinCount == 1);
    }

    short size = children.size();
    int ChooseChild;
    double ChooseValue = -1;
    double value;

    for (int i = 0; i < size; i++)
    {
        value = children[i].EvaluateNode(fout);

        if (value > ChooseValue)
        {
            ChooseChild = i;
            ChooseValue = value;

            if (value == numeric_limits<double>::max())
                break;
        }
    }

    if (value == numeric_limits<double>::max())
    {
        if (children[ChooseChild].RolloutSimulation(fout))
            children[ChooseChild].WinCount += 1;

        children[ChooseChild].totalSearch++;
        totalSearch++;
        WinCount += children[0].WinCount;

        return (children[0].WinCount == 1);
    }
    else
    {
        totalSearch++;
        if (children[ChooseChild].Selection(fout))
        {
            WinCount += 1;
            return true;
        }
        else
        {
            return false;
        }
    }
}

void NODE::Expansion(ofstream &fout)
{
    NODE temp;
    temp.BoardChessCount = BoardChessCount + 1;
    temp.depth = depth + 1;

    children.clear();

    POINT BoardRangeStart;
    POINT BoardRangeEnd;
    BoardRangeStart.first = (BoardStart.first >= 1) ? BoardStart.first - 1 : 0;
    BoardRangeStart.second = (BoardStart.second >= 1) ? BoardStart.second - 1 : 0;
    BoardRangeEnd.first = (BoardEnd.first < GAMEBOARD_SIZE - 1) ? BoardEnd.first + 1 : GAMEBOARD_SIZE - 1;
    BoardRangeEnd.second = (BoardEnd.second < GAMEBOARD_SIZE - 1) ? BoardEnd.second + 1 : GAMEBOARD_SIZE - 1;

    for (int y = BoardRangeStart.second; y <= BoardRangeEnd.second; y++)
    {
        for (int x = BoardRangeStart.first; x <= BoardRangeEnd.first; x++)
        {
            if (ChessPoints.find(make_pair(x, y)) == ChessPoints.end())
            {
                temp.Move = make_pair(x, y);

                temp.BoardStart.first = min(BoardStart.first, x);
                temp.BoardStart.second = min(BoardStart.second, y);
                temp.BoardEnd.first = max(BoardEnd.first, x);
                temp.BoardEnd.second = max(BoardEnd.second, y);

                temp.ChessPoints = ChessPoints;
                temp.ChessPoints.insert(make_pair(make_pair(x, y), (depth % 2 == 0) ? GameControl.PLAYER : GameControl.RIVAL));

                children.push_back(temp);
            }
        }
    }
}

void NODE::PrintBoard(ofstream &fout)
{
    array<array<short, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
            Board[x][y] = EMPTY;

    for (auto it : ChessPoints)
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

bool NODE::RolloutSimulation(ofstream &fout)
{
    totalSimulationCount++;

    array<array<short, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
            Board[x][y] = EMPTY;

    for (auto it : ChessPoints)
        Board[it.first.first][it.first.second] = it.second;

    short turn = (depth % 2 == 1) ? GameControl.PLAYER : GameControl.RIVAL;
    POINT pt;
    short L_count, R_count;
    short totalChess = BoardChessCount;

    for (int y = BoardStart.second; y <= BoardEnd.second; y++)
    {
        for (int x = BoardStart.first; x < BoardEnd.first; x++)
        {
            if (Board[x][y] == EMPTY)
                continue;

            if (x + 4 < GAMEBOARD_SIZE)
            {
                if (Board[x][y] == Board[x + 1][y] && Board[x][y] == Board[x + 2][y] && Board[x][y] == Board[x + 3][y] && Board[x][y] == Board[x + 4][y])
                {
                    return (Board[OverPoint.first][OverPoint.second] == GameControl.PLAYER);
                }

                if (y + 4 < GAMEBOARD_SIZE)
                {
                    if (Board[x][y] == Board[x + 1][y + 1] && Board[x][y] == Board[x + 2][y + 2] && Board[x][y] == Board[x + 3][y + 3] && Board[x][y] == Board[x + 4][y + 4])
                    {
                        return (Board[OverPoint.first][OverPoint.second] == GameControl.PLAYER);
                    }
                }
            }
            if (y + 4 < GAMEBOARD_SIZE)
            {
                if (Board[x][y] == Board[x][y + 1] && Board[x][y] == Board[x][y + 2] && Board[x][y] == Board[x][y + 3] && Board[x][y] == Board[x][y + 4])
                {
                    return (Board[OverPoint.first][OverPoint.second] == GameControl.PLAYER);
                }

                if (x >= 4)
                {
                    if (Board[x][y] == Board[x - 1][y + 1] && Board[x][y] == Board[x - 2][y + 2] && Board[x][y] == Board[x - 3][y + 3] && Board[x][y] == Board[x - 4][y + 4])
                    {
                        return (Board[OverPoint.first][OverPoint.second] == GameControl.PLAYER);
                    }
                }
            }
        }
    }

    POINT BoardRangeStart;
    POINT BoardRangeEnd;
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

    return false;
}

bool NODE::isOver()
{
    array<array<short, GAMEBOARD_SIZE>, GAMEBOARD_SIZE> Board;
    for (int y = 0; y < GAMEBOARD_SIZE; y++)
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
            Board[x][y] = EMPTY;

    for (auto it : ChessPoints)
        Board[it.first.first][it.first.second] = it.second;

    for (int y = 0; y < GAMEBOARD_SIZE; y++)
    {
        for (int x = 0; x < GAMEBOARD_SIZE; x++)
        {
            if (Board[x][y] == EMPTY)
                continue;

            if (x + 4 < GAMEBOARD_SIZE)
            {
                if (Board[x][y] == Board[x + 1][y] && Board[x][y] == Board[x + 2][y] && Board[x][y] == Board[x + 3][y] && Board[x][y] == Board[x + 4][y])
                {
                    OverPoint = make_pair(x, y);
                    return true;
                }

                if (y + 4 < GAMEBOARD_SIZE)
                {
                    if (Board[x][y] == Board[x + 1][y + 1] && Board[x][y] == Board[x + 2][y + 2] && Board[x][y] == Board[x + 3][y + 3] && Board[x][y] == Board[x + 4][y + 4])
                    {
                        OverPoint = make_pair(x, y);
                        return true;
                    }
                }
            }
            if (y + 4 < GAMEBOARD_SIZE)
            {
                if (Board[x][y] == Board[x][y + 1] && Board[x][y] == Board[x][y + 2] && Board[x][y] == Board[x][y + 3] && Board[x][y] == Board[x][y + 4])
                {
                    OverPoint = make_pair(x, y);
                    return true;
                }

                if (x >= 4)
                {
                    if (Board[x][y] == Board[x - 1][y + 1] && Board[x][y] == Board[x - 2][y + 2] && Board[x][y] == Board[x - 3][y + 3] && Board[x][y] == Board[x - 4][y + 4])
                    {
                        OverPoint = make_pair(x, y);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

double NODE::EvaluateNode(ofstream &fout)
{
    if (totalSearch == 0)
        return numeric_limits<double>::max();

    return ((WinCount / (double)totalSearch) + sqrt(2 * log(totalSimulationCount) / (double)totalSearch));
}