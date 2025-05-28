#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int ROWS = 10;
const int COLS = 10;
const int WIN = 5;

struct Player
{
    string name;
    char symbol;
};

void printError(const string &msg)
{
    cout << "[GRESKA] " << msg << endl;
    cout << "Ako trebate pomoc, unesite 'h' ili 'help'." << endl;
}

void help()
{
    cout << "\n--- Pomoc ---\n"
         << "Unesite broj kolone (0-" << COLS - 1 << ") kada ste na potezu.\n"
         << "Simboli: X za prvog igraca, O za drugog igraca.\n"
         << "Mozete sacuvati igru ili prikazati leaderboard iz menija.\n"
         << "----------------\n";
}

void printBoard(char board[ROWS][COLS])
{
    cout << "\n   ";
    for (int i = 0; i < COLS; ++i)
        cout << i << " ";
    cout << "\n  ";
    for (int i = 0; i < COLS; ++i)
        cout << "--";
    cout << "-\n";
    for (int i = 0; i < ROWS; ++i)
    {
        cout << i << "|";
        for (int j = 0; j < COLS; ++j)
        {
            cout << board[i][j] << " ";
        }
        cout << "|\n";
    }
    cout << "  ";
    for (int i = 0; i < COLS; ++i)
        cout << "--";
    cout << "-\n";
}

bool makeMove(char board[ROWS][COLS], int col, char symbol)
{
    if (col < 0 || col >= COLS)
    {
        printError("Neispravan unos kolone!");
        return false;
    }
    for (int i = ROWS - 1; i >= 0; --i)
    {
        if (board[i][col] == '.')
        {
            board[i][col] = symbol;
            return true;
        }
    }
    printError("Kolona je puna!");
    return false;
}

bool checkWin(char board[ROWS][COLS], char sym)
{
    // horizontal, vertical, diagonal
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
        {
            if (board[i][j] != sym)
                continue;
            // horizontal
            if (j + WIN <= COLS)
            {
                int k;
                for (k = 0; k < WIN; ++k)
                    if (board[i][j + k] != sym)
                        break;
                if (k == WIN)
                    return true;
            }
            // vertical
            if (i + WIN <= ROWS)
            {
                int k;
                for (k = 0; k < WIN; ++k)
                    if (board[i + k][j] != sym)
                        break;
                if (k == WIN)
                    return true;
            }
            // diag down-right
            if (i + WIN <= ROWS && j + WIN <= COLS)
            {
                int k;
                for (k = 0; k < WIN; ++k)
                    if (board[i + k][j + k] != sym)
                        break;
                if (k == WIN)
                    return true;
            }
            // diag up-right
            if (i - WIN + 1 >= 0 && j + WIN <= COLS)
            {
                int k;
                for (k = 0; k < WIN; ++k)
                    if (board[i - k][j + k] != sym)
                        break;
                if (k == WIN)
                    return true;
            }
        }
    return false;
}

bool checkDraw(char board[ROWS][COLS])
{
    for (int j = 0; j < COLS; ++j)
        if (board[0][j] == '.')
            return false;
    return true;
}

void saveGame(char board[ROWS][COLS], int currentPlayer, const Player p[2])
{
    ofstream fout("savegame.bin", ios::binary);
    if (!fout)
    {
        printError("Greska pri cuvanju igre.");
        return;
    }
    fout.write((char *)board, sizeof(char) * ROWS * COLS);
    fout.write((char *)&currentPlayer, sizeof(int));
    int len1 = p[0].name.size(), len2 = p[1].name.size();
    fout.write((char *)&len1, sizeof(int));
    fout.write(p[0].name.c_str(), len1);
    fout.write((char *)&len2, sizeof(int));
    fout.write(p[1].name.c_str(), len2);
    fout.close();
    cout << "Igra sacuvana.\n";
}

bool loadGame(char board[ROWS][COLS], int &currentPlayer, Player p[2])
{
    ifstream fin("savegame.bin", ios::binary);
    if (!fin)
    {
        printError("Ne postoji sacuvana igra.");
        return false;
    }
    fin.read((char *)board, sizeof(char) * ROWS * COLS);
    fin.read((char *)&currentPlayer, sizeof(int));
    int len1, len2;
    fin.read((char *)&len1, sizeof(int));
    p[0].name.resize(len1);
    fin.read(&p[0].name[0], len1);
    fin.read((char *)&len2, sizeof(int));
    p[1].name.resize(len2);
    fin.read(&p[1].name[0], len2);
    p[0].symbol = 'X';
    p[1].symbol = 'O';
    fin.close();
    cout << "Igra ucitana.\n";
    return true;
}

