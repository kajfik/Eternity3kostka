// EternityCpp.cpp : main project file.

#include "iostream"
#include "stdio.h"
#include "math.h"
#include <time.h>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <locale.h>
#include <random>
#include <vector>

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000.0)

using namespace std;

static int const szyrzkaPola = 16;
static int const wyszkaPola = 16;
static int const iloscKostek = szyrzkaPola * wyszkaPola;
//number of middle tiles
static int const nM = (szyrzkaPola - 2) * (wyszkaPola - 2);
//number of corners
static int const nC = 4;
//number of side tiles
static int const nS = iloscKostek - nM - nC;
//number of sides and corners
static int const nSpC = nS + nC;

__int64 iloscDanychKostek = 0;
int odKielaZapisowac = iloscKostek * (167 / 256.0);
int numerRzeszeni = 0;
int maxDoTeraz = -1;
int minDoTeraz = -1;
int xypole[szyrzkaPola][szyrzkaPola];//0->255 - number of tile on grid, iloscKostek - no tile on grid
bool polozono[iloscKostek + 1];
void dejKostke(int x, int y, int node);
int porzadi[iloscKostek][2];
static int wielkoscPorzadi = 1;
int wielkoscPorzadiM1 = -1;
bool zapisuj;
time_t start;
int lastTime = 0;
string txt = "";

/*
I get empty tile with colors a north, b east, c south, d west
returnTile[a][b][c][d][0] returns numbers of tiles that can go there with ...[1] orientation
0 is no color there, so empty space there
*/
//[a][b][c][d][0] - numer, ...[1] - obrocyni
vector<int> *returnTile;

int timer = time(NULL);
mt19937 rng(timer);
uniform_int_distribution<int> rnd0_3(0, 3);
uniform_int_distribution<int> rnd4_numOfSidesPlusCornersMinus1(4, nC + nS - 1);
uniform_int_distribution<int> rndNumOfSidesPlusCorners_numOfTilesMinus1(nC + nS, iloscKostek - 1);

int xypoleSmery[szyrzkaPola][szyrzkaPola][4];

//Brendan's 10x10 set2
int kostki3[100][7] = {
	{ 1, 1, 2, 3 }, { 1, 1, 3, 4 }, { 1, 1, 3, 5 }, { 1, 1, 5, 3 }, { 1, 2, 6, 3 }, { 1, 2, 8, 2 }, { 1, 2, 8, 5 }, { 1, 2, 9, 2 }, { 1, 2, 10, 2 }, { 1, 2, 10, 5 },
	{ 1, 2, 11, 4 }, { 1, 2, 15, 2 }, { 1, 3, 7, 4 }, { 1, 3, 7, 5 }, { 1, 3, 8, 5 }, { 1, 3, 10, 5 }, { 1, 3, 11, 4 }, { 1, 3, 14, 3 }, { 1, 3, 15, 4 }, { 1, 4, 7, 2 },
	{ 1, 4, 7, 3 }, { 1, 4, 10, 3 }, { 1, 4, 11, 4 }, { 1, 4, 12, 5 }, { 1, 4, 13, 2 }, { 1, 4, 14, 2 }, { 1, 4, 15, 2 }, { 1, 4, 15, 4 }, { 1, 5, 8, 3 }, { 1, 5, 8, 4 },
	{ 1, 5, 11, 5 }, { 1, 5, 12, 2 }, { 1, 5, 12, 3 }, { 1, 5, 12, 5 }, { 1, 5, 13, 4 }, { 1, 5, 15, 3 }, { 6, 6, 11, 12 }, { 6, 7, 6, 15 }, { 6, 7, 8, 11 }, { 6, 7, 11, 11 },
	{ 6, 7, 13, 13 }, { 6, 7, 13, 14 }, { 6, 8, 6, 11 }, { 6, 9, 7, 10 }, { 6, 9, 8, 7 }, { 6, 9, 9, 13 }, { 6, 9, 10, 8 }, { 6, 9, 12, 15 }, { 6, 9, 15, 7 }, { 6, 10, 10, 9 },
	{ 6, 10, 12, 15 }, { 6, 11, 13, 13 }, { 6, 11, 14, 9 }, { 6, 11, 14, 12 }, { 6, 12, 6, 13 }, { 6, 12, 7, 7 }, { 6, 12, 8, 12 }, { 6, 15, 7, 8 }, { 6, 15, 7, 14 }, { 6, 15, 10, 13 },
	{ 6, 15, 11, 11 }, { 7, 7, 15, 11 }, { 7, 8, 7, 11 }, { 7, 8, 7, 13 }, { 7, 8, 12, 9 }, { 7, 9, 12, 13 }, { 7, 10, 10, 9 }, { 7, 11, 13, 12 }, { 7, 13, 15, 8 }, { 7, 14, 11, 12 },
	{ 7, 14, 12, 11 }, { 7, 14, 13, 8 }, { 8, 8, 8, 15 }, { 8, 8, 9, 9 }, { 8, 9, 8, 14 }, { 8, 9, 8, 15 }, { 8, 9, 10, 9 }, { 8, 10, 11, 13 }, { 8, 10, 12, 14 }, { 8, 11, 12, 14 },
	{ 8, 13, 13, 10 }, { 9, 9, 13, 13 }, { 9, 11, 12, 14 }, { 9, 12, 11, 10 }, { 9, 12, 11, 15 }, { 9, 12, 14, 12 }, { 9, 13, 14, 10 }, { 9, 13, 14, 15 }, { 9, 13, 15, 11 }, { 9, 15, 10, 10 },
	{ 9, 15, 10, 14 }, { 10, 10, 13, 14 }, { 10, 12, 11, 15 }, { 10, 14, 12, 14 }, { 10, 14, 13, 15 }, { 10, 14, 14, 15 }, { 10, 15, 12, 14 }, { 11, 12, 14, 15 }, { 11, 13, 13, 13 }, { 14, 14, 14, 15 }
};

