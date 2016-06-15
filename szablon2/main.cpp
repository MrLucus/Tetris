#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<windows.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#include<conio.h>

using namespace std;
extern "C" {
	//#ifdef BIT64
	//#include"./sdl64/include/SDL.h"
	//#include"./sdl64/include/SDL_main.h"
	//#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
	//#endif
}

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	880
#define NAPISY_X 0.5
#define NAPISY_Y 0.3
#define SZYBKOSC 0.9 //szybkoœæ standardowa
#define SZYBKIE_OPADANIE 0.6 //wspó³czynnik prêdkoœci szybkiego opadania
#define CZERWONY 0
#define SZARY 1
#define MORSKI 2
#define ZOLTY 3
#define FIOLETOWY 4
#define NIEBIESKI 5
#define ZIELONY 6
#define BRAK 9
#define CZERWONYA 10
#define SZARYA 11
#define MORSKIA 12
#define ZOLTYA 13
#define FIOLETOWYA 14
#define NIEBIESKIA 15
#define ZIELONYA 16
#define RUCH 0.5
#define LEWO 1
#define PRAWO 2
#define CO_ILE_NOWY 30 //sekund
#define ILE_MAKSYMALNIE 20//ile maksymalnie poziomów mo¿e byæ
#define ILE_ZWIEKSZAC 1.05 //ile zwiêkszaæ szybkoœæ po ka¿dym poziomie
#define ILE_ZAPISYWAC  10 // ile osób w rankingu

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki

void Zapisz(int **Pola, long int punkty, double czas_opadniecia, int ile_Obrocony, bool czy_szybsze, int poziom, double czas_gry, int &ile_zapisow)
{
	FILE *plik;
	string nazwa = "zapisy.txt";
	plik = fopen(nazwa.c_str(), "w");
	ile_zapisow--;
	fprintf(plik, "%ld\n", punkty);
	fprintf(plik, "%f\n", czas_opadniecia);
	fprintf(plik, "%d\n", ile_Obrocony);
	fprintf(plik, "%d\n", czy_szybsze);
	fprintf(plik, "%d\n", poziom);
	fprintf(plik, "%f\n", czas_gry);
	fprintf(plik, "%d\n", ile_zapisow);
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			fprintf(plik, "%d ", Pola[i][j]);
		}
		fprintf(plik, "\n");
	}
	fclose(plik);
}
void Wczytaj(int **Pola, long int &punkty, double &czas_opadniecia, int &ile_Obrocony, bool &czy_szybsze, int &poziom, double &czas_gry, int &ile_zapisow)
{
	FILE *plik;
	string nazwa = "zapisy.txt";

	if ((plik = fopen(nazwa.c_str(), "r")) == NULL)
		goto koniec;
	fscanf(plik, "%ld\n", &punkty);
	fscanf(plik, "%f\n", &czas_opadniecia);
	fscanf(plik, "%d\n", &ile_Obrocony);
	fscanf(plik, "%d\n", &czy_szybsze);
	fscanf(plik, "%d\n", &poziom);
	fscanf(plik, "%f\n", &czas_gry);
	fscanf(plik, "%d\n", &ile_zapisow);
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			fscanf(plik, "%d ", &Pola[i][j]);
		}
	}
	fclose(plik);
koniec:;
}

void Zapisz_Najlepszych(string *Najlepsi, long int *punkty_najlepszych, int ile_najlepszych)
{
	FILE *plik;
	string nazwa = "najlepsi.txt";
	plik = fopen(nazwa.c_str(), "w");
	fprintf(plik, "%ld\n", ile_najlepszych);
	for (int i = 0; i < ile_najlepszych; i++)
	{
		//		fprintf(plik, "%s ", Najlepsi[i]);
		fprintf(plik, "%d ", punkty_najlepszych[i]);
		fprintf(plik, "\n");
	}
	fclose(plik);
}
void Wczytaj_Najlepszych(string *Najlepsi, long int *punkty_najlepszych, int &ile_najlepszych)
{
	FILE *plik;
	string nazwa = "najlepsi.txt";

	if ((plik = fopen(nazwa.c_str(), "r")) == NULL)
		goto koniec;
	fscanf(plik, "%ld\n", &ile_najlepszych);

	for (int i = 0; i < ile_najlepszych; i++)
	{
		//	fscanf(plik, "%s ", &Najlepsi[i]);
		fscanf(plik, "%ld\n ", &punkty_najlepszych[i]);
	}
	fclose(plik);
koniec:;
}



void DrawString(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};


// main
#ifdef __cplusplus
extern "C"
#endif




