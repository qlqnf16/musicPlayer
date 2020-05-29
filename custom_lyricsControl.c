#include <string.h>
#include "device_driver.h"

#define NUMS_OF_SONG	6

char songLyricses[][500] = {\
		"I see trees \nof green\nRed roses too\nI see them bloom\nFor me and you\nAnd I think to \nmyself...\nWhat a \nwonderful world\nI see skies\nof blue\nAnd clouds \nof white\nThe bright\nblessed day\nThe dark\nsacred night\nAnd I think to\nmyself...\nWhat a\nwonderful world", \
		"Buddy you're a boy\nmake a big noise\nPlaying in the\nstreet, \ngonna be a big man \nsomeday You got \nmud on your face, \nyou big disgrace\nKicking your can \nall over the place\nsingin'\nWe will, we will \nrock you\nWe will, we will \nrock you\nBuddy, you're \na young man, \nhard man\nShouting in the \n\nstreet, gonna \ntake on the world\nsomeday\nYou got blood\non your face,\nyou big disgrace",
		"no lyrics", "no lyrics", "no lyrics", "no lyrics"
};
const int lyricsTimestamp[][300] = {
		{6, 8, 11, 14, 17, 21, 22, 26, 27, 34, 36, 39, 40, 42, 43, 45, 46, 48, 50, 53, 54},
		{12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 27, 30, 33,  35, 36, 37, 38, 39, 40, 41, 42, 43, 44},
		{0}, {0}, {0}, {0}
};
char * splitedLyrics[NUMS_OF_SONG][50];
int numsOfTimestamp[NUMS_OF_SONG];
int timeStampIdx;

void drawLyrics(int idx) {
	int i;
	for (i = 0; i < 7; i++) {
		if (i >= numsOfTimestamp[idx]) break;
		Lcd_Printf(170, 90 + (i * 20), WHITE, BLACK, 1, 1, "%s", splitedLyrics[idx][i]);
	}
}

void initLyrics(int idx) {
	int i = 0;
	char *p;
	if (!numsOfTimestamp[idx]) {
		p = strtok(songLyricses[idx], "\n");
		while (p) {
			splitedLyrics[idx][i++] = p;
			p = strtok(0, "\n");
		}
		splitedLyrics[idx][i] = 0;
		numsOfTimestamp[idx] = i;
	}
	timeStampIdx = 0;

	Lcd_Draw_Bar(170, 90, 330, 230, BLACK);
	drawLyrics(idx);
}

void checkLyrics(int idx, int sec) {
	int i, j;
	if (sec != lyricsTimestamp[idx][timeStampIdx]) return;
	if (timeStampIdx < 4) {
		if (timeStampIdx > 0) {
			Lcd_Printf(170, 90 + ((timeStampIdx-1) * 20), WHITE, BLACK, 1, 1, "%s", splitedLyrics[idx][timeStampIdx-1]);
		}
		Lcd_Printf(170, 90 + (timeStampIdx * 20), YELLOW, BLACK, 1, 1, "%s", splitedLyrics[idx][timeStampIdx]);
	}
	else {
		Lcd_Draw_Bar(170, 90, 330, 230, BLACK);
		for (i = timeStampIdx - 3, j = 0; i < timeStampIdx + 4; i++, j++) {
			if (i >= numsOfTimestamp[idx]) break;
			if (i == timeStampIdx) {
				Lcd_Printf(170, 90 + (j * 20), YELLOW, BLACK, 1, 1, "%s", splitedLyrics[idx][i]);
			}
			else {
				Lcd_Printf(170, 90 + (j * 20), WHITE, BLACK, 1, 1, "%s", splitedLyrics[idx][i]);
			}
		}
	}
	timeStampIdx ++;
}
