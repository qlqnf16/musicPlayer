#include <string.h>
#include "device_driver.h"

#include "./Images/backArrow.h"
#include "./Images/arrow1.h"
#include "./Images/arrow2.h"
#include "./Images/play.h"
#include "./Images/volume.h"
#include "./Images/repeat.h"
#include "./Images/repeatActive.h"
#include "./Images/shuffle.h"
#include "./Images/shuffleActive.h"
#include "./Images/scrollUp.h"
#include "./Images/scrollDown.h"
#include "./Images/albumart1.h"
#include "./Images/albumart2.h"
#include "./Images/albumart3.h"

extern void initLyrics(int);
void drawMusicCard(int, int);

const char songTitles[][50] = { "What A Wonderful World", "We Will Rock You",  "If I Die Tomorrow", "Sweet but Psycho", "for him.", "Keep You Mine"};
const char songArtists[][50] = { "Louis Armstrong", "Queen", "Beenzino", "Ava Max", "Troye Sivan", "NOTD"};
const unsigned short * albumArts[] = { albumart1, albumart2, albumart3 };

int listTop, listBottom;

void drawPlayerUI(int idx, int vol) {
	Lcd_Set_Shape_Mode(0, 0xFFFe);
	Lcd_Clr_Screen(BLACK);
	Lcd_Draw_BMP(6, 8, backArrow);

	// play control
	Lcd_Draw_BMP(24, 185, arrow2);
	Lcd_Draw_Bar(77, 187, 85, 214, WHITE);
	Lcd_Draw_Bar(91, 187, 99, 214, WHITE);
	Lcd_Draw_BMP(121, 185, arrow1);

	// volume control
	Lcd_Draw_BMP(170, 24, volume);
	Lcd_Draw_Bar(206, 30, 206 + 18, 36, WHITE);
	Lcd_Printf(248, 24, WHITE, BLACK, 1, 1, "%d", vol);
	Lcd_Draw_Bar(278, 30, 278 + 18, 36, WHITE);
	Lcd_Draw_Bar(284, 24, 290, 24 + 18, WHITE);
	// play list control
	Lcd_Draw_BMP(170, 60, shuffle);
	Lcd_Draw_BMP(210, 60, repeat);
}

void drawSongUI(int idx) {
	// �ٹ���Ʈ
	Lcd_Draw_BMP(24, 24, albumArts[idx]);
	// progress bar
	Lcd_Draw_Bar(24, 170, 24 + 128, 172, GREY);
	// ����
	initLyrics(idx);
}

void showMusicList(void) {
	int i;
	listTop = 0;
	listBottom = 3;

	Lcd_Clr_Screen(BLACK);
	Lcd_Draw_BMP(298, 10, scrollUp);
	Lcd_Draw_BMP(298, 218, scrollDown);
	for (i = 0; i < 4; i++) {
		drawMusicCard(i, i);
		Lcd_Draw_Hline(55 * (i + 1), 10, 290, WHITE);
	}
}

void drawMusicCard(int i, int idx) {
	Lcd_Draw_Bar(10, 10 + 55 * i, 290, 50 + 55 * i, BLACK);
	Lcd_Printf(10, 10 + 55 * i, WHITE, BLACK, 1, 1, "%s", songTitles[idx]);
	Lcd_Printf(10, 30 + 55 * i, LGREY, BLACK, 1, 1, "%s", songArtists[idx]);
}

void moveMusicList(int up) {
	int i, j;

	if (up) {
		listTop--;
		listBottom--;
	} else {
		listTop++;
		listBottom++;
	}
	for (i = listTop, j = 0; i <= listBottom; i++, j++) {
		drawMusicCard(j, i);
	}
}

void togglePlayIcon(int paused) {
	Lcd_Draw_Bar(73, 185, 104, 216, BLACK);
	if (!paused) {
		Lcd_Draw_Bar(77, 187, 85, 214, WHITE);
		Lcd_Draw_Bar(91, 187, 99, 214, WHITE);
		return;
	}
	Lcd_Draw_BMP(73, 185, play);
}

void showVolume(int vol) {
	Lcd_Printf(248, 24, WHITE, BLACK, 1, 1, "%d", vol);
}

void drawProgressBar(int x1, int x2) {
	Lcd_Draw_Bar(24 + x1, 170, 24 + x2, 172, YELLOW);
}

void toggleShuffleIcon(int active) {
	if (active)
		Lcd_Draw_BMP(170, 60, shuffleActive);
	else
		Lcd_Draw_BMP(170, 60, shuffle);
}

void toggleRepeatIcon(int active) {
	if (active)
		Lcd_Draw_BMP(210, 60, repeatActive);
	else
		Lcd_Draw_BMP(210, 60, repeat);
}
