#include <Servo.h> // Servo motor library
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LCD i2c library
#include <ds3231.h>   // DS3231 RTC library
LiquidCrystal_I2C lcd(0x27, 16, 2); //0x27 is the LCD Address
struct ts t;


Servo myservo1, myservo2; // definding servo motor
int LDR1 = A0, LDR2 = A1, LDR3 = A2, LDR4 = A3;
int rRDL1 = 0, rRDL2 = 0, rRDL3 = 0, rRDL4 = 0;
int RDL1 = 0, RDL2 = 0, RDL3 = 0, RDL4 = 0;
int max1 = 0, max2 = 0, max3 = 0;
int ser1 = 80, ser2 = 30;
int Volt = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.backlight();      // LCD backlight ON
  lcd.init();
  DS3231_init(DS3231_CONTROL_INTCN);

  myservo1.attach(9);   //up down servo
  myservo2.attach(10); //left right servo

  // servo motor initial position rotate
  myservo1.write(ser1);
  myservo2.write(ser2);

  // RTC time set
  //   t.hour=20;
  //  t.min=34;
  //  t.sec=0;
  //  t.mday=23;
  //  t.mon=12;
  //  t.year=2021;
  //
  //  DS3231_set(t);
  lcd.clear();  // LCD display clear
}

void loop() {
  solar();
  PowerTime();
}

void PowerTime() // Power calculation
{

  int offset = 100;
  Volt = analogRead(A7);
  double Vo = map(Volt, 0, 1024, 0, 2500) - offset;
  Vo /= 100;

  float current = 0;
  for (int i = 0; i < 1000; i++)
  {
    current =  current + (.0264 * analogRead(A6) - 13.51) / 600;
    delay(1);
  }

  float watt = 0;
  watt = -(current * Vo);

  if (watt > 0.04) {

    lcd.setCursor(0, 1);
    lcd.print("POWER : ");
    lcd.print(watt);
    lcd.print("W");

  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("POWER : 0.00W");
  }


  Serial.println(String(Vo) + ", " + String(current) + ", " + String(watt));
  delay(1000);

  //Displaying time
  DS3231_get(&t);
  lcd.setCursor(0, 0);
  lcd.print("TIME  : ");
  lcd.print(t.hour);
  lcd.print(":");
  lcd.print(t.min);
  lcd.print(":");
  lcd.print(t.sec);


}

void solar() {  // Moter rotation

  RDL1 = analogRead(LDR1); //Down LDR
  RDL2 = analogRead(LDR2); //UP LDR
  RDL3 = analogRead(LDR3); //Left LDR
  RDL4 = analogRead(LDR4); //Right LDR

  // ALL LDR subtracting 100
  rRDL1 = RDL1 - 100;
  rRDL2 = RDL2 - 100;
  rRDL3 = RDL3 - 100;
  rRDL4 = RDL4 - 100;

  // maximum LDR selection
  max1 = max(rRDL1, rRDL2);
  max2 = max(rRDL3, rRDL4);
  max3 = max(max1, max2);

  //Serial.println(max1);
  //  Serial.println(String(rRDL1) + ", " + String(rRDL2) + ", " + String(rRDL3) + ", " + String(rRDL4));
  //  delay(1000);

  if (rRDL2 < max3) //Upward rotation
  {
    if (max3 - rRDL2 > 15) {
      if (ser1 < 140)
        ser1 += 1;
      myservo1.write(ser1);
    }
  }
  if (rRDL1 < max3) //Downward rotation
  {
    if (max3 - rRDL1 > 15) {
      if (ser1 > 10)
        ser1 -= 1;
      myservo1.write(ser1);
    }
  }

  if (rRDL4 < max3) //Leftward rotation
  {
    if (max3 - rRDL4 > 15) {
      //Serial.println("servo2 +" + String(ser2));
      if (ser2 < 150)
        ser2 += 1;
      myservo2.write(ser2);
    }
  }
  if (rRDL3 < max3)//Rightward rotation
  {
    if (max3 - rRDL3 > 15) {
      //Serial.println("servo2 -" + String(ser2));
      if (ser2 > 0)
        ser2 -= 1;
      myservo2.write(ser2);

    }
  }
  delay(100);

}