//kolory 19, 20, 21, 22, 23 som mjyndzy krajami, 1 - kraj, 2 az 18 som w srodku
//gora, prawo, dol, lewo
int kostkiPrawePlus1[256][7] = {
	/* 1 - 16 */{ 19, 23, 1, 1 }, { 19, 20, 1, 1 }, { 21, 23, 1, 1 }, { 23, 21, 1, 1 }, { 2, 19, 1, 19 }, { 10, 21, 1, 19 }, { 6, 19, 1, 19 }, { 6, 22, 1, 19 }, { 11, 23, 1, 19 }, { 7, 20, 1, 19 }, { 15, 21, 1, 19 }, { 8, 20, 1, 19 }, { 8, 22, 1, 19 }, { 13, 20, 1, 19 }, { 10, 19, 1, 21 }, { 18, 23, 1, 21 },
	/* 17 - 32 */{ 14, 22, 1, 21 }, { 5, 22, 1, 21 }, { 7, 21, 1, 21 }, { 15, 21, 1, 21 }, { 4, 20, 1, 21 }, { 12, 19, 1, 21 }, { 12, 22, 1, 21 }, { 9, 19, 1, 21 }, { 13, 19, 1, 21 }, { 2, 21, 1, 23 }, { 2, 23, 1, 23 }, { 10, 23, 1, 23 }, { 18, 23, 1, 23 }, { 7, 22, 1, 23 }, { 15, 21, 1, 23 }, { 9, 23, 1, 23 },
	/* 33 - 48 */{ 8, 21, 1, 23 }, { 8, 20, 1, 23 }, { 16, 22, 1, 23 }, { 17, 20, 1, 23 }, { 18, 19, 1, 20 }, { 3, 22, 1, 20 }, { 11, 22, 1, 20 }, { 5, 21, 1, 20 }, { 5, 23, 1, 20 }, { 15, 19, 1, 20 }, { 15, 21, 1, 20 }, { 15, 23, 1, 20 }, { 4, 19, 1, 20 }, { 9, 20, 1, 20 }, { 8, 20, 1, 20 }, { 16, 20, 1, 20 },
	/* 49 - 64 */{ 2, 22, 1, 22 }, { 10, 19, 1, 22 }, { 10, 21, 1, 22 }, { 6, 19, 1, 22 }, { 7, 20, 1, 22 }, { 4, 20, 1, 22 }, { 4, 22, 1, 22 }, { 8, 23, 1, 22 }, { 16, 19, 1, 22 }, { 16, 22, 1, 22 }, { 13, 21, 1, 22 }, { 17, 23, 1, 22 }, { 6, 18, 2, 2 }, { 14, 7, 2, 2 }, { 10, 3, 2, 10 }, { 2, 8, 2, 18 },
	/* 65 - 80 */{ 18, 17, 2, 18 }, { 14, 14, 2, 18 }, { 11, 10, 2, 18 }, { 9, 6, 2, 18 }, { 17, 8, 2, 18 }, { 3, 7, 2, 3 }, { 7, 12, 2, 3 }, { 14, 18, 2, 11 }, { 15, 4, 2, 11 }, { 9, 15, 2, 11 }, { 8, 3, 2, 11 }, { 14, 15, 2, 5 }, { 5, 15, 2, 5 }, { 3, 16, 2, 7 }, { 9, 3, 2, 7 }, { 16, 13, 2, 7 },
	/* 81 - 96 */{ 5, 18, 2, 15 }, { 18, 18, 2, 4 }, { 11, 4, 2, 4 }, { 18, 5, 2, 12 }, { 6, 14, 2, 12 }, { 8, 12, 2, 12 }, { 16, 9, 2, 12 }, { 2, 13, 2, 9 }, { 6, 17, 2, 9 }, { 4, 16, 2, 9 }, { 11, 12, 2, 8 }, { 5, 15, 2, 8 }, { 5, 4, 2, 8 }, { 4, 13, 2, 8 }, { 12, 14, 2, 8 }, { 13, 3, 2, 13 },
	/* 97 - 112 */{ 4, 5, 2, 17 }, { 9, 8, 2, 17 }, { 13, 6, 2, 17 }, { 17, 13, 2, 17 }, { 12, 15, 10, 10 }, { 12, 16, 10, 10 }, { 16, 5, 10, 10 }, { 17, 6, 10, 10 }, { 4, 15, 10, 18 }, { 3, 8, 10, 6 }, { 5, 8, 10, 6 }, { 4, 15, 10, 6 }, { 16, 11, 10, 6 }, { 15, 12, 10, 14 }, { 12, 15, 10, 14 }, { 9, 5, 10, 3 },
	/* 113 - 128 */{ 9, 16, 10, 3 }, { 14, 4, 10, 11 }, { 7, 12, 10, 11 }, { 12, 11, 10, 11 }, { 17, 16, 10, 11 }, { 3, 13, 10, 5 }, { 16, 12, 10, 7 }, { 8, 17, 10, 15 }, { 14, 17, 10, 4 }, { 6, 16, 10, 9 }, { 14, 5, 10, 9 }, { 9, 15, 10, 9 }, { 12, 17, 10, 8 }, { 13, 15, 10, 8 }, { 14, 6, 10, 16 }, { 5, 13, 10, 16 },
	/* 129 - 144 */{ 4, 3, 10, 16 }, { 9, 8, 10, 16 }, { 6, 9, 10, 13 }, { 12, 14, 10, 13 }, { 14, 16, 10, 17 }, { 11, 4, 10, 17 }, { 4, 3, 10, 17 }, { 16, 9, 10, 17 }, { 9, 7, 18, 18 }, { 6, 3, 18, 6 }, { 6, 11, 18, 6 }, { 6, 12, 18, 6 }, { 5, 13, 18, 6 }, { 15, 6, 18, 6 }, { 16, 12, 18, 6 }, { 13, 13, 18, 6 },
	/* 145 - 160 */{ 3, 4, 18, 14 }, { 18, 12, 18, 3 }, { 18, 17, 18, 3 }, { 3, 14, 18, 3 }, { 15, 12, 18, 3 }, { 6, 11, 18, 5 }, { 4, 17, 18, 5 }, { 11, 11, 18, 7 }, { 11, 5, 18, 7 }, { 17, 16, 18, 7 }, { 7, 7, 18, 4 }, { 7, 12, 18, 4 }, { 17, 7, 18, 4 }, { 7, 16, 18, 9 }, { 8, 6, 18, 9 }, { 13, 13, 18, 8 },
	/* 161 - 176 */{ 6, 9, 18, 16 }, { 14, 9, 18, 16 }, { 15, 11, 18, 17 }, { 4, 16, 18, 17 }, { 3, 4, 6, 14 }, { 4, 8, 6, 14 }, { 3, 3, 6, 11 }, { 11, 15, 6, 5 }, { 5, 13, 6, 5 }, { 4, 8, 6, 7 }, { 9, 16, 6, 7 }, { 13, 11, 6, 7 }, { 15, 15, 6, 15 }, { 12, 9, 6, 15 }, { 7, 13, 6, 4 }, { 7, 5, 6, 12 },
	/* 177 - 192 */{ 14, 4, 6, 9 }, { 12, 16, 6, 8 }, { 8, 15, 6, 8 }, { 7, 16, 6, 16 }, { 11, 16, 6, 13 }, { 7, 11, 6, 13 }, { 5, 8, 14, 14 }, { 17, 7, 14, 3 }, { 5, 12, 14, 11 }, { 8, 8, 14, 11 }, { 13, 7, 14, 5 }, { 14, 13, 14, 7 }, { 3, 5, 14, 7 }, { 16, 5, 14, 7 }, { 3, 3, 14, 15 }, { 15, 9, 14, 15 },
	/* 193 - 208 */{ 11, 7, 14, 4 }, { 13, 11, 14, 12 }, { 13, 17, 14, 12 }, { 17, 15, 14, 12 }, { 11, 17, 14, 9 }, { 5, 8, 14, 9 }, { 9, 9, 14, 9 }, { 5, 3, 14, 8 }, { 13, 8, 14, 16 }, { 17, 7, 14, 16 }, { 12, 5, 14, 13 }, { 12, 8, 14, 13 }, { 16, 3, 14, 13 }, { 17, 13, 14, 13 }, { 17, 7, 3, 3 }, { 5, 17, 3, 11 },
	/* 209 - 224 */{ 8, 15, 3, 11 }, { 11, 5, 3, 7 }, { 16, 15, 3, 7 }, { 3, 16, 3, 15 }, { 8, 8, 3, 4 }, { 3, 9, 3, 12 }, { 4, 17, 3, 12 }, { 17, 13, 3, 12 }, { 5, 15, 3, 9 }, { 4, 12, 3, 16 }, { 11, 4, 3, 13 }, { 11, 16, 3, 17 }, { 13, 13, 3, 17 }, { 13, 17, 3, 17 }, { 12, 17, 11, 11 }, { 9, 7, 11, 11 },
	/* 225 - 240 */{ 16, 15, 11, 11 }, { 5, 15, 11, 7 }, { 12, 12, 11, 7 }, { 5, 8, 11, 4 }, { 7, 17, 11, 9 }, { 16, 8, 11, 9 }, { 12, 9, 11, 8 }, { 12, 13, 11, 8 }, { 5, 9, 5, 5 }, { 16, 4, 5, 7 }, { 7, 4, 5, 4 }, { 7, 9, 5, 4 }, { 12, 15, 5, 4 }, { 4, 16, 5, 12 }, { 15, 17, 5, 9 }, { 13, 15, 5, 9 },
	/* 241 - 256 */{ 7, 13, 5, 8 }, { 4, 13, 5, 8 }, { 15, 12, 7, 15 }, { 9, 8, 7, 15 }, { 17, 9, 7, 4 }, { 16, 17, 7, 13 }, { 13, 17, 15, 15 }, { 12, 4, 15, 4 }, { 4, 13, 15, 12 }, { 16, 13, 15, 9 }, { 17, 8, 4, 4 }, { 8, 12, 4, 12 }, { 16, 9, 12, 8 }, { 13, 16, 9, 16 }, { 16, 17, 9, 17 }, { 13, 17, 8, 17 } };

