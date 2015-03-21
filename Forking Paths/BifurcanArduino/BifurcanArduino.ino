/*
  A quick and dirty version of the Bifurcan clock to Arduino, using a
  little 128x64 OLED display.

  Uses the Universal 8bit Graphics Library, http://code.google.com/p/u8glib/
  
*/


#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


class ScreenState {
public:
  uint8_t w,h;
  uint8_t w_logical, h_logical;
  uint8_t pixel_pitch;

  uint8_t padding_w, padding_h;

  uint8_t *screen_buf;

  ScreenState(uint8_t w_, uint8_t h_, uint8_t w_logical_, uint8_t h_logical_, uint8_t pixel_pitch_): 
    w(w_), h(h_), w_logical(w_logical_), h_logical(h_logical_), pixel_pitch(pixel_pitch_)
  {
    screen_buf = (uint8_t*) malloc(w_logical*h_logical/8 + 1);
    padding_w  = (w - w_logical*pixel_pitch)/2;
    padding_h  = (h - h_logical*pixel_pitch)/2;

    clear_buffer();
  }


  void draw(void) {
    uint16_t i, j, k;
    uint8_t lx,ly;
    uint8_t sx,sy;


    for (i = 0; i < w_logical*h_logical; i++) {
      j = i / 8;

      k = i % 8;
#if 0
      Serial.print("draw ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(j);
      Serial.print(";");
      Serial.print(k);
      Serial.println(" ");
#endif


      if ( (screen_buf[j] & (1<<k)) ) {
	lx = i % w_logical;
	ly = i / w_logical;


	sx = padding_w + pixel_pitch*lx;
	sy = padding_h + pixel_pitch*ly;
#if 0
	Serial.print("draw ");
	Serial.print(i);
	Serial.print(" ");
	Serial.print(lx);
	Serial.print(",");
	Serial.print(ly);
	Serial.print(" ");
	Serial.print(sx);
	Serial.print(" ");
	Serial.print(sy);
	Serial.println(" ");
#endif


	u8g.drawBox(sx, sy, pixel_pitch, pixel_pitch);
      }
    }
  }

  void clear_buffer() {
    memset(screen_buf, 0, (w_logical*h_logical/8 + 1));
  }

  void set_bit(uint8_t x,uint8_t y) {
    int i = (w_logical*y + x) / 8;
    int j = (w_logical*y + x) % 8;

    screen_buf[i] |= (1<<j);
  }

  void clear_bit(uint8_t x, uint8_t y) {
    int i = (w_logical*y + x) / 8;
    int j = (w_logical*y + x) % 8;

    screen_buf[i] &= ~(1<<j);
  }
};



ScreenState ss(128, 64, 21, 10, 6);

uint8_t draw_state = 0;
uint8_t span = 3;

uint8_t DIGITS[10] = {
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 0 
  (1<<1 | 1<<4 | 1<<7) ,   // 1
  (1<<0 | 1<< 1 | 1<<2 | 1 << 5 | 1<<6, 1<<7, 1 << 8 | 1 << 9 || 1 << 12 | 1 << 13 | 1 << 14),  // 2
  (1<<0 | 1<< 1 | 1<<2 | 1<<5 | 1 << 6| 1 << 7 | 1 << 8 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 3 

  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 4
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 5
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 6 
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 7
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 8
  (1<<0 | 1<< 1 | 1<<2 | 1<<3 | 1 << 5 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14),  // 9
};

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
  pinMode(13, OUTPUT);           
  digitalWrite(13, HIGH);  
}

void loop(void) {
  Serial.println("Loop.");
  digitalWrite(13, !digitalRead(13));
  delay(50);
  digitalWrite(13, !digitalRead(13));
  delay(50);
  digitalWrite(13, !digitalRead(13));
  delay(50);
  digitalWrite(13, !digitalRead(13));
  delay(50);

  u8g_prepare();
  u8g.firstPage();
  do {
    draw();
    digitalWrite(13, !digitalRead(13));
  
  } while ( u8g.nextPage() );



  ss.clear_bit(draw_state % 21, draw_state / 21);
  draw_state = (draw_state+1)%210;
  ss.set_bit(draw_state % 21, draw_state / 21);

}
