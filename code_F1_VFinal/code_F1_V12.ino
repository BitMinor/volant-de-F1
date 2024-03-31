//engine on / off
int engine_on = false;

//clock
#include <DS3231.h>

//boutons rapports
#include "SevSeg.h" 
SevSeg sevseg; //Instantiate a seven segment object
int boutonPin = 10;
int boutonPin_moins = 11;
int compteur = 0;
int valeur = 1;
int boutonState_plus;
int boutonState_moins;
int etatboutonprecedent_plus = HIGH;
int etatboutonprecedent_moins = HIGH;
int BP_enfonce_plus;
int BP_enfonce_moins;

int limit;

int ledT = 13;

//ecran lcd
#include <LiquidCrystal.h>
//pins
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

//température 
int tempPin = 0;
int tempC0;

//leds

int lightPin = 1;
int latchPin = 29;
int clockPin = 30;
int dataPin = 28;

int leds = 0;
//temp 2 + humidité
#include <SimpleDHT.h>
int pinDHT11 = 31;
SimpleDHT11 dht11;
int err = SimpleDHTErrSuccess;

//clock

DS3231  rtc(SDA, SCL);

//encodeur
int pinA = 32;  // Connected to CLK on KY-040
int pinB = 33;  // Connected to DT on KY-040
int encoderPosCount = 0; 
int pinALast;  
int aVal;
boolean bCW;

//8x8

#include <LedControl.h>
int DIN = 36;
int CS =  37;
int CLK = 38;
LedControl lc=LedControl(DIN,CLK,CS,0);

//clavier num.

#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {46, 45, 44, 43}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {42, 41, 40, 39}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


void setup(){
  // Initialisation de l'afficheur
    byte numDigits = 1;
    byte digitPins[] = {};
    byte segmentPins[] = {6, 5, 2, 3, 4, 7, 8, 9};
    bool resistorsOnSegments = true;

    byte hardwareConfig = COMMON_CATHODE; 
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
    sevseg.setBrightness(50);
    sevseg.blank();
   sevseg.setChars("n");
   sevseg.refreshDisplay();

// Initialisation liaison série à 9600baud
   Serial.begin(9600);


  // Configuration BP+ avec résistance pull-up interne
   pinMode(boutonPin, INPUT);
   digitalWrite(boutonPin, HIGH);

// Configuration BP- avec résistance pull-up interne
   pinMode(boutonPin_moins, INPUT);
   digitalWrite(boutonPin_moins, HIGH);

// configuration écran lcd
lcd.begin(16, 2);
lcd.setCursor(5,0);
lcd.print("System");
lcd.setCursor(1,1);
lcd.print("initialization");
delay(2000);
lcd.clear();

//leds
pinMode(latchPin, OUTPUT);
pinMode(dataPin, OUTPUT);  
pinMode(clockPin, OUTPUT);
// clock 
rtc.begin();
//8x8
 lc.shutdown(0,false);       
 lc.setIntensity(0,10);      //Adjust the brightness maximum is 15
 lc.clearDisplay(0);

//encodeur
  pinMode (pinA,INPUT);
  pinMode (pinB,INPUT);
  pinALast = digitalRead(pinA);   

}