void Ruch(int**Pola, int kierunek_X)
{
	for (int i = 1; i < 25; i++)
	{
		for (int j = 1; j < 11; j++)
		{
			if (Pola[i][j] == CZERWONYA || Pola[i][j] == SZARYA || Pola[i][j] == MORSKIA || Pola[i][j] == ZOLTYA || Pola[i][j] == FIOLETOWYA || Pola[i][j] == NIEBIESKIA || Pola[i][j] == ZIELONYA)
			{
				if (kierunek_X == 0)
					Pola[i - 1][j] = Pola[i][j];
				if (kierunek_X == 1)
					Pola[i - 1][j - 1] = Pola[i][j];
				if (kierunek_X == 2)
					Pola[i - 1][j + 1] = Pola[i][j];
				Pola[i][j] = BRAK;
			}
		}
	}
}
bool czy_Moze_Opadac(int**Pola, int x)
{
	bool czy_Moze = 1;
	for (int i = 1; i < 24; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			if (Pola[i - x][j] == CZERWONYA || Pola[i - x][j] == SZARYA || Pola[i - x][j] == MORSKIA || Pola[i - x][j] == ZOLTYA || Pola[i - x][j] == FIOLETOWYA || Pola[i - x][j] == NIEBIESKIA || Pola[i - x][j] == ZIELONYA)
			{
				if (czy_Moze == 1)
				{
					if (i - x <= 1 || Pola[i - x - 1][j] <= 6)
					{
						if (x > 0)
							return 0;
						czy_Moze = 0;
						i = 1;
						j = 0;
					}
				}
				else
					if (x == 0)
						Pola[i][j] -= 10;

			}
		}
	}
	return czy_Moze;
}

int losuj_klocek()
{
	srand(time(0));
	return rand() % 7;
}
void nowy_klocek(int **Pola)
{
	int wylosowany = losuj_klocek();
	switch (wylosowany)
	{
	case CZERWONY:
		Pola[21][5] = CZERWONYA;
		Pola[22][5] = CZERWONYA;
		Pola[23][5] = CZERWONYA;
		Pola[24][5] = CZERWONYA;
		break;
	case SZARY:
		Pola[21][5] = SZARYA;
		Pola[22][4] = SZARYA;
		Pola[22][5] = SZARYA;
		Pola[22][6] = SZARYA;

		break;

	case MORSKI:
		Pola[21][5] = MORSKIA;
		Pola[21][4] = MORSKIA;
		Pola[22][5] = MORSKIA;
		Pola[22][4] = MORSKIA;
		break;
	case ZOLTY:
		Pola[21][5] = ZOLTYA;
		Pola[21][6] = ZOLTYA;
		Pola[22][5] = ZOLTYA;
		Pola[23][5] = ZOLTYA;

		break;
	case FIOLETOWY:
		Pola[21][5] = FIOLETOWYA;
		Pola[21][6] = FIOLETOWYA;
		Pola[22][6] = FIOLETOWYA;
		Pola[23][6] = FIOLETOWYA;

		break;
	case NIEBIESKI:
		Pola[21][5] = NIEBIESKIA;
		Pola[21][6] = NIEBIESKIA;
		Pola[22][6] = NIEBIESKIA;
		Pola[22][7] = NIEBIESKIA;
		break;
	case ZIELONY:
		Pola[21][5] = ZIELONYA;
		Pola[21][6] = ZIELONYA;
		Pola[22][5] = ZIELONYA;
		Pola[22][4] = ZIELONYA;
		break;

	}

}
void Podmien(int **Pola)
{
	int pozycjaX = 0;
	int pozycjaY = 0;
	for (int i = 1; i < 25; i++)
	{
		for (int j = 1; j < 11; j++)
		{
			if (Pola[i][j] >= 10 && Pola[i][j] <= 16)
			{
				Pola[i][j] = 9;
				if (pozycjaX == 0)
				{
					pozycjaX = j;
					pozycjaY = i;
				}
			}
		}
	}

	for (int i = pozycjaX - 1; i < pozycjaX + 3; i++)
	{
		Pola[pozycjaY][i] = CZERWONYA;
	}


}
bool Czy_Moze_Ruch(int **Pola, int kierunek_X, int x)
{
	if (kierunek_X == PRAWO)
		kierunek_X = 1;
	else if (kierunek_X == LEWO)
		kierunek_X = -1;
	for (int i = 1; i < 24; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			if (Pola[i][j] == CZERWONYA || Pola[i][j] == SZARYA || Pola[i][j] == MORSKIA || Pola[i][j] == ZOLTYA || Pola[i][j] == FIOLETOWYA || Pola[i][j] == NIEBIESKIA || Pola[i][j] == ZIELONYA)
			{

				if (j + kierunek_X == 0 || j + kierunek_X == 11 || Pola[i - x - 1][j + kierunek_X] <= 6)
					return 0;
			}
		}
	}
	return 1;
}

