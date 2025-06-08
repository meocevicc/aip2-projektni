#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

const int ROWS = 9;
const int COLS = 9;
const int TO_CONNECT = 5;
const char PLAYER1 = 'X';
const char PLAYER2 = 'O';
const int MAX_IGRACA = 100;

struct Rezultat
{
    string ime;
    int pobjede;
};

void naslov()
{
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                                                                                  ║\n";
    cout << "║   ██████╗ ██████╗ ███╗  ██╗███╗  ██╗███████╗███████╗████████╗  ╔██       ██╗     ║\n";
    cout << "║  ██╔════╝██╔═══██╗████╗ ██║████╗ ██║██╔════╝██╔════╝╚══██╔══╝  ╚╗██     ██╔╝     ║\n";
    cout << "║  ██║     ██║   ██║██╔██╗██║██╔██╗██║█████╗  ██║        ██║      ╚╗██   ██╔╝      ║\n";
    cout << "║  ██║     ██║   ██║██║╚████║██║╚████║██╔══╝  ██║        ██║       ╚╗██ ██╔╝       ║\n";
    cout << "║  ╚██████╗╚██████╔╝██║ ╚███║██║ ╚███║███████╗███████║   ██║        ╚╗███╔╝        ║\n";
    cout << "║   ╚═════╝ ╚═════╝ ╚═╝  ╚══╝╚═╝  ╚══╝╚══════╝╚══════╝   ╚═╝         ╚═══╝         ║\n";
    cout << "║                                                                                  ║\n";
    cout << "║                          Dobrodošli u Connect 5!                                 ║\n";
    cout << "║                       by Vukoja & Zebic                                          ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n";
}

void pravila()
{
    cout << "══════════════════════════════════════════════════════════════════════════════\n";
    cout << " PRAVILA IGRE CONNECT 5\n";
    cout << "──────────────────────────────────────────────────────────────────────────────\n";
    cout << " 1. Igra se na ploci 9x9.\n";
    cout << " 2. Igraci ubacuju X i O u stupce.\n";
    cout << " 3. Cilj: spojiti 5 svojih znakova zaredom\n";
    cout << "    (vodoravno, okomito ili dijagonalno).\n";
    cout << " 4. Igra prestaje kada netko spoji 5 ili je ploca puna.\n";
    cout << "══════════════════════════════════════════════════════════════════════════════\n\n";
}

void plocaIspis(char ploca[ROWS][COLS])
{
    cout << "\n     ";
    for (int c = 0; c < COLS; ++c)
    {
        cout << "\033[1;36m " << c + 1 << " \033[0m";
    }
    cout << endl;

    cout << "    ╔─";
    for (int c = 0; c < COLS - 1; ++c)
        cout << "───";
    cout << "──╗\n";

    for (int r = 0; r < ROWS; ++r)
    {
        cout << "    ║";
        for (int c = 0; c < COLS; ++c)
        {
            if (ploca[r][c] == PLAYER1)
                cout << " \033[1;31m" << ploca[r][c] << "\033[0m";
            else if (ploca[r][c] == PLAYER2)
                cout << " \033[1;34m" << ploca[r][c] << "\033[0m";
            else
                cout << " " << ploca[r][c];
            cout << " ";
        }
        cout << "║\n";
    }

    cout << "    ╚─";
    for (int c = 0; c < COLS - 1; ++c)
        cout << "───";
    cout << "──╝\n\n";
}

void plocaInit(char ploca[ROWS][COLS])
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            ploca[r][c] = '.';
}

bool ubaci(char ploca[ROWS][COLS], int col, char znak)
{
    for (int r = ROWS - 1; r >= 0; --r)
    {
        if (ploca[r][col] == '.')
        {
            ploca[r][col] = znak;
            return true;
        }
    }
    return false;
}

bool smjer(char ploca[ROWS][COLS], int r, int c, int dr, int dc, char znak)
{
    int broj = 0;
    for (int i = 0; i < TO_CONNECT; ++i)
    {
        int nr = r + i * dr;
        int nc = c + i * dc;
        if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
            return false;
        if (ploca[nr][nc] == znak)
            broj++;
        else
            break;
    }
    return broj == TO_CONNECT;
}

bool pobjeda(char ploca[ROWS][COLS], char znak)
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            if (
                smjer(ploca, r, c, 0, 1, znak) ||
                smjer(ploca, r, c, 1, 0, znak) ||
                smjer(ploca, r, c, 1, 1, znak) ||
                smjer(ploca, r, c, 1, -1, znak))
                return true;
        }
    }
    return false;
}

