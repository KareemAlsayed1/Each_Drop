#include <RTClib.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
/*Circute connections with arduino
 * Tmeprature sensor signal to pin 2
 * -----------------------------------
 * LCD CONNECTIONS:
 * D7 ==> PIN 7
 * D6 ==> PIN 6
 * D5 ==> PIN 5
 * D4 ==> PIN 4
 * E ==> PIN 9
 * RW ==> GND
 * RS ==> PIN 8
 * VO ==> VARIBLE RESISTOR 10K
 * VDD ==> 5V.
 * VSS ==> GND
 *------------------------------------- 
 *Bluetooth
 *tx ==> PIN 14
 *rx == PIN 15
 *-------------------------------------
 * BUTTON:
 * CONNECT TO PIN 52
 * ------------------------------------
 * WATER LEVEL SENSOR:
 * SIGNAL ==> A8
 * ------------------------------------
 * MOTOR 1 :
 * CONNECT TO PIN 31
 * -----------------------------------
 * MOTOR 2 :
 * CONNECT TO PIN 41
 * -----------------------------------
 * MOTOR 3 :
 * CONNECT TO PIN 47
 * -----------------------------------
 * MOTOR 4 :
 * CONNECT TO PIN 51
 * ------------------------------------
 * PUMP :
 * CONNECT TO PIN 35
 * -----------------------------------
 * RTC :
 * SDA ==> PIN 20
 * SCL ==> PIN 21
 * -----------------------------------
 * LDR :
 * CONNECT TO PIN A7
 * -----------------------------------
 * SOIL MOIUSTURE SENSOR :
 * CONNECT TO A3
 * ----------------------------------
 * LED 
 * CONNECT TO PIN 42
 * CONNECT TO PIN 40
 * -----------------------------------
 * Buzzer
 * CONNECT TO PIN 30
 * CONNECT TO PIN 32
 */
RTC_DS1307 rtc;
DHT dht (2,DHT11);
LiquidCrystal lcd (8,9,4,5,6,7);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int m1 = 31;
int m2 = 41;
int m3 = 47;
int m4 = 51;
int p1 = 35;
int waterlevel;
int lasthour = 00;
int lastminute = 00;
   byte degree[8] = {
  B11100,
  B10100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
}; 
int led1 = 44;
int led2 = 42;
int buzzer1 = 30;
int buzzer2 = 32;
int state = 1;
int data = 2;
unsigned long time = millis();
SoftwareSerial test (10,11);
void setup() {
  pinMode (m1 , OUTPUT);
  pinMode (m2 , OUTPUT);
  pinMode (m3, OUTPUT);
  pinMode (m4 , OUTPUT);
  pinMode (p1 , OUTPUT);
  pinMode (52,INPUT);
  pinMode (led1 , OUTPUT);
  pinMode (led2 , OUTPUT);
  pinMode (buzzer1 , OUTPUT);
  pinMode (buzzer2 , OUTPUT);
  test.begin (9200);
  stopall ();
  lcd.begin (16,2);
  lcd.clear();
  lcd.createChar(0,degree);
  Serial.begin (9200);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite (led1 , LOW);
  digitalWrite (led2 , LOW);
  digitalWrite (buzzer1 , LOW);
  digitalWrite (buzzer2 , LOW);
  DateTime  now = rtc.now();
  int temp = dht.readTemperature();
  double reading ;
  int ldr = analogRead (A7);
  int bstate = digitalRead (52);
  waterlevel  = analogRead (A8);
  Serial.println(ldr);
  reading = analogRead (A3);
  reading = reading / 1024 ;
  reading = reading *100;
  reading =100- reading ;
    lcd.clear();
    lcd.print(now.hour());
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    lcd.setCursor (9,0);
    lcd.print ("T= ");
    lcd.print (temp);
    lcd.write (byte (0));
    lcd.setCursor (3,1);
    lcd.print ("S_M= ");
    lcd.print (reading);
    lcd.print ("%");
     stopall ();
     int h = 20, m = 00, s=00;
     test.print (" |");
test.print (reading);
test.print ("% |");
test.print (temp);
test.print (" C|");
  test.print (ldr);
test.print (" |");
  test.print (lasthour);
  test.print (":");
  test.print (lastminute);
test.print (" | "); 
if(test.available() > 0)  // Send data only when you receive data:
  {
     data = test.read();  } 
 if(millis()-time > 1000)     //Has one second passed?
  {
     time = millis(); 
     data = 2;
  }
     if (bstate == 1 || reading <5.5 && ldr < 80 || h == now.hour ()&& m == now.minute () && s == now.second () && ldr < 80 || data == 1 ){
      irrigate ();
    }
 delay (500);
}
void irrigate (){
  DateTime  now = rtc.now();
  stopall ();
  lcd.clear ();
  lcd.print ("IRRIGATING now!!");
  lcd.setCursor (0,1);
  lcd.print ("M1&2:ON  M3:OFF");
  digitalWrite (m1,HIGH);
  digitalWrite (m2,HIGH);
  for (int r = 0; r < 20 ; r++){
  digitalWrite (led1 , state);
  digitalWrite (led2 , state);
  digitalWrite (buzzer1 , state);
  digitalWrite (buzzer2 , state);
  if (state == 1){
    state =0;
  }else {
    state =1;
  }
  delay (500);
  }
  lcd.clear ();
  lcd.print ("IRRIGATING now!!");
  lcd.setCursor (0,1);
  lcd.print ("M1&2:OFF  M3:ON");
  digitalWrite (m1,LOW);
  digitalWrite (m2,LOW);
  digitalWrite (m3,HIGH);
  digitalWrite (m4,HIGH);
   for (int r = 0; r < 30 ; r++){
  digitalWrite (led1 , state);
  digitalWrite (led2 , state);
  digitalWrite (buzzer1 , state);
  digitalWrite (buzzer2 , state);
  if (state == 1){
    state =0;
  }else {
    state =1;
  }
  delay (500);
  }
   stopall ();
    digitalWrite (led1 , HIGH);
  digitalWrite (led2 , HIGH);
  digitalWrite (buzzer1 , HIGH);
  digitalWrite (buzzer2 , HIGH); 
  lcd.clear ();
  lcd.print ("DONE IRRIGATING");
  for (int c = 0 ; c < 3; c++){
    lcd.print (".");
    delay (1000);}
  stopall ();
  lasthour = now.hour ();
  lastminute = now.minute();
  digitalWrite (led1 , LOW);
  digitalWrite (led2 , LOW);
  digitalWrite (buzzer1 , LOW);
  digitalWrite (buzzer2 , LOW);
  waterlevel  = analogRead (A8);
   lcd.clear ();
   lcd.print ("FILLING THE TANK");
   lcd.setCursor(0,1);
   lcd.print ("Please Wait..");
  do {
   digitalWrite (p1,HIGH); 
   waterlevel  = analogRead (A8);
  }while (waterlevel < 430);
  digitalWrite (p1,LOW); 
  lcd.clear ();
  lcd.print ("Tank : Full");
  delay (1000);
  lcd.clear ();
  stopall ();
  data = 2;
}
void stopall (){
  digitalWrite (m1,LOW);
  digitalWrite (m2,LOW);
  digitalWrite (m3,LOW);
  digitalWrite (m4,LOW);
  digitalWrite (p1,LOW);  
}