int ObrotNiebieski(int**Pola, int ileObrotow, int *klockiX, int *klockiY, int kolor)
{
	int X = 0, Y = 0;
	switch (ileObrotow)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && Pola[Y + 2][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 2][X] = NIEBIESKIA;
			else if (i == 1)
				Pola[Y + 1][X - 1] = NIEBIESKIA;
			else if (i == 2)
				Pola[Y][X] = NIEBIESKIA;
			else if (i == 3)
				Pola[Y - 1][X - 1] = NIEBIESKIA;
		}
		break;

	case 1:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && Pola[Y + 1][X + 1] != BRAK || X + 1 == 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X - 1] = NIEBIESKIA;
			else if (i == 1)
				Pola[Y - 1][X + 1] = NIEBIESKIA;
			else if (i == 2)
				Pola[Y][X] = NIEBIESKIA;
			else if (i == 3)
				Pola[Y - 1][X + 2] = NIEBIESKIA;
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && Pola[Y + 2][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 2][X] = NIEBIESKIA;
			else if (i == 1)
				Pola[Y + 1][X - 1] = NIEBIESKIA;
			else if (i == 2)
				Pola[Y][X] = NIEBIESKIA;
			else if (i == 3)
				Pola[Y - 1][X - 1] = NIEBIESKIA;
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && Pola[Y + 1][X + 1] != BRAK || X + 1 == 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X - 1] = NIEBIESKIA;
			else if (i == 1)
				Pola[Y - 1][X + 1] = NIEBIESKIA;
			else if (i == 2)
				Pola[Y][X] = NIEBIESKIA;
			else if (i == 3)
				Pola[Y - 1][X + 2] = NIEBIESKIA;
		}
		break;
	}
	return 6;
}
int ObrotZielony(int** Pola, int ileObrotow, int *klockiX, int *klockiY, int kolor)
{
	int X = 0, Y = 0;
	switch (ileObrotow)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Y + 2 >= 25 || Pola[Y + 2][X] != BRAK))
				return ileObrotow;
			else if (i == 1 && Pola[Y + 1][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X - 1] = ZIELONYA;
			else if (i == 1)
				Pola[Y + 1][X - 2] = ZIELONYA;
			else if (i == 2)
				Pola[Y][X + 1] = ZIELONYA;
			else if (i == 3)
				Pola[Y + 1][X] = ZIELONYA;
		}
		break;

	case 1:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && Pola[Y + 1][X - 1] != BRAK || X - 1 == 0)
				return ileObrotow;
			else if (i == 1 && Pola[Y][X + 2] != BRAK || X + 2 >= 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X + 2] = ZIELONYA;
			else if (i == 1)
				Pola[Y - 1][X + 1] = ZIELONYA;
			else if (i == 2)
				Pola[Y][X] = ZIELONYA;
			else if (i == 3)
				Pola[Y - 1][X - 1] = ZIELONYA;
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Pola[Y + 2][X + 1] != BRAK || X + 1 == 11))
				return ileObrotow;
			else if (i == 1 && Pola[Y + 1][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X - 1] = ZIELONYA;
			else if (i == 1)
				Pola[Y + 1][X - 2] = ZIELONYA;
			else if (i == 2)
				Pola[Y][X + 1] = ZIELONYA;
			else if (i == 3)
				Pola[Y + 1][X] = ZIELONYA;
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && Pola[Y + 1][X - 1] != BRAK)
				return ileObrotow;
			else if (i == 1 && Pola[Y][X + 2] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X + 2] = ZIELONYA;
			else if (i == 1)
				Pola[Y - 1][X + 1] = ZIELONYA;
			else if (i == 2)
				Pola[Y][X] = ZIELONYA;
			else if (i == 3)
				Pola[Y - 1][X - 1] = ZIELONYA;
		}
		break;
	}
	return 6;
}
int ObrotFioletowy(int **Pola, int ileObrotow, int *klockiX, int *klockiY, int kolor)
{
	int X = 0, Y = 0;
	switch (ileObrotow)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Y + 1 >= 25 || Pola[Y + 1][X] != BRAK))
				return ileObrotow;
			else if (i == 1 && Pola[Y][X + 1] != BRAK || X + 1 >= 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 1][X] = FIOLETOWYA;
			else if (i == 1)
				Pola[Y][X - 1] = FIOLETOWYA;
			else if (i == 2)
				Pola[Y - 1][X] = FIOLETOWYA;
			else if (i == 3)
				Pola[Y - 2][X + 1] = FIOLETOWYA;
		}
		break;

	case 1:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Y - 2 <= 0 || Pola[Y - 1][X] < 7 || Pola[Y - 2][X] < 7))
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X] = FIOLETOWYA;
			else if (i == 1)
				Pola[Y][X] = FIOLETOWYA;
			else if (i == 2)
				Pola[Y - 1][X - 2] = FIOLETOWYA;
			else if (i == 3)
				Pola[Y - 3][X] = FIOLETOWYA;
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Y + 1 >= 24 || Pola[Y + 1][X + 1] != BRAK) || X + 1 == 11)
				return ileObrotow;
			else if (i == 1 && Pola[Y][X - 1] != BRAK || X - 1 == 0)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 2][X - 1] = FIOLETOWYA;
			else if (i == 1)
				Pola[Y + 1][X] = FIOLETOWYA;
			else if (i == 2)
				Pola[Y][X + 1] = FIOLETOWYA;
			else if (i == 3)
				Pola[Y - 1][X] = FIOLETOWYA;
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 3 && (Y + 1 >= 24 || Pola[Y + 1][X] != BRAK))
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 1][X - 1] = FIOLETOWYA;
			else if (i == 1)
				Pola[Y + 2][X + 2] = FIOLETOWYA;
			else if (i == 2)
				Pola[Y + 1][X + 1] = FIOLETOWYA;
			else if (i == 3)
				Pola[Y][X] = FIOLETOWYA;
		}
		break;
	}
	return 6;
}

