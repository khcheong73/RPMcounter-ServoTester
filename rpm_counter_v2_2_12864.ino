#include <Arduino.h>
#include <U8g2lib.h>
#include <Servo.h>

// Changes in v 2.1
// 1. Remove mode selection > Show Angle & RPM in the same screen
// 2. Change font 6x12 to 8x13

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 8, /* data=*/ 7, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C

//------code for RPM----------------
const int pinEncoder = 2; // The pin the encoder is connected

unsigned int rpm; // rpm reading
unsigned int rpmPrev; // previous rpm reading
volatile byte pulses; // number of pulses
unsigned long timePrev; // The number of pulses per revolution. depends on your index disc!!
unsigned int pulsesPerTurn = 1;

// added by Kevin
Servo myServo;
//const int setModePIN = 4;
const int pinSetBlade = 3;
const int pinSetServo = A6;
const int pinVoltageServo = A3;
const int pinVoltageIN=A2;
const int pinPWMout=9;
const int numBladeMax=4;
const int defaultDelay=25;

int numBlade = 2;
//float adcSetServo;
//float servoAngle;
int servoAngleINT;
int servoAngleOLD;

// define display line
const int Line1=29;
const int Line2=46;
const int Line3=63;

void attachInterrupts() {
    attachInterrupt(digitalPinToInterrupt(pinEncoder), counter, FALLING);
    attachInterrupt(digitalPinToInterrupt(pinSetBlade), count_blade, FALLING);
}

void detachInterrupts() {
    detachInterrupt(digitalPinToInterrupt(pinEncoder));
    detachInterrupt(digitalPinToInterrupt(pinSetBlade));
}

void counter()
{
    pulses++;
}

//-----------------------------------
void setup()   {
  Serial.begin(9600);

  myServo.attach(pinPWMout);
  pinMode(pinSetBlade, INPUT_PULLUP);
  pinMode(pinEncoder, INPUT);
  attachInterrupts();

  pulses = 0;
  rpm = 0;
  timePrev = 0;

  u8g2.begin();
  drawTitle();
  u8g2.setFont(u8g2_font_profont17_tf); // choose a suitable font
  u8g2.drawStr(0,Line2,"Initializing..");
  u8g2.sendBuffer();          // transfer internal memory to the display
}

void displayVoltage() {
  float voltageServo;
  float voltageIN;

// calculate Voltage for Servo
  int R1 = 2000.0;
  int R2 = 1000.0;
  float refVolt = 5.0;
  float adcVoltageServo = analogRead(pinVoltageServo) * refVolt / 1023.0;
  voltageServo=((R1+R2)/R2)*adcVoltageServo;

// calculate Input Voltage from external power source
  int R1i=3000.0;
  int R2i=1000.0;
  float adcVoltageIN = analogRead(pinVoltageIN) * refVolt / 1023.0;
  voltageIN=((R1i+R2i)/R2i)*adcVoltageIN;

// For monitoring 
  Serial.print(" A3:");    Serial.print(analogRead(pinVoltageServo)); Serial.print(",");
  Serial.print("volSRV:"); Serial.print(voltageServo);                Serial.print("V,");
  Serial.print(" A2:");    Serial.print(analogRead(pinVoltageIN));    Serial.print(",");
  Serial.print("volIN:");  Serial.print(voltageIN);                   Serial.print("V,");

// Show voltage information on the display
  u8g2.drawStr(0, Line1, "Voltage");
/*  char voltageINStr[5];
  dtostrf(voltageIN, 3, 1, voltageINStr);  // 숫자를 문자열로 변환
  strcat(voltageINStr, "V");
  u8g2.setCursor(50, Line1); u8g2.print(voltageINStr);
*/
  char voltageServoStr[5];
  dtostrf(voltageServo, 3, 1, voltageServoStr);  // 숫자를 문자열로 변환
  strcat(voltageServoStr, "V");
  u8g2.setCursor(128 - u8g2.getStrWidth(voltageServoStr), Line1);
  u8g2.print(voltageServoStr);
}

