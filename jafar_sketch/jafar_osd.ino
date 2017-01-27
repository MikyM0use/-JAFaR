/*
   This file is part of Fatshark© goggle rx module project (JAFaR).

   JAFaR is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   JAFaR is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

   Copyright © 2016 Michele Martinelli
 */

#include <avr/pgmspace.h>

extern char STANDALONE;
extern char USE_SCANNER;
extern char USE_48CH;

void osd_init(void) {
	//tv init
	TV.begin(PAL, D_COL, D_ROW);

	//splash screen
	//TV.clear_screen();
	TV.select_font(font6x8);
	TV.printPGM(0, 0, PSTR("JAFaR Project 2.0"));
	TV.printPGM(5,10, PSTR("by MikyM0use"));
	TV.printPGM(0, 50, PSTR("RSSI MIN"));
	TV.printPGM(0, 60, PSTR("RSSI MAX"));
	TV.print(60, 50, rx5808.getRssiMin(), DEC); //RSSI
	TV.print(60, 60, rx5808.getRssiMax(), DEC); //RSSI

	//progress bar
	int i = 0;
	TV.draw_rect(10, 80, D_COL-40, 8,  WHITE); //draw frame
	for (i = 0; i < D_COL-40; i++) {
		TV.draw_rect(10, 80, i, 8, WHITE, WHITE);
		TV.delay(35); //2s
	}
}

void osd_submenu(int8_t menu_pos, uint8_t band) {
	int i;
	TV.clear_screen();
	TV.draw_rect(1, 1, 100, 94,  WHITE); //draw frame

	//show the channels list and the % RSSI
	for (i = 0; i < 8; i++) {
		TV.print(10, 3 + i * MENU_Y_SIZE, pgm_read_word_near(channelFreqTable + (8 * band) + i), DEC); //channel name

		TV.print(60, 3 + i * MENU_Y_SIZE, rx5808.getVal(band, i, 100), DEC); //RSSI
		TV.printPGM(78, 3 + i * MENU_Y_SIZE, PSTR("%")); //percentage symbol
	}

	TV.draw_rect(9, 2 + menu_pos * MENU_Y_SIZE, 85, 7,  WHITE, INVERT); //current selection

	if (!STANDALONE) {
		//timer
		//TV.println(92, 3, (int)timer, DEC);
		osd_display_timer();
	}

}

void osd_mainmenu(uint8_t menu_pos) {
	int i;
	TV.clear_screen();
	TV.select_font(font6x8);
	TV.draw_rect(1, 1, 100, 94,  WHITE);

	//last used band,freq
	TV.printPGM(10, 3 + compute_position(LAST_USED_POS) * MENU_Y_SIZE, PSTR("LAST:"));
	TV.print(45, 3 + compute_position(LAST_USED_POS) * MENU_Y_SIZE, pgm_read_byte_near(channelNames + (8 * last_used_band) + last_used_freq_id), HEX);
	TV.print(60, 3 + compute_position(LAST_USED_POS) * MENU_Y_SIZE, last_used_freq, DEC);

	//entire menu
	TV.printPGM(10, 3 + compute_position(BAND_A_POS) * MENU_Y_SIZE, PSTR("BAND A"));
	TV.printPGM(10, 3 + compute_position(BAND_B_POS) * MENU_Y_SIZE, PSTR("BAND B"));
	TV.printPGM(10, 3 + compute_position(BAND_E_POS) * MENU_Y_SIZE, PSTR("BAND E"));
	TV.printPGM(10, 3 + compute_position(BAND_F_POS) * MENU_Y_SIZE, PSTR("FATSHARK"));
	TV.printPGM(10, 3 + compute_position(BAND_R1_POS) * MENU_Y_SIZE, PSTR("RACEBAND"));
	if (USE_SCANNER)
		TV.printPGM(10, 3 + compute_position(SCANNER_POS) * MENU_Y_SIZE, PSTR("SCANNER"));
	if (USE_48CH)
		TV.printPGM(10, 3 + compute_position(BAND_R2_POS) * MENU_Y_SIZE, PSTR("RACE2"));
	TV.printPGM(10, 3 + compute_position(AUTOSCAN_POS) * MENU_Y_SIZE, PSTR("AUTOSCAN"));

	for (i = 0; i < NUM_BANDS; i++) {
		TV.println(65, 3 + ((_init_selection + 1 + i) % 8) * MENU_Y_SIZE, rx5808.getMaxValBand(i, 100), DEC); //RSSI
		TV.printPGM(85, 3 + ((_init_selection + 1 + i) % 8) * MENU_Y_SIZE, PSTR("%")); //% symbol
	}

	TV.draw_rect(9, 2 + menu_pos * MENU_Y_SIZE, 85, 7,  WHITE, INVERT); //current selection

	if (!STANDALONE) {
		//header and countdown
		//TV.println(92, 3, (int)timer, DEC);
		osd_display_timer();
	}
}

void osd_scanner() {
	uint8_t s_timer = 5;
	while (s_timer-- > 0) {
		rx5808.scan();
		TV.clear_screen();
		TV.draw_rect(1, 1, 100, 94,  WHITE);
		TV.select_font(font4x6);
		TV.printPGM(5, 87, PSTR("5645"));
		TV.printPGM(45, 87, PSTR("5800"));
		TV.printPGM(85, 87, PSTR("5945"));
		TV.select_font(font6x8);
		for (int i = CHANNEL_MIN; i < CHANNEL_MAX; i++) {
			uint8_t channelIndex = pgm_read_byte_near(channelList + i); //retrive the value based on the freq order
			uint16_t rssi_norm = constrain(rx5808.getRssi(channelIndex), rx5808.getRssiMin(), rx5808.getRssiMax());
			rssi_norm = map(rssi_norm, rx5808.getRssiMin(), rx5808.getRssiMax(), 0, 70);

#define START_OSD_FRAME_Y 80
			TV.draw_rect(10 + 2 * i, START_OSD_FRAME_Y - rssi_norm , 2, rssi_norm, WHITE, WHITE);
		}

		TV.println(92, 3, (int)s_timer, DEC);
		TV.delay(100);
	}
	s_timer = 9;
}

void osd_autoscan() {
	TV.clear_screen();
	TV.draw_rect(1, 1, 100, 94,  WHITE);

	if (!STANDALONE) {
		//header and countdown
		//TV.println(92, 3, (int)timer, DEC); //timer
		osd_display_timer();
	}

	for (uint8_t i = 0; i < 8; i++) {
		TV.print(10, 3 + i * MENU_Y_SIZE, pgm_read_word_near(channelFreqTable + rx5808.getfrom_top8(i)), DEC); //channel freq
		TV.print(45, 3 + i * MENU_Y_SIZE , pgm_read_byte_near(channelNames + rx5808.getfrom_top8(i)), HEX); //channel name
		TV.print(65, 3 + i * MENU_Y_SIZE, rx5808.getVal(rx5808.getfrom_top8(i), 100), DEC); //RSSI
		TV.printPGM(85, 3 + i * MENU_Y_SIZE, PSTR("%"));
	}

	TV.draw_rect(9, 2 + menu_pos * MENU_Y_SIZE, 85, 7,  WHITE, INVERT); //current selection
}

void osd_display_timer() {
	if (timer > 0) {
		int fh = D_ROW ;
		int height = ((long)(timer) * 100) * fh / ((int)TIMER_INIT_VALUE * 100);
		if (height > 0)
			TV.draw_rect(D_COL - 24, fh - height + 1, 4, height, WHITE, WHITE);
	}
}

