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

enum SPOT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;

void read_board(std::ifstream &fin)
{
    fin >> player;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fin >> board[i][j];
        }
    }
}

void write_valid_spot(std::ofstream &fout)
{
    srand(time(NULL));
    int x, y;
    // Keep updating the output until getting killed.
    while (true)
    {
        // Choose a random spot.
        x = (rand() % SIZE);
        y = (rand() % SIZE);
        if (board[x][y] == EMPTY)
        {
            fout << x << " " << y << std::endl;
            // Remember to flush the output to ensure the last action is written to file.
            fout.flush();
            break;
        }
    }
}

int main(int, char **argv)
{
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);

    read_board(fin);
    write_valid_spot(fout);

    fin.close();
    fout.close();
    return 0;
}