int kostki[iloscKostek][7]; // 7 -> no need to do kostki[a][(b + 3) % 4]
int kostkiNowe[iloscKostek]; //used to remember how are kostki changed 


void umiejscKostke(int x, int y, int czisloKostki, int kieroStronaJeNaGorze)
{
	if (czisloKostki >= 1 && polozono[czisloKostki - 1] == false)
	{
		xypole[x][y] = czisloKostki - 1;
		xypoleSmery[x][y][0] = kostki[czisloKostki - 1][kieroStronaJeNaGorze];
		xypoleSmery[x][y][1] = kostki[czisloKostki - 1][(kieroStronaJeNaGorze + 1) % 4];
		xypoleSmery[x][y][2] = kostki[czisloKostki - 1][(kieroStronaJeNaGorze + 2) % 4];
		xypoleSmery[x][y][3] = kostki[czisloKostki - 1][(kieroStronaJeNaGorze + 3) % 4];
		polozono[czisloKostki - 1] = true;
	}
}

void wypiszInformacje()
{
	cout << "\r";
	printf("Mx: %d Mn: %d Rz: %d Sp: %d | %d | ", maxDoTeraz + 1, minDoTeraz + 1, numerRzeszeni, (int)(iloscDanychKostek / (lastTime + 0.01)), (int)(numerRzeszeni / (lastTime + 0.01)));
	cout << iloscDanychKostek / 1000000000.0 << "        ";
}