bool puna(char ploca[ROWS][COLS])
{
    for (int c = 0; c < COLS; ++c)
        if (ploca[0][c] == '.')
            return false;
    return true;
}

void spremi(char ploca[ROWS][COLS], char naPotezu)
{
    ofstream bin("spoji5_save.bin", ios::binary);
    if (!bin)
    {
        cout << "Greska prilikom spremanja igre!\n";
        return;
    }
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            bin.put(ploca[r][c]);
    bin.put(naPotezu);
    bin.close();
    cout << "Igra je spremljena!\n";
}

bool ucitaj(char ploca[ROWS][COLS], char &naPotezu)
{
    ifstream bin("spoji5_save.bin", ios::binary);
    if (!bin)
    {
        cout << "Nije pronadena spremljena igra!\n";
        return false;
    }
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            ploca[r][c] = bin.get();
    naPotezu = bin.get();
    bin.close();
    return true;
}

void upisiRez(string pobjednik)
{
    ofstream txt("spoji5_leaderboard.txt", ios::app);
    txt << pobjednik << endl;
    txt.close();
}

bool usporedi(const Rezultat &a, const Rezultat &b)
{
    if (a.pobjede != b.pobjede)
        return a.pobjede > b.pobjede;
    return a.ime < b.ime;
}

void leaderboard()
{
    string imena[MAX_IGRACA];
    int n = 0;
    ifstream txt("spoji5_leaderboard.txt");
    if (!txt)
    {
        cout << "Nema leaderboarda!\n";
        return;
    }
    string linija;
    while (getline(txt, linija))
    {
        if (linija.size() && n < MAX_IGRACA)
            imena[n++] = linija;
    }
    txt.close();

    Rezultat bodovi[MAX_IGRACA];
    int brIgraca = 0;
    for (int i = 0; i < n; ++i)
    {
        bool nadjen = false;
        for (int j = 0; j < brIgraca; ++j)
        {
            if (bodovi[j].ime == imena[i])
            {
                bodovi[j].pobjede++;
                nadjen = true;
                break;
            }
        }
        if (!nadjen && brIgraca < MAX_IGRACA)
        {
            bodovi[brIgraca].ime = imena[i];
            bodovi[brIgraca].pobjede = 1;
            brIgraca++;
        }
    }

    for (int i = 0; i < brIgraca - 1; ++i)
    {
        for (int j = i + 1; j < brIgraca; ++j)
        {
            if (!usporedi(bodovi[i], bodovi[j]))
            {
                Rezultat tmp = bodovi[i];
                bodovi[i] = bodovi[j];
                bodovi[j] = tmp;
            }
        }
    }

    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════╗\n";
    cout << "║                       🏆  LEADERBOARD  🏆                  ║\n";
    cout << "╠════════════════════════════════════════════════════════════╣\n";
    cout << "║  Pozicija    │   Igrač          │  Pobjede                 ║\n";
    cout << "╟──────────────┼──────────────────┼──────────────────────────╢\n";
    int prikazi = brIgraca;
    if (prikazi > 10)
        prikazi = 10;
    for (int i = 0; i < prikazi; ++i)
    {
        cout << "║     ";
        cout << setw(2) << i + 1 << "       │ ";
        cout << setw(17) << left << bodovi[i].ime.substr(0, 17) << "│ ";
        cout << setw(7) << right << bodovi[i].pobjede << "                  ║\n";
    }
    cout << "╚════════════════════════════════════════════════════════════╝\n\n";
}

void novaIgra()
{
    char ploca[ROWS][COLS];
    plocaInit(ploca);
    char naPotezu = PLAYER1;
    string p1, p2;
    cout << "Unesite ime igraca 1 (\033[1;31mX\033[0m): ";
    getline(cin, p1);
    cout << "Unesite ime igraca 2 (\033[1;34mO\033[0m): ";
    getline(cin, p2);
    while (true)
    {
        plocaIspis(ploca);
        cout << "Na potezu je " << ((naPotezu == PLAYER1) ? ("\033[1;31m" + p1 + "\033[0m") : ("\033[1;34m" + p2 + "\033[0m")) << " (" << naPotezu << ")" << endl;
        cout << "Unesite broj kolone (1-" << COLS << "), 0 za spremi, -1 za izlaz: ";
        int col;
        cin >> col;
        if (col == 0)
        {
            spremi(ploca, naPotezu);
            cout << "Igra spremljena, vracanje u menu...\n";
            cin.ignore();
            break;
        }
        if (col == -1)
        {
            cout << "Izlazak iz igre...\n";
            cin.ignore();
            break;
        }
        if (col < 1 || col > COLS)
        {
            cout << "Neispravan unos!\n";
            continue;
        }
        if (!ubaci(ploca, col - 1, naPotezu))
        {
            cout << "Kolona je puna!\n";
            continue;
        }
        if (pobjeda(ploca, naPotezu))
        {
            plocaIspis(ploca);
            string pobjednik = (naPotezu == PLAYER1) ? p1 : p2;
            cout << "🎉 Cestitamo! Pobjednik je \033[1;32m" << pobjednik << "\033[0m! 🎉\n";
            upisiRez(pobjednik);
            cout << "Pritisnite Enter za povratak u menu...";
            cin.ignore();
            cin.get();
            break;
        }
        if (puna(ploca))
        {
            plocaIspis(ploca);
            cout << "Nerijeseno! Nitko nije pobjedio.\n";
            cout << "Pritisnite Enter za povratak u menu...";
            cin.ignore();
            cin.get();
            break;
        }
        naPotezu = (naPotezu == PLAYER1) ? PLAYER2 : PLAYER1;
    }
}

