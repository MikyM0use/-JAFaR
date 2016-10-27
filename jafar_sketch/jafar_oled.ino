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
#ifdef USE_OLED

void oled_message(char *str) {
  u8g.setFont(u8g_font_8x13);
  u8g.firstPage();
  do {
    u8g.drawStr( 0, 20, str);
  } while ( u8g.nextPage() );
}

void oled_waitmessage() {
  u8g.setFont(u8g_font_8x13);
  u8g.firstPage();
  do {
    u8g.drawStr( 0, 20, "PLEASE");
    u8g.drawStr( 0, 35, "WAIT...");
  } while ( u8g.nextPage() );
}

void oled_splash() {

  u8g.setFont(u8g_font_8x13);
  u8g.firstPage();
  do {
    u8g.drawStr( 0, 20, "JAFaR Project");
    u8g.drawStr( 0, 35, "by MikyM0use");

    u8g.setFont(u8g_font_6x10);
    sprintf (j_buf, "RSSI MIN %d", rx5808.getRssiMin()); //Rssi min
    u8g.drawStr(0, 50, j_buf);

    sprintf (j_buf, "RSSI MAX %d", rx5808.getRssiMax()); //Rssi max
    u8g.drawStr(0, 60, j_buf);
  } while ( u8g.nextPage() );
  delay(2000);

}

void oled_init(void) { // flip screen, if required
#ifdef FLIP_SCREEN
  u8g.setRot180();
#endif
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }

  oled_splash();
}

#define MENU_ITEMS 8
uint8_t oled_submenu(uint8_t menu_pos, uint8_t band) {
  int i;

  u8g.setFont(u8g_font_6x10);

  u8g.firstPage();
  do {
    for (i = 0; i < MENU_ITEMS; i++) {
      u8g.setDefaultForegroundColor();
      if (i == menu_pos) {

        u8g.drawBox(0, 1 + menu_pos * 8, 127, 7);
        u8g.setDefaultBackgroundColor();
      }

      sprintf (j_buf, "%d     %d", pgm_read_word_near(channelFreqTable + (8 * band) + i), rx5808.getVal(band, i, 100));

      u8g.drawStr( 0, 8 + i * 8, j_buf);
    }
#ifndef STANDALONE
    u8g.setPrintPos(110, 10);
    u8g.print((int)timer);
#endif

  } while ( u8g.nextPage() );
}

void oled_mainmenu(uint8_t menu_pos) {
  int i;
  u8g.setFont(u8g_font_6x10);

#ifdef STANDALONE
  sprintf (j_buf, "last used: %x:%d", pgm_read_byte_near(channelNames + (8 * last_used_band) + last_used_freq_id), last_used_freq); //last used freq
#else //no timer
  sprintf (j_buf, "last used: %x:%d  %d", pgm_read_byte_near(channelNames + (8 * last_used_band) + last_used_freq_id), last_used_freq, (int)timer); //last used freq
#endif

#ifdef USE_SCANNER
  char *menu_strings[MENU_ITEMS] {j_buf, "BAND A", "BAND B", "BAND E", "BAND FATSHARK", "RACEBAND", "SCANNER", "AUTOSCAN"};
#endif

#ifdef USE_48CH
  char *menu_strings[MENU_ITEMS] {j_buf, "BAND A", "BAND B", "BAND E", "BAND FATSHARK", "RACEBAND1", "RACEBAND2", "AUTOSCAN"};
#endif

  u8g.firstPage();
  do {

    for (i = 0; i < MENU_ITEMS; i++) {
      u8g.setDefaultForegroundColor();

      if (i == menu_pos) { //current selection
        u8g.drawBox(0, 1 + menu_pos * 8, 127, 7);
        u8g.setDefaultBackgroundColor();
      }

      u8g.drawStr( 0, 8 + i * 8, menu_strings[i]); //menu item

      u8g.setPrintPos(80, 8 + i * 8); //RSSI value
      if (i > 0 && i < NUM_BANDS+1) //only for the "real" bands (no for scanner, autoscan etc)
        u8g.print(rx5808.getMaxValBand(i - 1, 100));
    }

  } while ( u8g.nextPage() );
}