void zapiszZabraneKostkiZtxt()
{
	ifstream is("C:\\Eternity\\zabraneKostki" + txt + ".txt");

	char c;
	for (int a = 0; a < wyszkaPola; a++)
	{
		for (int b = 0; b < szyrzkaPola; b++)
		{
			int numerKostki = 0;
			int otoczyni = 0;
			for (int d = 0; d < 7; d++)
			{
				is.get(c);
				if (d == 0)
				{
					numerKostki += (c - 48) * 100;
				}
				else if (d == 1)
				{
					numerKostki += (c - 48) * 10;
				}
				else if (d == 2)
				{
					numerKostki += (c - 48);
				}
				else if (d == 4)
				{
					otoczyni = c - 48;
				}
			}
			umiejscKostke(b, a, numerKostki, otoczyni);
		}
		is.get(c);
	}
	is.close();
}

void ZapiszTahDoEternitytxt()
{
	ofstream file2;
	file2.open("C:\\Eternity\\EternityCpp.txt", std::ios::app);
	{
		file2 << "x, " << ("%d", numerRzeszeni) << ", " << ("%d", maxDoTeraz + 1) << "\n";
		for (int m = 1; m <= wyszkaPola; m++)
		{
			for (int n = 1; n <= szyrzkaPola; n++)
			{
				file2 << ("%d", xypole[n - 1][m - 1] == iloscKostek ? 0 : kostkiNowe[xypole[n - 1][m - 1]] + 1) << "(" << ("%d", xypoleSmery[n - 1][m - 1][0]) << ", " << ("%d", xypoleSmery[n - 1][m - 1][1]) << ", " << ("%d", xypoleSmery[n - 1][m - 1][2]) << ", " << ("%d", xypoleSmery[n - 1][m - 1][3]) << ")" << ", ";
			}
			file2 << "\n\n";
		}
		file2.close();
	};
	ofstream file3;
	file3.open("C:\\Eternity\\EternityCpp2.txt", std::ios::app);
	{
		for (int m = 1; m <= wyszkaPola; m++)
		{
			for (int n = 1; n <= szyrzkaPola; n++)
			{
				int smer = 0;
				for (int a = 0; a < 4; a++)
				{
					if (
						kostki[xypole[n - 1][m - 1]][a] == xypoleSmery[n - 1][m - 1][0] &&
						kostki[xypole[n - 1][m - 1]][(a + 1) % 4] == xypoleSmery[n - 1][m - 1][1] &&
						kostki[xypole[n - 1][m - 1]][(a + 2) % 4] == xypoleSmery[n - 1][m - 1][2] &&
						kostki[xypole[n - 1][m - 1]][(a + 3) % 4] == xypoleSmery[n - 1][m - 1][3]
						)
					{
						smer = a;
						break;
					}
				}
				file3 << (xypole[n - 1][m - 1] == iloscKostek ? "00" : (kostkiNowe[xypole[n - 1][m - 1]] >= 100 - 1 ? "" : (kostkiNowe[xypole[n - 1][m - 1]] >= 10 - 1 ? "0" : "00"))) << ("%d", xypole[n - 1][m - 1] == iloscKostek ? 000 : kostkiNowe[xypole[n - 1][m - 1]] + 1) << " " << ("%d", smer) << ", ";
			}
			file3 << "\n";
		}
		file3 << "\n\n";
		file3.close();
	};
}