void displayServoInfo() {
  float adcSetServo;
  float servoAngle;
  //int servoAngleINT;
  //int servoAngleOLD;
  int xCoordinate;

// Convert potentiometer input to servo angle
  adcSetServo=analogRead(pinSetServo);
  servoAngle=adcSetServo*180.0/1023.0;
  servoAngleINT=180-static_cast<int>(round(servoAngle));

  Serial.print(" A6:");    Serial.print(adcSetServo);   Serial.print(",");
  Serial.print("sAngle:"); Serial.print(servoAngleINT); Serial.print("deg,");

// Convert servoAngleINT to a char array
  char servoAngleChar[4];  // Assuming servoAngleINT is a maximum of 3 digits
//  sprintf(servoAngleChar, "%d", servoAngleINT);
// Calculate the x-coordinate for right alignment
//  int xCoordinate = 119 - u8g2.getStrWidth(servoAngleChar);

// Show servo information on the display
  u8g2.drawStr(0,Line2,"S_angle");
  u8g2.drawGlyph(119,Line2,176);

// Send angle value to servo
  if ( servoAngleINT > servoAngleOLD ) {
    for (int i=servoAngleOLD; i<=servoAngleINT; i++) {
      myServo.write(i);
      delay(defaultDelay);
      sprintf(servoAngleChar, "%d", i);
// Calculate the x-coordinate for right alignment
      xCoordinate = 119 - u8g2.getStrWidth(servoAngleChar);
      u8g2.setCursor(xCoordinate, Line2); 
      u8g2.print(servoAngleChar);
    }    
  }
  else {
    for (int i=servoAngleOLD; i>=servoAngleINT; i--) {
      myServo.write(i);
      delay(defaultDelay);
      sprintf(servoAngleChar, "%d", i);
// Calculate the x-coordinate for right alignment
      xCoordinate = 119 - u8g2.getStrWidth(servoAngleChar);
      u8g2.setCursor(xCoordinate, Line2); 
      u8g2.print(servoAngleChar);
    }
  }
  servoAngleOLD=servoAngleINT;
}

void calcRPM() {
  detachInterrupts();
  rpm = (30 * 2000 / pulsesPerTurn / numBlade)/ (millis() - timePrev)* pulses;
  timePrev = millis();
  pulses = 0;
  attachInterrupts();
}

void displayRPMInfo() {
  // Show RPM information
  u8g2.drawStr(0,Line3,"RPM[# ]");
  u8g2.setCursor(u8g2.getStrWidth("RPM[#")+1,Line3); u8g2.print(numBlade);
  int xPos = 128;
  int textWidth;

  if (millis() - timePrev >= 2000){
    calcRPM();
    textWidth=u8g2.getStrWidth(String(rpm).c_str());
    u8g2.setCursor(xPos - textWidth,Line3); u8g2.print(rpm);
    rpmPrev=rpm;
  }
  else {
    textWidth=u8g2.getStrWidth(String(rpm).c_str());
    u8g2.setCursor(xPos - textWidth,Line3);
    u8g2.print(rpmPrev);
  }

  Serial.print(" RPM:");    Serial.print(rpm);
  Serial.print(" #Blade:"); Serial.print(numBlade);
}

void drawTitle() {
  u8g2.setDrawColor(1);
  u8g2.drawBox(0,0,128,12);
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_pixzillav1_tf);
  u8g2.drawStr(0,11, "Servo/RPM tester");
  u8g2.setDrawColor(1);
  //u8g2.sendBuffer();
}

void loop()
{
  u8g2.clearBuffer();          // clear the internal memory

  // Draw Title
  drawTitle();
  u8g2.setFont(u8g2_font_profont17_tf); // choose a suitable font
  displayVoltage();
  displayServoInfo();
  displayRPMInfo();

  Serial.println("");

  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(defaultDelay);
}

void count_blade()
{
  numBlade++;
  if (numBlade > numBladeMax) {
    numBlade = 1;
  }
}