//only one screen to show all the channels
//funtction taken from the project: github.com/MikyM0use/OLED-scanner
#define FRAME_START_X 0
#define FRAME_START_Y 7
void oled_scanner() {

  uint8_t i;
  u8g.setFont(u8g_font_6x10);

  timer = TIMER_INIT_VALUE;
  while (timer > 0) {

    u8g.firstPage();
    do {
      u8g.drawStr(FRAME_START_X, FRAME_START_Y, "BAND");
      u8g.drawStr(FRAME_START_X + 80, FRAME_START_Y, "FREE CH");

      u8g.drawStr(FRAME_START_X + 15, FRAME_START_Y + 12, "A");
      u8g.drawStr(FRAME_START_X + 15, FRAME_START_Y + 22, "B");
      u8g.drawStr(FRAME_START_X + 15, FRAME_START_Y + 32, "E");
      u8g.drawStr(FRAME_START_X + 15, FRAME_START_Y + 42, "F");
      u8g.drawStr(FRAME_START_X + 15, FRAME_START_Y + 52, "R");

      u8g.drawLine(25, 0, 25, 60); //start
      u8g.drawLine(76, 0, 76, 60); //end

#define START_BIN FRAME_START_X+29

#define BIN_H_LITTLE 8
#define START_BIN_Y 13

      //computation of the free channels
      for (i = 0; i < 5; i++) {
        uint16_t chan = rx5808.getMinPosBand(i);
        sprintf (j_buf, "%x %d", pgm_read_byte_near(channelNames + chan), pgm_read_word_near(channelFreqTable + chan));
        u8g.drawStr(FRAME_START_X + 80, FRAME_START_Y + 10 * i + 12, j_buf);
      }

      for (i = 0; i < 8; i++) {
        uint8_t bin;
        bin = 1 + rx5808.getVal(0, i, BIN_H_LITTLE);
        u8g.drawBox(START_BIN + i * 6, FRAME_START_Y + START_BIN_Y - bin, 2, bin);

        bin = 1 + rx5808.getVal(1, i, BIN_H_LITTLE);
        u8g.drawBox(START_BIN + i * 6, FRAME_START_Y + START_BIN_Y - bin + 10, 2, bin);

        bin = 1 + rx5808.getVal(2, i, BIN_H_LITTLE);
        u8g.drawBox(START_BIN + i * 6, FRAME_START_Y + START_BIN_Y - bin + 20, 2, bin);

        bin = 1 + rx5808.getVal(3, i, BIN_H_LITTLE);
        u8g.drawBox(START_BIN + i * 6, FRAME_START_Y + START_BIN_Y - bin + 30, 2, bin);

        bin = 1 + rx5808.getVal(4, i, BIN_H_LITTLE);
        u8g.drawBox(START_BIN + i * 6, FRAME_START_Y + START_BIN_Y - bin + 40, 2, bin);
      }

    } while ( u8g.nextPage() );
    delay(LOOPTIME);

#ifdef STANDALONE
    menu_pos = readSwitch();
#else
    timer -= (LOOPTIME / 1000.0);
#endif
  }
}

void oled_autoscan(uint8_t reinit) {

  if (reinit) {
    U8GLIB_SSD1306_128X64 u8g(8, A1, A4, 11 , 13); //CLK, MOSI, CS, DC, RESET
    u8g.setRot180();
    u8g.setFont(u8g_font_6x10);
  }

  u8g.firstPage();
  do {
    for (int i = 0; i < MENU_ITEMS; i++) {
      u8g.setDefaultForegroundColor();
      if (i == menu_pos) {

        u8g.drawBox(0, 1 + menu_pos * 8, 127, 7);
        u8g.setDefaultBackgroundColor();
      }

      sprintf (j_buf, "%d   %x   %d ", pgm_read_word_near(channelFreqTable + rx5808.getfrom_top8(i)), pgm_read_byte_near(channelNames + rx5808.getfrom_top8(i)), rx5808.getVal(rx5808.getfrom_top8(i), 100));

#ifndef STANDALONE
      if (i == 0)
        sprintf (j_buf, "%s   %d ", j_buf, (int)timer); //timer on the first line
#endif

      u8g.drawStr( 0, 8 + i * 8, j_buf);
    }


  } while ( u8g.nextPage() );
}
#endif //OLED