void zapiszDoPorzadi()
{
	ofstream file2;
	file2.open("C:\\Eternity\\porzadi0test.txt", std::ofstream::out | std::ofstream::trunc);
	file2.close();
	ofstream file;
	file.open("C:\\Eternity\\porzadi0test.txt", std::ios::app);
	{
		for (int m = 1; m <= wyszkaPola; m++)
		{
			for (int n = 1; n <= szyrzkaPola; n++)
			{
				file << " " << (xypole[n - 1][m - 1] >= 100 ? "" : (xypole[n - 1][m - 1] >= 10 ? " " : "  ")) << xypole[n - 1][m - 1];
			}
			file << "\n";
		}
		file << "\n\n";
		file.close();
	};
}

//writes data from porzadi.txt to array porzadi[][]
void zapiszPorzadi()
{
	fstream infile("C:\\Eternity\\porzadi" + txt + ".txt");
	int a[szyrzkaPola];
	int rzondek = 0;
	while (rzondek <= wyszkaPola - 1)
	{
		for (int c = 0; c < szyrzkaPola; c++)
		{
			infile >> a[c];
		}
		for (int b = 0; b < szyrzkaPola; b++)
		{
			if (a[b] != 0)
			{
				porzadi[a[b] - 1][0] = b;
				porzadi[a[b] - 1][1] = rzondek;
				if (a[b] > wielkoscPorzadi)
				{
					wielkoscPorzadi = a[b];
				}
			}
		}
		rzondek++;
	}
	wielkoscPorzadiM1 = wielkoscPorzadi - 1;
}