int ObrotZolty(int **Pola, int ileObrotow, int *klockiX, int *klockiY, int kolor)
{
	int X = 0, Y = 0;
	switch (ileObrotow)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Y + 2 >= 24 || Pola[Y + 2][X + 1] != BRAK) || X + 1 >= 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 1][X] = kolor;
			else if (i == 1)
				Pola[Y][X - 1] = kolor;
			else if (i == 2)
				Pola[Y][X + 1] = kolor;
			else if (i == 3)
				Pola[Y - 1][X + 2] = kolor;
		}
		break;

	case 1:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0 && (Y - 2 <= 0 || (Pola[Y - 2][X + 1] != BRAK || Pola[Y - 1][X + 1] != BRAK)) || X + 1 == 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X] = kolor;
			else if (i == 1)
				Pola[Y - 1][X + 1] = kolor;
			else if (i == 2)
				Pola[Y - 2][X] = kolor;
			else if (i == 3)
				Pola[Y - 3][X - 1] = kolor;
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 2 && Pola[Y][X - 1] != BRAK || X - 1 == 0)
				return ileObrotow;
			else if (i == 3 && Pola[Y + 1][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 2][X - 2] = kolor;
			else if (i == 1)
				Pola[Y + 1][X - 1] = kolor;
			else if (i == 2)
				Pola[Y][X + 1] = kolor;
			else if (i == 3)
				Pola[Y + 1][X] = kolor;
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 3 && Y + 3 < 25 && Pola[Y + 3][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 3][X + 1] = kolor;
			else if (i == 1)
				Pola[Y + 2][X] = kolor;
			else if (i == 2)
				Pola[Y + 1][X - 1] = kolor;
			else if (i == 3)
				Pola[Y][X] = kolor;
		}
		break;

	}
	return 6;
}
int ObrotSzary(int **Pola, int ileObrotow, int *klockiX, int *klockiY, int kolor)
{
	int X = 0, Y = 0;
	switch (ileObrotow)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 2 && Pola[Y + 1][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X] = kolor;
			else if (i == 1)
				Pola[Y][X] = kolor;
			else if (i == 2)
				Pola[Y][X] = kolor;
			else if (i == 3)
				Pola[Y + 1][X - 1] = kolor;
		}
		break;

	case 1:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 2 && Y >= 2 && Pola[Y][X + 1] != BRAK || X + 1 == 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 1][X + 1] = kolor;
			else if (i == 1)
				Pola[Y][X] = kolor;
			else if (i == 2)
				Pola[Y][X] = kolor;
			else if (i == 3)
				Pola[Y][X] = kolor;
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 1 && Pola[Y - 1][X] != BRAK)
				return ileObrotow;
			else if (i == 3 && Pola[Y + 1][X] != BRAK)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y - 1][X + 1] = kolor;
			else if (i == 1)
				Pola[Y][X] = kolor;
			else if (i == 2)
				Pola[Y][X] = kolor;
			else if (i == 3)
				Pola[Y][X] = kolor;
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 3 && Pola[Y + 1][X - 1] != BRAK || X - 1 == 0)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y][X] = kolor;
			else if (i == 1)
				Pola[Y][X] = kolor;
			else if (i == 2)
				Pola[Y][X] = kolor;
			else if (i == 3)
				Pola[Y - 1][X - 1] = kolor;
		}
		break;

	}
	return 6;
}
int ObrotCzerwony(int **Pola, int ileObrotow, int *klockiX, int *klockiY, int kolor)
{
	int X = 0, Y = 0;
	switch (ileObrotow)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 1 && (Pola[Y][X - 1] != BRAK || Pola[Y][X + 2] != BRAK || Pola[Y][X + 1] != BRAK) || X - 1 == 0 || X + 2 >= 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 2][X - 1] = kolor;
			else if (i == 1)
				Pola[Y + 1][X] = kolor;
			else if (i == 2)
				Pola[Y][X + 1] = kolor;
			else if (i == 3)
				Pola[Y - 1][X + 2] = kolor;
		}
		break;

	case 1:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 1 && (Pola[Y + 1][X] != BRAK || Pola[Y - 1][X] != BRAK || Pola[Y - 2][X] != BRAK) || Y - 2 <= 0)
				return 0;
		}
		ileObrotow++;
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y - 2][X + 1] = kolor;
			else if (i == 1)
				Pola[Y - 1][X] = kolor;
			else if (i == 2)
				Pola[Y][X - 1] = kolor;
			else if (i == 3)
				Pola[Y + 1][X - 2] = kolor;
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 1 && (Pola[Y][X - 1] != BRAK || Pola[Y][X + 2] != BRAK || Pola[Y][X + 1] != BRAK) || X - 1 == 0 || X + 2 >= 11)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y + 2][X - 1] = kolor;
			else if (i == 1)
				Pola[Y + 1][X] = kolor;
			else if (i == 2)
				Pola[Y][X + 1] = kolor;
			else if (i == 3)
				Pola[Y - 1][X + 2] = kolor;
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 1 && (Pola[Y + 1][X] != BRAK || Pola[Y - 1][X] != BRAK || Pola[Y - 2][X] != BRAK) || Y - 2 <= 0)
				return ileObrotow;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			Pola[Y][X] = BRAK;
		}
		for (int i = 0; i < 4; i++)
		{
			X = klockiX[i];
			Y = klockiY[i];
			if (i == 0)
				Pola[Y - 2][X + 1] = kolor;
			else if (i == 1)
				Pola[Y - 1][X] = kolor;
			else if (i == 2)
				Pola[Y][X - 1] = kolor;
			else if (i == 3)
				Pola[Y + 1][X - 2] = kolor;
		}
		break;

	}
	return 6;
}
int WykonajObrot(int**Pola, int ileObrotow)
{
	int wynik;
	int klockiX[4] = {};
	int klockiY[4] = {};
	int z = 0;
	int kolor = 0;
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			if (Pola[i][j] >= 10)
			{
				klockiX[z] = j;
				klockiY[z] = i;
				kolor = Pola[i][j];
				z++;
			}
		}
	}
	switch (kolor)
	{
	case NIEBIESKIA:
	{
		wynik = ObrotNiebieski(Pola, ileObrotow, klockiX, klockiY, kolor);
	}
	break;
	case ZIELONYA:
	{
		wynik = ObrotZielony(Pola, ileObrotow, klockiX, klockiY, kolor);
	}
	break;
	case FIOLETOWYA:
	{
		wynik = ObrotFioletowy(Pola, ileObrotow, klockiX, klockiY, kolor);
	}
	break;
	case ZOLTYA:
	{
		wynik = ObrotZolty(Pola, ileObrotow, klockiX, klockiY, kolor);
	}
	break;


	case SZARYA:
	{
		wynik = ObrotSzary(Pola, ileObrotow, klockiX, klockiY, kolor);
	}
	break;
	case CZERWONYA:
	{
		wynik = ObrotCzerwony(Pola, ileObrotow, klockiX, klockiY, kolor);
	}
	break;

	}
	if (wynik == 6)
		ileObrotow++;
	if (ileObrotow == 4)ileObrotow = 0;
	return ileObrotow;
}
void Usun_wiersz(int **Pola, int rzad)
{
	for (int i = rzad; i < 22; i++)
	{
		for (int j = 1; j < 11; j++)
		{
			Pola[i][j] = Pola[i + 1][j];
		}
	}
}