void loop(){
  if(engine_on){

 //clavier num.
//clavNum();
  char customKey = customKeypad.getKey();
  int key = customKey;
  if (key){
    Serial.println(customKey);
    if(customKey == 49){//1
      encoderPosCount = 5;

    }
    if(customKey == 50){//2
      encoderPosCount = 15;

    }
    if(customKey == 51){//3
      encoderPosCount = -5;

    }
        if(customKey == 52){//4
      encoderPosCount = -15;  
    }
      if(customKey == 66){//B
        lcd.clear();
      engine_on = false;  
    }
    if(customKey == 67){//C
    compteur = 0;
    sevseg.setChars("n");
    sevseg.refreshDisplay();  
    }
  }

//encodeur
encodeur();
  //bouton +
  boutonState_plus = digitalRead(boutonPin);  
   
  if((boutonState_plus == LOW) && (etatboutonprecedent_plus == HIGH))
    BP_enfonce_plus = true;
  
    else
      BP_enfonce_plus = false;
    
    
 etatboutonprecedent_plus = boutonState_plus;

 // bouton -
  boutonState_moins = digitalRead(boutonPin_moins);  
   
  if((boutonState_moins == LOW) && (etatboutonprecedent_moins == HIGH))
    BP_enfonce_moins = true;
  
    else
      BP_enfonce_moins = false;
    
    
 etatboutonprecedent_moins = boutonState_moins;
 
 //code bouton
   if(BP_enfonce_plus){
      compteur = compteur + valeur;
      if(compteur >8) compteur = 8;
      sevseg.setNumber(compteur);
      if(compteur ==0){
        sevseg.setChars("n");
        sevseg.refreshDisplay();
      }
      sevseg.refreshDisplay();
    }
    else
      sevseg.refreshDisplay();


   if(BP_enfonce_moins){
      compteur = compteur - valeur;
      if(compteur <0) compteur = 0;
      sevseg.setNumber(compteur);
      if(compteur == 0) {
        sevseg.setChars("n");
        sevseg.refreshDisplay();
      }
      sevseg.refreshDisplay();
    }
    else
      sevseg.refreshDisplay();

  //clock____ lcd -1
  if(encoderPosCount>10){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(rtc.getTimeStr());
  lcd.setCursor(0,1);
  lcd.print(rtc.getDateStr());
  }

//leds
int reading  = analogRead(lightPin);
  int numLEDSLit = reading / 57;  //1023 / 9 / 2
  if (numLEDSLit > 8) numLEDSLit = 8;
  leds = 0;   // no LEDs lit to start
  for (int i = 0; i < numLEDSLit; i++)
  {
    leds = leds + (1 << i);  // sets the i'th bit
  }
  updateShiftRegister();


// temp 2 + humidité

  // Lecture de la température et de l'humidité ------------------
  byte temperature = 0;
  byte humidity = 0;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {

    return;
  }

  // Ecran LCD temp. 1 --------------------------------------------------
  if(encoderPosCount<0){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature: "); lcd.print((int)temperature-1); lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("humidity: ");    lcd.print((int)humidity-21);    lcd.print("%");

  }

//lcd page 0
if((encoderPosCount<=10 && encoderPosCount>=0)){
  lcd.clear();
  lcd.setCursor( 0,0);
  lcd.print("<-");
  lcd.setCursor( 4,0);
  lcd.print("STOP = !");
  lcd.setCursor(14,0);
  lcd.print("->");
  lcd.setCursor(0,1);
  lcd.print("ENGINE OFF = B");


}

// lcd page -2
 if(encoderPosCount<-10 ){
  lcd.clear();
  lcd.setCursor(0,0);
  if(numLEDSLit == 1) lcd.print("0");
  else if(numLEDSLit == 2) lcd.print("1000");
  else if(numLEDSLit == 3) lcd.print("2000");
  else if(numLEDSLit == 4) lcd.print("3000");
  else if(numLEDSLit == 5) lcd.print("4000");
  else if(numLEDSLit == 6) lcd.print("5000");
  else if(numLEDSLit == 7) lcd.print("6000");
  else if(numLEDSLit == 8) lcd.print("7000");
  lcd.setCursor(5,0);
  lcd.print("rpm");
  lcd.setCursor(0,1);
  lcd.print("Gear : ");
  lcd.setCursor(7,1);
  if(compteur ==0)lcd.print("neutral");
  else lcd.print(compteur);
  lcd.setCursor(8,1);
  if(numLEDSLit >= 8 && compteur != 8){
    if(compteur ==0){ lcd.setCursor(14,1); lcd.print("+");}
    else lcd.print("+");
  }
  else if(numLEDSLit < 3 && compteur != 0) lcd.print("-");

 }




//8x8
byte attention[8] = {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x18};
byte plus[8] = {0x18,0x18,0x18,0xFF,0xFF,0x18,0x18,0x18};
byte moins[8] {0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00};

if(encoderPosCount>20 or encoderPosCount<-20 ) printByte(attention);

else if(numLEDSLit >= 8 && compteur!=8) printByte(plus);

else if(numLEDSLit < 3 && compteur != 0) printByte(moins);

else lc.clearDisplay(0);


  }//fin ifffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
else{
  lcd.setCursor(3,0);
  lcd.print("ENGINE OFF");
  lcd.setCursor(0,1);
  lcd.print("press A to start");
  char customKey = customKeypad.getKey();
  int key = customKey;
  if (key){
    Serial.println(customKey);
    if(customKey == 65){//A
      engine_on = true;

                       }
         }
    }
}
//fonction pour les leds
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}


//encodeur
void encodeur(){
aVal = digitalRead(pinA);
  if (aVal != pinALast){ // Means the knob is rotating
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(pinB) != aVal) {  // Means pin A Changed first - We're Rotating Clockwise
      encoderPosCount --;
      bCW = true;
    } else {// Otherwise B changed first and we're moving CCW
      bCW = false;
      encoderPosCount++;
    }
    Serial.print ("Rotated: ");
    if (bCW){
      Serial.println ("clockwise");
    }else{
      Serial.println("counterclockwise");
    }
    Serial.print("Encoder Position: ");
    Serial.println(encoderPosCount);
    
  } 
  pinALast = aVal;
}

//8x8
void printByte(byte character [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}
/*
void clavNum(){
  char customKey = customKeypad.getKey();
  int key = customKey;
  if (customKey){
    Serial.println(customKey);
  }
}*/
