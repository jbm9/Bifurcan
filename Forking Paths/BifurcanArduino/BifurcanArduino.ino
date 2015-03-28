/*
  A quick and dirty version of the Bifurcan clock to Arduino, using a
  little 128x64 OLED display.

  Uses the Universal 8bit Graphics Library, http://code.google.com/p/u8glib/
  
*/


#include "U8glib.h"

#include <Wire.h>
#include "RTClib.h"


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 

RTC_DS1307 rtc;





uint16_t DIGITS[10] = {
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 6 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 0 
  (1<<1 | 1<<4 | 1<<7 | 1<<10 | 1<<13) ,   // 1
  (1<<0 | 1<< 1 | 1<<2 | 1 << 5 | 1<<6 | 1<<7 |  1 << 8 | 1 << 9 | 1 << 12 | 1 << 13 | 1 << 14),  // 2
  (1<<0 | 1<< 1 | 1<<2 | 1<<5 | 1 << 6| 1 << 7 | 1 << 8 | 1<<11 | 1 << 12 | 1 << 13 | 1 << 14),  // 3 

  (1<<0 | 1<< 2 | 1<<3 | 1<<5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 11 | 1 << 14 ),  // 4
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 5

  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 6 
  (1<<0 | 1<< 1 | 1<<2 | 1<<5 | 1 << 8 | 1 << 11 | 1 << 14),  // 7
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 8
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8  | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 9
};


class ScreenState {
public:
  uint8_t w,h;
  uint8_t w_logical, h_logical;
  uint8_t pixel_pitch;

  uint8_t padding_w, padding_h;

  uint8_t digits[6];
  uint8_t line_step;



  ScreenState(uint8_t w_, uint8_t h_, uint8_t w_logical_, uint8_t h_logical_, uint8_t pixel_pitch_): 
    w(w_), h(h_), w_logical(w_logical_), h_logical(h_logical_), pixel_pitch(pixel_pitch_)
  {
    padding_w  = (w - w_logical*pixel_pitch)/2;
    padding_h  = (h - h_logical*pixel_pitch)/2;

    line_step = 3;
  }


  void box_right(uint8_t sx, uint8_t sy) {
    return;		
    uint8_t i;

    for (i = 0; i < pixel_pitch; i += line_step) {
      u8g.drawLine(sx, sy+i, sx+pixel_pitch-i-1, sy+pixel_pitch-1);
    }

    for (i = line_step; i < pixel_pitch; i += line_step) {
      u8g.drawLine(sx+i, sy, sx+pixel_pitch-1, sy+pixel_pitch-i-1);
    }

  }

  void box_left(uint8_t sx, uint8_t sy) {
    uint8_t i;

    for (i = 0; i < pixel_pitch; i += line_step) {
      u8g.drawLine(sx, sy+pixel_pitch-i-1, sx+pixel_pitch-i-1, sy);
    }

    for (i = line_step; i < pixel_pitch; i += line_step) {
      u8g.drawLine(sx+i, sy+pixel_pitch-1, sx+pixel_pitch-1, sy+pixel_pitch-i);
    }


  }


  void draw(void) {
    uint16_t i, j, k;
    uint8_t lx,ly;
    uint8_t sx,sy;
    int dx,dy;

    for (i = 0; i < 6; i++) {
      j = DIGITS[ digits[i] ];

      for (k = 0; k < 16; k++) {
	lx = k % 3;
	ly = k / 3;

	sy = h -( padding_h + pixel_pitch*(lx + 4*(i%2) + 1) + pixel_pitch);
	sx = padding_w + pixel_pitch*(ly + 6*(i/2) + 1);

	if (j & (1<<k)) {
	  // u8g.drawBox(sx, sy, pixel_pitch, pixel_pitch);
	  box_left(sx, sy);
	} else {
	  box_right(sx, sy);
	}
      }
    }
    

    for (lx = 0; lx < h_logical; lx += 4) {
      for (ly = 0; ly < w_logical; ly++) {
	sy = h -( padding_h + pixel_pitch*(lx) + pixel_pitch);
	sx = padding_w + pixel_pitch*(ly);
	
	box_right(sx,sy);
      }  
    } 

    for (ly = 0; ly < w_logical; ly += 6) {
      for (lx = 0; lx < h_logical; lx++) {
	sy = h -( padding_h + pixel_pitch*(lx) + pixel_pitch);
	sx = padding_w + pixel_pitch*(ly);
	
	box_right(sx,sy);
      }
    }


    u8g.drawPixel(padding_w, padding_h+h_logical*pixel_pitch-1);
    u8g.drawPixel(padding_w+w_logical*pixel_pitch-1, padding_h);

 }
};




class WallClock {
public:
  uint8_t hh,mm,ss;

  WallClock(uint8_t hh_, uint8_t mm_, uint8_t ss_) : hh(hh_), mm(mm_), ss(ss_) { };

  void tick() {
    ss++;

    tick_fixup();
  }

  void tick_fixup() {

    if (ss >= 60) {
      ss = 0;

      mm++;

      if (mm >= 60) {
	mm = 0;
	hh++;

	if (hh >= 24)
	  hh = 0;
      }
    }
  }
};


ScreenState ss(128, 64, 19, 9, 6);
WallClock wc(19, 59, 0);


uint8_t draw_state = 0;
uint8_t span = 3;

void u8g_prepare(void) {
  u8g.setDefaultForegroundColor();
}


void draw(void) {

  ss.draw();
  /*
int i;
  
  int x0,y0, x1,y1;
  
  for (i = draw_state; i < SCREENHEIGHT; i += span) {
    u8g.drawLine(0, i, SCREENHEIGHT-i, SCREENHEIGHT);
  }

  for (i = draw_state; i < SCREENWIDTH; i += span) {
    u8g.drawLine(i, 0, i+SCREENHEIGHT, SCREENHEIGHT);
    }*/
}

void setup(void) {
  Serial.begin(9600);
  rtc.begin();

  if (!rtc.isrunning()) {
    Serial.println("RTC not running.");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(13, OUTPUT);           
  digitalWrite(13, HIGH);  
}



uint32_t last_millis = millis();
void loop(void) {


  ss.digits[0] = wc.hh / 10;
  ss.digits[1] = wc.hh % 10;

  ss.digits[2] = wc.mm / 10;
  ss.digits[3] = wc.mm % 10;

  ss.digits[4] = wc.ss / 10;
  ss.digits[5] = wc.ss % 10;
  
  u8g_prepare();
  u8g.firstPage();
  do {
    draw();
    digitalWrite(13, !digitalRead(13));
  } while ( u8g.nextPage() );
  u8g_i2c_stop();

  delay(50);

  //  rtc.begin();
  DateTime now = rtc.now();
  now = rtc.now();
  Serial.println(now.unixtime(), 10);

  if (now.hour() < 24) {
    wc.hh = now.hour();
    Serial.print(wc.hh, 10);
    Serial.print(':');
    wc.mm = now.minute();
    Serial.print(wc.mm, 10);
    Serial.print(':');

    wc.ss = now.second();
    Serial.print(wc.ss, 10);
    Serial.println(' ');
  }
  //  delay(100);
}