int SprawdzPlansze(int **Pola)
{
	bool czy_usunac;
	int ile_usunietych = 0;
	for (int i = 1; i < 22; i++)
	{
		czy_usunac = 1;
		for (int j = 1; j < 11; j++)
		{
			if (i == 20 && Pola[20][j] != 9)
				return 5;
			if (Pola[i][j] > 7)
			{
				czy_usunac = 0;

			}


		}
		if (czy_usunac == 1)
		{
			Usun_wiersz(Pola, i);
			ile_usunietych++;
			i = 0;
		}
	}
	return ile_usunietych;
}
int LiczPunkty(int**Pola, int poziom, int usuniete_teraz, int usuniete_wczesniej)
{
	switch (usuniete_teraz)
	{
	case 1:
		return(100 * (poziom + 1));
		break;
	case 2:
		return (200 * (poziom + 1));
		break;
	case 3:
		return (400 * (poziom + 1));
		break;
	case 4:
		if (usuniete_wczesniej != 4)
			return(800 * (poziom + 1));
		else
			return(1200 * (poziom + 1));
		break;
	}
	return 0;
}
void Od_nowa(int **Pola, int &next_kierunek_X, bool &czy_obrot, int &ile_Obrocony, int &poziom, long int &punkty, double &worldTime, int &usuniete_wczesniej, double &czas_opadniecia, double &od_ostatniego, double &od_przyspieszenia, double &delta, int &ile_zapisow)
{

	for (int i = 1; i < 25; i++)
	{
		for (int j = 1; j < 11; j++)
			Pola[i][j] = BRAK;
	}
	next_kierunek_X = 0;
	czy_obrot = 0;
	ile_Obrocony = 0;
	poziom = 0;
	punkty = 0;
	worldTime = 0;
	usuniete_wczesniej = 0;
	czas_opadniecia = 0.5 / SZYBKOSC;
	od_ostatniego = 0;
	od_przyspieszenia = 0;
	delta = 0;
	ile_zapisow = 2;

}


