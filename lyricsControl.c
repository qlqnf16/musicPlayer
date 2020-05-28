#include <string.h>
#include "device_driver.h"

const char songLyricses[][300] = {"I see trees \nof green\nRed roses too\nI see them bloom\nFor me and you\nAnd I think to \nmyself...\nWhat a \nwonderful world\nI see skies\nof blue\nAnd clouds \nof white\nThe bright\nblessed day\nThe dark\nsacred night\nAnd I think to\nmyself...\nWhat a\nwonderful world" };
const int lyricsTimestamp[][300] = {{6, 8, 11, 14, 17, 21, 22, 26, 27, 34, 36, 39, 40, 42, 43, 45, 46, 48, 50, 53, 54}};
char * currentLyrics[50];
int currentTimeStamps, timeStampIdx;

void drawLyrics(int idx) {
	int i;
	for (i = 0; i < 7; i++) {
		Lcd_Printf(170, 90 + (i * 20), WHITE, BLACK, 1, 1, "%s", currentLyrics[i]);
	}
}

void initLyrics(int idx) {
	int i = 0;
	char *p;
	p = strtok(songLyricses[idx], "\n");
	while (p) {
		currentLyrics[i++] = p;
		p = strtok(0, "\n");
	}
	currentLyrics[i] = 0;
	currentTimeStamps = i;
	timeStampIdx = 0;

	drawLyrics(idx);
}

void checkLyrics(int idx, int sec) {
	int i, j;
	if (sec != lyricsTimestamp[idx][timeStampIdx]) return;
	if (timeStampIdx < 4) {
		if (timeStampIdx > 0) {
			Lcd_Printf(170, 90 + ((timeStampIdx-1) * 20), WHITE, BLACK, 1, 1, "%s", currentLyrics[timeStampIdx-1]);
		}
		Lcd_Printf(170, 90 + (timeStampIdx * 20), YELLOW, BLACK, 1, 1, "%s", currentLyrics[timeStampIdx]);
	}
	else {
		Lcd_Draw_Bar(170, 90, 330, 230, BLACK);
		for (i = timeStampIdx - 3, j = 0; i < timeStampIdx + 4; i++, j++) {
			if (i >= currentTimeStamps) break;
			if (i == timeStampIdx) {
				Lcd_Printf(170, 90 + (j * 20), YELLOW, BLACK, 1, 1, "%s", currentLyrics[i]);
			}
			else {
				Lcd_Printf(170, 90 + (j * 20), WHITE, BLACK, 1, 1, "%s", currentLyrics[i]);
			}
		}
	}
	timeStampIdx ++;
}