void reset()
{
	numerRzeszeni = 0;
	for (int i = 0; i < iloscKostek; i++)
	{
		kostkiNowe[i] = i;
	}
	for (int a = 0; a < szyrzkaPola; a++)
	{
		for (int b = 0; b < wyszkaPola; b++)
		{
			xypole[a][b] = iloscKostek;
		}
	}
	for (int a = 0; a < szyrzkaPola; a++)
	{
		for (int b = 0; b < wyszkaPola; b++)
		{
			for (int c = 0; c < 4; c++)
			{
				xypoleSmery[a][b][c] = 0;
			}
		}
	}
	for (int i = 0; i < iloscKostek + 1; i++)
	{
		polozono[i] = false;
	}
}

void dejKostke(int x, int y, int node)
{
	if (x == 0)
	{
		if (y == 0)
		{
			int tile = 27648 + 1152 * xypoleSmery[x + 1][y][3] + 48 * xypoleSmery[x][y + 1][0] + 2;
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a < nC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
		else if (y >= 1 && y <= wyszkaPola - 2)
		{
			int tile = 27648 * xypoleSmery[x][y - 1][2] + 1152 * xypoleSmery[x + 1][y][3] + 48 * xypoleSmery[x][y + 1][0] + 2;
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a >= nC && a < nSpC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
		else if (y == wyszkaPola - 1)
		{
			int tile = 27648 * xypoleSmery[x][y - 1][2] + 1152 * xypoleSmery[x + 1][y][3] + 48 + 2;
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a < nC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
	}
	else if (x <= szyrzkaPola - 2 && x >= 1)
	{
		if (y == 0)
		{
			int tile = 27648 + 1152 * xypoleSmery[x + 1][y][3] + 48 * xypoleSmery[x][y + 1][0] + 2 * xypoleSmery[x - 1][y][1];
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a >= nC && a < nSpC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
		else if (y >= 1 && y <= wyszkaPola - 2)
		{
			int tile = 27648 * xypoleSmery[x][y - 1][2] + 1152 * xypoleSmery[x + 1][y][3] + 48 * xypoleSmery[x][y + 1][0] + 2 * xypoleSmery[x - 1][y][1];
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a >= nSpC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
		else if (y == wyszkaPola - 1)
		{
			int tile = 27648 * xypoleSmery[x][y - 1][2] + 1152 * xypoleSmery[x + 1][y][3] + 48 + 2 * xypoleSmery[x - 1][y][1];
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a >= nC && a < nSpC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
	}
	else if (x == szyrzkaPola - 1)
	{
		if (y == 0)
		{
			int tile = 27648 + 1152 + 48 * xypoleSmery[x][y + 1][0] + 2 * xypoleSmery[x - 1][y][1];
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a < nC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
		else if (y >= 1 && y <= wyszkaPola - 2)
		{
			int tile = 27648 * xypoleSmery[x][y - 1][2] + 1152 + 48 * xypoleSmery[x][y + 1][0] + 2 * xypoleSmery[x - 1][y][1];
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a >= nC && a < nSpC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
		else if (y == wyszkaPola - 1)
		{
			int tile = 27648 * xypoleSmery[x][y - 1][2] + 1152 + 48 + 2 * xypoleSmery[x - 1][y][1];
			int size = returnTile[tile].size();
			for (int i = 0; i < size; i++)
			{
				int a = returnTile[tile][i];
				if (!polozono[a] && a < nC)
				{
					int b = returnTile[tile + 1][i];
					//if (!(rzeszic3x3 && node >= odKielaZkuszac3x3 && !idzieDac3x3(x, y, a, b, node)))
					{
						xypole[x][y] = a;
						xypoleSmery[x][y][0] = kostki[a][b];
						xypoleSmery[x][y][1] = kostki[a][b + 1];
						xypoleSmery[x][y][2] = kostki[a][b + 2];
						xypoleSmery[x][y][3] = kostki[a][b + 3];
						polozono[a] = true;
						iloscDanychKostek++;
						if (iloscDanychKostek % 5000000 == 0)
						{
							lastTime = difftime(time(0), start);

							wypiszInformacje();
						}
						if (node > maxDoTeraz)
						{
							minDoTeraz = node;
							maxDoTeraz = node;
							if (maxDoTeraz >= odKielaZapisowac)
							{
								ZapiszTahDoEternitytxt();
							}
							if (node == wielkoscPorzadiM1)
							{
								numerRzeszeni++;
								maxDoTeraz = 0;
							}
						}
						if (node < wielkoscPorzadiM1)
						{
							if (node < minDoTeraz)
							{
								minDoTeraz = node;
							}
							dejKostke(porzadi[node + 1][0], porzadi[node + 1][1], node + 1);
						}
						xypole[x][y] = iloscKostek;
						xypoleSmery[x][y][0] = 0;
						xypoleSmery[x][y][1] = 0;
						xypoleSmery[x][y][2] = 0;
						xypoleSmery[x][y][3] = 0;
						polozono[a] = false;
					}
				}
			}
		}
	}
}

void changeKostki()
{
	int zmiana1, zmiana2, pomoc;
	for (int i = 0; i < 100; i++)
	{
		zmiana1 = rnd0_3(rng);
		zmiana2 = rnd0_3(rng);
		if (!polozono[zmiana1] && !polozono[zmiana2])
		{
			pomoc = kostkiNowe[zmiana1];
			kostkiNowe[zmiana1] = kostkiNowe[zmiana2];
			kostkiNowe[zmiana2] = pomoc;
			for (int j = 0; j < 4; j++)
			{
				pomoc = kostki[zmiana1][j];
				kostki[zmiana1][j] = kostki[zmiana2][j];
				kostki[zmiana2][j] = pomoc;
			}
		}
	}
	for (int i = 0; i < 10000; i++)
	{
		zmiana1 = rnd4_numOfSidesPlusCornersMinus1(rng);
		zmiana2 = rnd4_numOfSidesPlusCornersMinus1(rng);
		if (!polozono[zmiana1] && !polozono[zmiana2])
		{
			pomoc = kostkiNowe[zmiana1];
			kostkiNowe[zmiana1] = kostkiNowe[zmiana2];
			kostkiNowe[zmiana2] = pomoc;
			for (int j = 0; j < 4; j++)
			{
				pomoc = kostki[zmiana1][j];
				kostki[zmiana1][j] = kostki[zmiana2][j];
				kostki[zmiana2][j] = pomoc;
			}
		}
	}
	for (int i = 0; i < 10000; i++)
	{
		zmiana1 = rndNumOfSidesPlusCorners_numOfTilesMinus1(rng);
		zmiana2 = rndNumOfSidesPlusCorners_numOfTilesMinus1(rng);
		if (!polozono[zmiana1] && !polozono[zmiana2])
		{
			pomoc = kostkiNowe[zmiana1];
			kostkiNowe[zmiana1] = kostkiNowe[zmiana2];
			kostkiNowe[zmiana2] = pomoc;
			for (int j = 0; j < 4; j++)
			{
				pomoc = kostki[zmiana1][j];
				kostki[zmiana1][j] = kostki[zmiana2][j];
				kostki[zmiana2][j] = pomoc;
			}
		}
	}
}

int main()
{
	clock_t startD;
	double sElapsed;

	cout << "Rzeszyni " << szyrzkaPola << "x" << wyszkaPola << endl;
	if (szyrzkaPola == 10 && wyszkaPola == 10)
	{
		txt = "10x10";
		for (int i = 0; i < iloscKostek; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				kostki[i][j] = kostki3[i][j];
			}
		}
	}
	else if (szyrzkaPola == 16 && wyszkaPola == 16)
	{
		txt = "";
		for (int i = 0; i < iloscKostek; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				kostki[i][j] = kostkiPrawePlus1[i][j];
			}
		}
	}
	bool rzeszicPorzadi;
	cout << "Defaultni nastawjyni: nie rzesz 3x3, nie wymazuj EternityCpp(2).txt\n";
	cout << "Defaultni nastawjyni? (y/n): ";
	char c;
	cin >> c;
	bool def = true;
	if (c == 'n')
	{
		def = false;
	}
	if (!def)
	{
		cout << "Wymazac EternityCpp(2).txt? (y/n): ";
		cin >> c;
		if (c == 'y' || c == 'z')
		{
			ofstream file2;
			file2.open("C:\\Eternity\\EternityCpp.txt", std::ofstream::out | std::ofstream::trunc);
			file2.close();
			ofstream file3;
			file2.open("C:\\Eternity\\EternityCpp2.txt", std::ofstream::out | std::ofstream::trunc);
			file3.close();
		}
	}
	reset();
	zapiszPorzadi();
	zapiszZabraneKostkiZtxt();
	changeKostki();
	for (int i = 0; i < iloscKostek; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			kostki[i][j + 4] = kostki[i][j];
		}
	}
	cout << "Initializing array of tiles... ";
	returnTile = new vector<int>[24 * 24 * 24 * 24 * 2];
	for (int i = 0; i < iloscKostek; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int s1 = kostki[i][j];
			int s2 = kostki[i][j + 1];
			int s3 = kostki[i][j + 2];
			int s4 = kostki[i][j + 3];
			//s1,0,0,0
			returnTile[s1 * 27648 + 0].push_back(i);
			returnTile[s1 * 27648 + 1].push_back(j);
			//0,s2,0,0
			returnTile[s2 * 1152 + 0].push_back(i);
			returnTile[s2 * 1152 + 1].push_back(j);
			//0,0,s3,0
			returnTile[s3 * 48 + 0].push_back(i);
			returnTile[s3 * 48 + 1].push_back(j);
			//0,0,0,s4
			returnTile[s4 * 2 + 0].push_back(i);
			returnTile[s4 * 2 + 1].push_back(j);
			//s1,s2,0,0
			returnTile[s1 * 27648 + s2 * 1152 + 0].push_back(i);
			returnTile[s1 * 27648 + s2 * 1152 + 1].push_back(j);
			//s1,0,s3,0
			returnTile[s1 * 27648 + s3 * 48 + 0].push_back(i);
			returnTile[s1 * 27648 + s3 * 48 + 1].push_back(j);
			//s1,0,0,s4
			returnTile[s1 * 27648 + s4 * 2 + 0].push_back(i);
			returnTile[s1 * 27648 + s4 * 2 + 1].push_back(j);
			//0,s2,s3,0
			returnTile[s2 * 1152 + s3 * 48 + 0].push_back(i);
			returnTile[s2 * 1152 + s3 * 48 + 1].push_back(j);
			//0,s2,0,s4
			returnTile[s2 * 1152 + s4 * 2 + 0].push_back(i);
			returnTile[s2 * 1152 + s4 * 2 + 1].push_back(j);
			//0,0,s3,s4
			returnTile[s3 * 48 + s4 * 2 + 0].push_back(i);
			returnTile[s3 * 48 + s4 * 2 + 1].push_back(j);
			//s1,s2,s3,0
			returnTile[s1 * 27648 + s2 * 1152 + s3 * 48 + 0].push_back(i);
			returnTile[s1 * 27648 + s2 * 1152 + s3 * 48 + 1].push_back(j);
			//s1,s2,0,s4
			returnTile[s1 * 27648 + s2 * 1152 + s4 * 2 + 0].push_back(i);
			returnTile[s1 * 27648 + s2 * 1152 + s4 * 2 + 1].push_back(j);
			//s1,0,s3,s4
			returnTile[s1 * 27648 + s3 * 48 + s4 * 2 + 0].push_back(i);
			returnTile[s1 * 27648 + s3 * 48 + s4 * 2 + 1].push_back(j);
			//0,s2,s3,s4
			returnTile[s2 * 1152 + s3 * 48 + s4 * 2 + 0].push_back(i);
			returnTile[s2 * 1152 + s3 * 48 + s4 * 2 + 1].push_back(j);
			//s1,s2,s3,s4
			returnTile[s1 * 27648 + s2 * 1152 + s3 * 48 + s4 * 2 + 0].push_back(i);
			returnTile[s1 * 27648 + s2 * 1152 + s3 * 48 + s4 * 2 + 1].push_back(j);
			//0,0,0,0
			returnTile[0].push_back(i);
			returnTile[1].push_back(j);
		}
	}
	cout << "Done." << endl;
	cout << "Sp - Prumer kostki na sekunde | rzeszeni na sekunde | Ilosc miliard danych kostek\n";
	zapisuj = true;
	start = time(0);
	startD = clock();
	dejKostke(porzadi[0][0], porzadi[0][1], 0);
	sElapsed = ((clock() - startD) / (double)CLOCKS_PER_MS) / 1000.0;
	cout << endl << endl << "Ilosc polozonych kostek: " << iloscDanychKostek << endl;
	cout << "Ilosc kostek na sekunde: " << (long long int)(iloscDanychKostek / sElapsed) << endl;
	cout << "Ilosc rzeszeni: " << numerRzeszeni << endl;
	cout << "Czas: " << sElapsed << " sekund" << endl;


	cout << endl;
	system("pause");
	return 0;
}
