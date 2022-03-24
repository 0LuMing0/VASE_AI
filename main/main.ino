#include <U8glib.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"

#define ADDR 0b0100011

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 
ClosedCube_HDC1080 hdc1080;

// HDC num
void setUpHDC(){
  Serial.begin(9600);

  // Default settings: 
  //  - Heater off
  //  - 14 bit Temperature and Humidity Measurement Resolutions
  hdc1080.begin(0x40);
  uint8_t huTime = 1;
  Serial.print("Heating up for approx. ");
  Serial.print(huTime);
  Serial.println(" seconds. Please wait...");

  hdc1080.heatUp(huTime);
}

float getHeat() {
  return hdc1080.readTemperature();
}

float getHum(){
  return hdc1080.readHumidity();
}

float getBright(){
  // init
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Wire.begin();
  Wire.beginTransmission(ADDR);
  Wire.write(0b00000001);
  Wire.endTransmission();

  // getdata
  int val = 0;
  // reset
  Wire.beginTransmission(ADDR);
  Wire.write(0b00000111);
  Wire.endTransmission();
 
  Wire.beginTransmission(ADDR);
  Wire.write(0b00100000);
  Wire.endTransmission();
  // typical read delay 120ms
  delay(120);
  Wire.requestFrom(ADDR, 2); // 2byte every time
  for (val = 0; Wire.available() >= 1; ) {
    char c = Wire.read();
    //Serial.println(c, HEX);
    val = (val << 8) + (c & 0xFF);
  }
  float output = 1.0 * val / 1.2;
  return output;
}

void draw(void) {
  char bright[8];
  char heat[8];
  char hum[8];
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  dtostrf(getBright(),3,2,bright);
  dtostrf(getHeat(),3,2,heat);
  dtostrf(getHum(),3,2,hum);
  
  u8g.drawStr( 5, 12, "Brig:");
  u8g.drawStr( 45, 12, bright);
  u8g.drawStr( 100, 12, "lx");
  
  u8g.drawStr( 5, 35, "Heat:");
  u8g.drawStr( 45, 35, heat);
  u8g.drawStr( 100, 35, "c");
  
  u8g.drawStr( 5, 58, "Humi:");
  u8g.drawStr( 45, 58, hum);
  u8g.drawStr( 100, 58, "%");
}

// main
void setup() {
  setUpHDC();
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  // u8g.setHardwareBackup(u8g_backup_avr_spi);

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
    u8g.setHiColorByRGB(255,255,255);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(10000);

}