int main(int argc, char **argv) {
	bool zresetowane = 0;
	int usuniete_wczesniej = 0, usuniete_teraz = 0;
	int kierunek_X = 0, next_kierunek_X = 0;
	bool czy_opadajacy = 0, czy_obrot = 0;
	int przesuniecie_X = 0, przesuniecie_Y = 0;
	double czas_opadniecia = 0.5 / SZYBKOSC, czas_wczesniej = 0;;
	int poziom = 0;
	double od_ostatniego = 0;
	int etap = 0;
	bool czy_klocek;
	int **Pola;
	char **nicki;
	nicki = new char *[ILE_ZAPISYWAC];
	for (int i = 0; i < ILE_ZAPISYWAC; i++)
	{
		nicki[i] = new char[22];
	}
	Pola = new int *[26];
	long int Twoje_Punkty = 0;
	string * Najlepsi = new string[ILE_ZAPISYWAC];
	long int * Punkty_najlepszych = new long int[ILE_ZAPISYWAC];
	for (int i = 0; i < ILE_ZAPISYWAC; i++)
	{
		Najlepsi[i] = "";
		Punkty_najlepszych[i] = i;
	}
	for (int i = 0; i < 26; i++)
		Pola[i] = new int[12];
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			Pola[i][j] = BRAK;
		}
	}
	long int punkty = 0;
	const int BOK = round(SCREEN_HEIGHT / 22);
	const int LEGENDA = (SCREEN_WIDTH - 12 * BOK);
	int t1, t2, quit, frames, rc, pause;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	double co_ile = RUCH / BOK;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe³noekranowy
	//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
	//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Tetris Arkadiusz Ciosek s161340");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	eti = SDL_LoadBMP("./eti.bmp");
	if (eti == NULL) {
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int szary = SDL_MapRGB(screen->format, 157, 161, 170);
	int morski = SDL_MapRGB(screen->format, 132, 195, 190);
	int zolty = SDL_MapRGB(screen->format, 229, 190, 1);
	int fioletowy = SDL_MapRGB(screen->format, 160, 052, 114);
	int szarosc = SDL_MapRGB(screen->format, 78, 87, 84);


	t1 = SDL_GetTicks();
	int przesuwaj = 0;
	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;
	int kolor;
	nowy_klocek(Pola);
	int ile_Obrocony = 0;
	int Twoja_pozycja;
	double od_przyspieszenia = 0;
	int ile_zapisow = 3;
	bool szybkie_opadanie = 0;
	bool koniec_gry = 0;
	bool pauza = 0;
	int ile_najlepszych = 0;
	long int Brakuje_punktow = 0;
	Wczytaj_Najlepszych(Najlepsi, Punkty_najlepszych, ile_najlepszych);
	while (!quit) {
		if (zresetowane == 1)
		{
			nowy_klocek(Pola);
			zresetowane = 0;
		}
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		distance += etiSpeed * delta;

		SDL_FillRect(screen, NULL, czarny);
		Twoje_Punkty = punkty;
		przesuwaj = ile_najlepszych;

		for (int i = ile_najlepszych - 1; i >= 0; i--)
		{
			if (ile_najlepszych != 0 && Twoje_Punkty > Punkty_najlepszych[i])
				przesuwaj = i;
			else
				break;
		}

		if (przesuwaj != ile_najlepszych)
		{
			Twoja_pozycja = przesuwaj + 1;
		}
		if (przesuwaj == ile_najlepszych)
		{
			if (ILE_ZAPISYWAC > ile_najlepszych)
				Twoja_pozycja = ile_najlepszych;
			else

				Twoja_pozycja = ile_najlepszych + 1;
		}
		Brakuje_punktow = Punkty_najlepszych[Twoja_pozycja - 2] - Twoje_Punkty + 1;




		//	DrawSurface(screen, eti,
		//            SCREEN_WIDTH  + sin(distance) * SCREEN_HEIGHT ,
		//		    SCREEN_HEIGHT  + cos(distance) * SCREEN_HEIGHT );
		//		DrawScreen(screen, plane, ship, charset, worldTime, delta, vertSpeed);
		// naniesienie wyniku rysowania na rzeczywisty ekran
		//		SDL_Flip(screen);

		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};
		od_ostatniego += delta;
		od_przyspieszenia = od_przyspieszenia + delta;
		if (od_przyspieszenia >= CO_ILE_NOWY && poziom < ILE_MAKSYMALNIE)
		{
			poziom++;
			czas_opadniecia = czas_opadniecia / ILE_ZWIEKSZAC;
			od_przyspieszenia = 0;
		}
		if (od_ostatniego >= czas_opadniecia)
		{
			if (czy_Moze_Opadac(Pola, 0) == 1)
			{
				if (kierunek_X != 0 && Czy_Moze_Ruch(Pola, kierunek_X, 0) == 0)
					kierunek_X = 0;
				Ruch(Pola, kierunek_X);
				if (czy_obrot == 1)
					ile_Obrocony = WykonajObrot(Pola, ile_Obrocony);
				czy_obrot = 0;
			}
			else
			{
				if (szybkie_opadanie == 1)
				{
					czas_opadniecia = czas_wczesniej;
					czas_wczesniej = 0;
					szybkie_opadanie = 0;
				}
				usuniete_teraz = SprawdzPlansze(Pola);
				if (usuniete_teraz > 0)
				{

					punkty += LiczPunkty(Pola, poziom, usuniete_teraz, usuniete_wczesniej);
					usuniete_wczesniej = usuniete_teraz;
				}
				if (usuniete_teraz == 5) {
					koniec_gry = 1;
				}
				nowy_klocek(Pola);
				ile_Obrocony = 0;
			}
			kierunek_X = next_kierunek_X;
			next_kierunek_X = 0;
			od_ostatniego = 0;
		}
		//		DrawRectangle(screen, 33,33, 55,55, czarny,niebieski);
		for (int i = 0; i < 22; i++)
		{
			for (int j = 0; j < 12; j++)
			{
				czy_klocek = 1;
				czy_opadajacy = 0;
				switch (Pola[i][j])
				{
				case CZERWONY:
					kolor = czerwony;
					break;
				case SZARY:
					kolor = szary;
					break;
				case MORSKI:
					kolor = morski;
					break;
				case ZOLTY:
					kolor = zolty;
					break;
				case FIOLETOWY:
					kolor = fioletowy;
					break;
				case NIEBIESKI:
					kolor = niebieski;
					break;
				case ZIELONY:
					kolor = zielony;
					break;
				case ZIELONYA:
					kolor = zielony;
					czy_opadajacy = 1;
					break;
				case ZOLTYA:
					kolor = zolty;
					czy_opadajacy = 1;
					break;
				case CZERWONYA:
					kolor = czerwony;
					czy_opadajacy = 1;
					break;
				case SZARYA:
					kolor = szary;
					czy_opadajacy = 1;
					break;
				case MORSKIA:
					kolor = morski;
					czy_opadajacy = 1;
					break;
				case FIOLETOWYA:
					kolor = fioletowy;
					czy_opadajacy = 1;
					break;
				case NIEBIESKIA:
					kolor = niebieski;
					czy_opadajacy = 1;
					break;
				default:
					czy_klocek = 0;
					break;
				}
				if (czy_klocek == 1 && i >= 1 && i != 21 && czy_Moze_Opadac(Pola, 1) == 1)
				{
					if (czy_opadajacy == 0)
						DrawRectangle(screen, j*BOK, (21 - i)*BOK, BOK, BOK, -1, kolor);
					else if (kierunek_X == 0 || kierunek_X != 0 && Czy_Moze_Ruch(Pola, kierunek_X, 1) == 0)
						DrawRectangle(screen, j*BOK, (21 - i)*BOK + od_ostatniego / czas_opadniecia*BOK, BOK, BOK, -1, kolor);
					else if (kierunek_X == PRAWO)
						DrawRectangle(screen, j*BOK + od_ostatniego / czas_opadniecia*BOK, (21 - i)*BOK + od_ostatniego / czas_opadniecia*BOK, BOK, BOK, -1, kolor);
					else if (kierunek_X == LEWO)
						DrawRectangle(screen, j*BOK - od_ostatniego / czas_opadniecia*BOK, (21 - i)*BOK + od_ostatniego / czas_opadniecia*BOK, BOK, BOK, -1, kolor);
				}
				else if (czy_klocek == 1 && i >= 1 && i != 21)
					DrawRectangle(screen, j*BOK, (21 - i)*BOK, BOK, BOK, -1, kolor);
				if (i == 0 || j == 0 || j == 11 || i == 21)
					DrawRectangle(screen, j*BOK, (21 - i)*BOK, BOK, BOK, -1, szarosc);
			}
		}
		DrawRectangle(screen, SCREEN_WIDTH - LEGENDA, 0, LEGENDA, SCREEN_HEIGHT, czerwony, niebieski);
		sprintf(text, "Liczba punktow= %d", punkty);
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 2) * 5 / 5 * NAPISY_Y, text, charset);
		if (Twoja_pozycja - 1 < ILE_ZAPISYWAC)sprintf(text, "Miejsce:%d ", Twoja_pozycja);
		else sprintf(text, "Nie jestes klasyfikowany w rankingu. ");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 2) * 4 / 5 * NAPISY_Y, text, charset);
		if (Twoja_pozycja != 1)sprintf(text, "Brakuje Ci %ld punktow do kolejnego miejsca ", Brakuje_punktow);
		else sprintf(text, "Gratuluje! Zajmujesz pierwsze miejsce!");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 2) * 3 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "Czas trwania = %.1lf s Poziom= %d", worldTime, poziom);
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 2) * 2 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "%.0lf klatek / s", fps);
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 2) * 1 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "N - nowa gra P- pauza");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 2) * 0 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "S - zapis (%d dostepne)", ile_zapisow);
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 + (SCREEN_HEIGHT / 2) * 1 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "U- przyspieszenie");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 + (SCREEN_HEIGHT / 2) * 2 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "Esc - wyjscie");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 + (SCREEN_HEIGHT / 2) * 3 / 5 * NAPISY_Y, text, charset);
		sprintf(text, "\031 natychmiatowe opadniecie");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 + (SCREEN_HEIGHT / 2) * 4 / 5 * NAPISY_Y, text, charset);
		sprintf(text, " \030 lub spacja - obrot");
		DrawString(screen, SCREEN_WIDTH - LEGENDA + (LEGENDA - strlen(text) * 8)* NAPISY_X, SCREEN_HEIGHT / 2 + (SCREEN_HEIGHT / 2) * 5 / 5 * NAPISY_Y - 10, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		//	SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y)
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_LEFT) next_kierunek_X = LEWO;
				else if (event.key.keysym.sym == SDLK_RIGHT) next_kierunek_X = PRAWO;
				else if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP) czy_obrot = 1;
				else if (event.key.keysym.sym == SDLK_p) {
					pauza = 1; nicki[0] = nicki[0] + 'x';
				}
				else if (event.key.keysym.sym == SDLK_k) {
					Podmien(Pola);
				}
				else if (event.key.keysym.sym == SDLK_a)nicki[0] = nicki[0] + 'a';
				else if (event.key.keysym.sym == SDLK_u) od_przyspieszenia = CO_ILE_NOWY;
				else if (event.key.keysym.sym == SDLK_n && worldTime > 2) {
					Od_nowa(Pola, next_kierunek_X, czy_obrot, ile_Obrocony, poziom, punkty, worldTime, usuniete_wczesniej, czas_opadniecia, od_ostatniego, od_przyspieszenia, delta, ile_zapisow); zresetowane = 1;
					szybkie_opadanie = 0;
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					if (szybkie_opadanie == 0)
						czas_wczesniej = czas_opadniecia;
					szybkie_opadanie = 1;
					czas_opadniecia = 0.03*SZYBKIE_OPADANIE;
				}
				else if (event.key.keysym.sym == SDLK_s && ile_zapisow > 0) Zapisz(Pola, punkty, czas_opadniecia, ile_Obrocony, szybkie_opadanie, poziom, worldTime, ile_zapisow);
				else if (event.key.keysym.sym == SDLK_l) Wczytaj(Pola, punkty, czas_opadniecia, ile_Obrocony, szybkie_opadanie, poziom, worldTime, ile_zapisow);

				break;
			case SDL_KEYUP:
				quit = 0;
				if (szybkie_opadanie == 1)
				{
					czas_opadniecia = czas_wczesniej;
					czas_wczesniej = 0;
					szybkie_opadanie = 0;
				}
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		while (pauza == 1)
		{

			SDL_FillRect(screen, NULL, czarny);
			DrawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, czerwony, niebieski);
			sprintf(text, "pauza");
			DrawString(screen, LEGENDA, BOK * 11, text, charset);
			while (SDL_PollEvent(&event)) {
				if (event.type != SDL_KEYDOWN)
					continue;
				if (event.key.keysym.sym == SDLK_p)
				{
					pauza = 0;
					t1 = SDL_GetTicks();
					t2 = SDL_GetTicks();
				}
			};
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//	SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
		frames++;




		//SDLK_RETURN - enter
		if (koniec_gry == 1)
		{
			//	przesuwaj = ile_najlepszych;
			if (Twoje_Punkty == 0)
				goto wyniki;

			for (int i = ile_najlepszych - 1; i >= 0; i--)
			{
				if (ile_najlepszych != 0 && Twoje_Punkty > Punkty_najlepszych[i])
					przesuwaj = i;
				else
					break;
			}






			if (przesuwaj != ile_najlepszych)
			{
				Twoja_pozycja = przesuwaj + 1;
				if (ILE_ZAPISYWAC > ile_najlepszych)
					Punkty_najlepszych[ile_najlepszych] = Punkty_najlepszych[ile_najlepszych - 1];
				for (int i = ile_najlepszych - 1; i > przesuwaj; i--)
				{
					Punkty_najlepszych[i] = Punkty_najlepszych[i - 1];
				}
				if (ILE_ZAPISYWAC > ile_najlepszych)
					ile_najlepszych++;
			}
			if (przesuwaj == ile_najlepszych && ile_najlepszych < ILE_ZAPISYWAC)
			{
				Punkty_najlepszych[ile_najlepszych] = Twoje_Punkty;
				Twoja_pozycja = ile_najlepszych + 1;
				ile_najlepszych++;
			}
			if (Punkty_najlepszych[Twoja_pozycja - 1] < Twoje_Punkty)
				Punkty_najlepszych[Twoja_pozycja - 1] = Twoje_Punkty;
		}
		while (koniec_gry == 1)
		{
		wyniki:
			SDL_FillRect(screen, NULL, czarny);
			DrawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, czerwony, niebieski);
			sprintf(text, "Koniec gry");
			DrawString(screen, LEGENDA, BOK * 8, text, charset);
			sprintf(text, "Zdobyles %ld punktow", Twoje_Punkty);
			DrawString(screen, LEGENDA, BOK * 9, text, charset);
			//	sprintf(text, "Wprowadz swoj nick:%s",Twoj_nick);
			DrawString(screen, LEGENDA, BOK * 10, text, charset);
			sprintf(text, "Zajales %d miejsce!", Twoja_pozycja);
			DrawString(screen, LEGENDA, BOK * 11, text, charset);
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						koniec_gry = 0;
						Od_nowa(Pola, next_kierunek_X, czy_obrot, ile_Obrocony, poziom, punkty, worldTime, usuniete_wczesniej, czas_opadniecia, od_ostatniego, od_przyspieszenia, delta, ile_zapisow); zresetowane = 1;
						szybkie_opadanie = 0;
						if (ile_najlepszych < ILE_ZAPISYWAC)
							ile_najlepszych++;
						Zapisz_Najlepszych(Najlepsi, Punkty_najlepszych, ile_najlepszych); Twoje_Punkty = 0;

					}
					else if (event.key.keysym.sym == SDLK_n && worldTime > 2) {
						Od_nowa(Pola, next_kierunek_X, czy_obrot, ile_Obrocony, poziom, punkty, worldTime, usuniete_wczesniej, czas_opadniecia, od_ostatniego, od_przyspieszenia, delta, ile_zapisow); zresetowane = 1;
						szybkie_opadanie = 0;
						koniec_gry = 0;
						if (ile_najlepszych < ILE_ZAPISYWAC)
							ile_najlepszych++;
						Zapisz_Najlepszych(Najlepsi, Punkty_najlepszych, ile_najlepszych); Twoje_Punkty = 0;
					}
					else if (event.key.keysym.sym == SDLK_l) {
						Wczytaj(Pola, punkty, czas_opadniecia, ile_Obrocony, szybkie_opadanie, poziom, worldTime, ile_zapisow); koniec_gry = 0;
					}
					else if (event.key.keysym.sym == SDLK_a)

						break;
				case SDL_KEYUP:
					quit = 0;
					break;
				case SDL_QUIT:
					koniec_gry = 0;
					Od_nowa(Pola, next_kierunek_X, czy_obrot, ile_Obrocony, poziom, punkty, worldTime, usuniete_wczesniej, czas_opadniecia, od_ostatniego, od_przyspieszenia, delta, ile_zapisow); zresetowane = 1;
					szybkie_opadanie = 0;
					Zapisz_Najlepszych(Najlepsi, Punkty_najlepszych, ile_najlepszych); Twoje_Punkty = 0;
					if (ile_najlepszych < ILE_ZAPISYWAC)
						ile_najlepszych++;
					break;
				};
			};
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//	SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);
		};

	};

	// zwolnienie powierzchni
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};
