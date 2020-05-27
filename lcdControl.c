
#include "device_driver.h"

#define BLACK	0x0000
#define WHITE 	0xffff
#define BLUE	0x003e

#include "./Images/arrow1.h"
#include "./Images/arrow2.h"
#include "./Images/play.h"
#include "./Images/volume.h"
#include "./Images/repeat.h"
#include "./Images/repeatActive.h"
#include "./Images/shuffle.h"
#include "./Images/shuffleActive.h"

const char songTitles[][50] = {"What A Wonderful World", "We Will Rock You"};
const char songArtists[][50] = {"Louis Armstrong", "Queen"};

void drawPlayUI(int idx, int vol) {
	Lcd_Set_Shape_Mode(0, 0xFFFe);
	Lcd_Clr_Screen(BLACK);
	// ¾Ù¹ü¾ÆÆ®
	Lcd_Draw_Bar(24, 24, 24+128, 24+128, WHITE);
	// progress bar
	Lcd_Draw_Hline(170, 24, 24+128, WHITE);
	// play control
	Lcd_Draw_BMP(24, 185, arrow2);
	Lcd_Draw_BMP(73, 185, play);
	Lcd_Draw_BMP(121, 185, arrow1);
	// volume control
	Lcd_Draw_BMP(170, 24, volume);
	//206 242 278
	Lcd_Draw_Bar(206, 30, 206+18, 36, WHITE);
	Lcd_Printf(248, 24, WHITE, BLACK, 1, 1, "%d", vol);
	Lcd_Draw_Bar(278, 30, 278+18, 36, WHITE);
	Lcd_Draw_Bar(284, 24, 290, 24+18, WHITE);

	// play list control
	Lcd_Draw_BMP(170, 60, shuffle);
	Lcd_Draw_BMP(270, 60, repeat);
}

void showMusicList(void) {
	int i;
	for (i = 0; i < 2; i++) {
		Lcd_Printf(10, 10+40*i, WHITE, BLACK, 1, 1, "%s", songTitles[i]);
		Lcd_Draw_Hline(40*(i+1), 10, 310, WHITE);
	}
}