void ucitajIgru()
{
    char ploca[ROWS][COLS];
    char naPotezu;
    if (!ucitaj(ploca, naPotezu))
    {
        cout << "Nema spremljene igre!\n";
        cout << "Pritisnite Enter za povratak u menu...";
        cin.ignore();
        return;
    }
    string p1, p2;
    cout << "Unesite ime igraca 1 (\033[1;31mX\033[0m): ";
    getline(cin, p1);
    cout << "Unesite ime igraca 2 (\033[1;34mO\033[0m): ";
    getline(cin, p2);
    while (true)
    {
        plocaIspis(ploca);
        cout << "Na potezu je " << ((naPotezu == PLAYER1) ? ("\033[1;31m" + p1 + "\033[0m") : ("\033[1;34m" + p2 + "\033[0m")) << " (" << naPotezu << ")" << endl;
        cout << "Unesite broj kolone (1-" << COLS << "), 0 za spremi, -1 za izlaz: ";
        int col;
        cin >> col;
        if (col == 0)
        {
            spremi(ploca, naPotezu);
            cout << "Igra spremljena, vracanje u menu...\n";
            cin.ignore();
            break;
        }
        if (col == -1)
        {
            cout << "Izlazak iz igre...\n";
            cin.ignore();
            break;
        }
        if (col < 1 || col > COLS)
        {
            cout << "Neispravan unos!\n";
            continue;
        }
        if (!ubaci(ploca, col - 1, naPotezu))
        {
            cout << "Kolona je puna!\n";
            continue;
        }
        if (pobjeda(ploca, naPotezu))
        {
            plocaIspis(ploca);
            string pobjednik = (naPotezu == PLAYER1) ? p1 : p2;
            cout << "🎉 Cestitamo! Pobjednik je \033[1;32m" << pobjednik << "\033[0m! 🎉\n";
            upisiRez(pobjednik);
            cout << "Pritisnite Enter za povratak u menu...";
            cin.ignore();
            cin.get();
            break;
        }
        if (puna(ploca))
        {
            plocaIspis(ploca);
            cout << "Nerijeseno! Nitko nije pobjedio.\n";
            cout << "Pritisnite Enter za povratak u menu...";
            cin.ignore();
            cin.get();
            break;
        }
        naPotezu = (naPotezu == PLAYER1) ? PLAYER2 : PLAYER1;
    }
}

int main()
{
    while (true)
    {
        naslov();
        cout << "╔════════════════════════════════════════════════════════════╗\n";
        cout << "║  1. Nova igra                                              ║\n";
        cout << "║  2. Ucitaj spremljenu igru                                 ║\n";
        cout << "║  3. Pravila                                                ║\n";
        cout << "║  4. Leaderboard                                            ║\n";
        cout << "║  5. Izlaz                                                  ║\n";
        cout << "╚════════════════════════════════════════════════════════════╝\n";
        cout << "Odaberite opciju: ";
        int izbor;
        cin >> izbor;
        cin.ignore();
        if (izbor == 1)
        {
            novaIgra();
        }
        else if (izbor == 2)
        {
            ucitajIgru();
        }
        else if (izbor == 3)
        {
            pravila();
            cout << "Pritisnite Enter za povratak u menu...";
            cin.get();
        }
        else if (izbor == 4)
        {
            leaderboard();
            cout << "Pritisnite Enter za povratak u menu...";
            cin.get();
        }
        else if (izbor == 5)
        {
            cout << "Hvala sto ste igrali Connect 5! 👋\n";
            break;
        }
        else
        {
            cout << "Neispravan izbor!\n";
        }
        cout << endl;
    }
    return 0;
}