void updateLeaderboard(const string &file, string winner)
{
    string names[100];
    int wins[100], n = 0;
    ifstream fin(file.c_str());
    while (fin >> names[n] >> wins[n])
        n++;
    fin.close();
    bool found = false;
    for (int i = 0; i < n; ++i)
    {
        if (names[i] == winner)
        {
            wins[i]++;
            found = true;
        }
    }
    if (!found)
    {
        names[n] = winner;
        wins[n] = 1;
        n++;
    }
    // sort by wins
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (wins[i] < wins[j])
            {
                swap(wins[i], wins[j]);
                swap(names[i], names[j]);
            }
    ofstream fout(file.c_str());
    for (int i = 0; i < n; ++i)
        fout << names[i] << " " << wins[i] << endl;
    fout.close();
}

void showLeaderboard(const string &file)
{
    ifstream fin(file.c_str());
    string n;
    int w, pos = 1;
    cout << "\n--- Leaderboard ---\n";
    cout << "Pozicija | Igrac           | Pobede\n";
    cout << "-------------------------------\n";
    while (fin >> n >> w)
    {
        cout << "   " << pos << "     | " << n;
        for (int i = 0; i < 15 - n.length(); ++i)
            cout << ' ';
        cout << "|   " << w << endl;
        pos++;
    }
    cout << "-------------------------------\n";
    fin.close();
}

int getIntInput(const string &poruka, int minval, int maxval)
{
    string unos;
    int x;
    while (true)
    {
        cout << poruka;
        cin >> unos;
        if (unos == "h" || unos == "help")
        {
            help();
            continue;
        }
        try
        {
            x = stoi(unos);
        }
        catch (...)
        {
            printError("Unos nije broj!");
            continue;
        }
        if (x < minval || x > maxval)
        {
            printError("Unos nije u dozvoljenom opsegu!");
            continue;
        }
        return x;
    }
}

void playGame(Player p[2])
{
    char board[ROWS][COLS];
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            board[i][j] = '.';
    int currentPlayer = 0;
    string unos;
    while (true)
    {
        printBoard(board);
        cout << "Na potezu: " << p[currentPlayer].name << " (" << p[currentPlayer].symbol << ")" << endl;
        cout << "Unesite kolonu (0-" << COLS - 1 << ") ili 'h' za pomoc: ";
        cin >> unos;
        if (unos == "h" || unos == "help")
        {
            help();
            continue;
        }
        int col;
        try
        {
            col = stoi(unos);
        }
        catch (...)
        {
            printError("Unos nije broj!");
            continue;
        }
        if (!makeMove(board, col, p[currentPlayer].symbol))
            continue;
        if (checkWin(board, p[currentPlayer].symbol))
        {
            printBoard(board);
            cout << "Pobednik je: " << p[currentPlayer].name << "!\n";
            updateLeaderboard("leaderboard.txt", p[currentPlayer].name);
            break;
        }
        if (checkDraw(board))
        {
            printBoard(board);
            cout << "Nereseno!\n";
            break;
        }
        currentPlayer = 1 - currentPlayer;
    }
    cout << "Zelite li sacuvati igru? (d/n): ";
    cin >> unos;
    if (unos == "d" || unos == "D")
        saveGame(board, currentPlayer, p);
}

int main()
{
    Player p[2];
    string unos;
    int izbor;
    while (true)
    {
        cout << "\n--- Connect 5 ---\n";
        cout << "1. Nova igra\n";
        cout << "2. Ucitaj igru\n";
        cout << "3. Prikazi leaderboard\n";
        cout << "4. Izlaz\n";
        cout << "Unesite izbor: ";
        cin >> unos;
        if (unos == "h" || unos == "help")
        {
            help();
            continue;
        }
        try
        {
            izbor = stoi(unos);
        }
        catch (...)
        {
            printError("Pogresan unos!");
            continue;
        }
        if (izbor == 1)
        {
            cout << "Ime prvog igraca: ";
            cin >> p[0].name;
            p[0].symbol = 'X';
            cout << "Ime drugog igraca: ";
            cin >> p[1].name;
            p[1].symbol = 'O';
            playGame(p);
        }
        else if (izbor == 2)
        {
            if (loadGame((char (*)[COLS])p, izbor, p))
                playGame(p);
        }
        else if (izbor == 3)
        {
            showLeaderboard("leaderboard.txt");
        }
        else if (izbor == 4)
        {
            cout << "Dovidjenja!\n";
            break;
        }
        else
        {
            printError("Pogresan izbor!");
        }
    }
    return 0;
}