#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>


// PINS VARIABLES
// 2 - 6: Knoppen voor schietstanden
// 7: Schietknop
// 9, 10: PWM-uitgangen voor ESC's
// A0: Potentiometer voor schietsnelheid
const int inputPins[] = {2, 3, 4, 5, 6, 7, A0};
const int outputPins[] = {9, 10};
const int servoKnop = 11;



// VARIABLES
const int maxSchijven = 15;
int aantalSchijven = maxSchijven;
int huidigeStand = 0;

// SERVO VARIABLE
const int SERVO_HOME = 0;
const int SERVO_FIRE = 180;

// ESC VARIABLES
const int escMin = 1000;
const int escMax = 2000;



Servo ServoModule;
LiquidCrystal_I2C lcd(0x27, 16, 2);


//FUNCTION DECLARATIONS
void SetupESC() {

  TCCR1A = 0;
  TCCR1B = 0;

  // Fast PWM, TOP = ICR1 (mode 14)
  
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // Prescaler = 8 → 2 MHz timer
  TCCR1B |= (1 << CS11);

  // 2 MHz / 400 Hz = 5000 ticks → 2.5 ms
  ICR1 = 5000;

  /*Start at minimum throttle
  OCR1A = escMin * 2;
  OCR1B = escMin * 2;
  */

  
}
void SetupLCD() {
  lcd.init();
  lcd.backlight();
  updateLCD(false);
}
void SetupServo() {
  ServoModule.attach(servoKnop);
  ServoModule.write(SERVO_HOME);
   ServoModule.writeMicroseconds(escMin);
}


void setup() {
  Serial.begin(9600);
  for(int i=0; i<7; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
  }

  for(int i=0; i<2; i++) {
    pinMode(outputPins[i], OUTPUT);
  }
  SetupESC();
  SetupLCD();
  SetupServo();
  


}

void loop() {
  schietSnelheidAanpassen();

  const int* knopPins = inputPins; // Eerste 5 pinnen zijn de schietstand knoppen
  for (int i = 0; i < 5; i++) {
    if (digitalRead(knopPins[i]) == LOW) {
      huidigeStand = i + 1;
      updateLCD(false);
      delay(200); // Knoppen debouncen.
    }
  }

  if (digitalRead(inputPins[6]) == LOW) {  // Pin 7 (the fire button)
      schiet();
  }

  
}

void schiet() {
  if (aantalSchijven > 0) {
    ServoModule.write(SERVO_FIRE);
    delay(500);
    ServoModule.write(SERVO_HOME);
    delay(300);

    aantalSchijven--;

    if (aantalSchijven == 0) {
            resetSysteem();
    }
  }

  updateLCD(false);
  delay(200); // debounce
}

void schietSnelheidAanpassen() {
  int potWaarde = analogRead(inputPins[6]); // A0 is the 7th pin in inputPins array (index 6)
  int pulse = map(potWaarde, 0,1023, 1000, 2000); 

  OCR1A = pulse * 2;
  OCR1B = pulse * 2;
}


void resetSysteem() {
  aantalSchijven = maxSchijven;
  huidigeStand = 0;
  ServoModule.write(SERVO_HOME);
  updateLCD(true);
}

void updateLCD(bool reset) {
  lcd.clear();

  if (reset) {
    lcd.setCursor(0, 1);
    lcd.print("Magazijn leeg, Herladen!");
    Serial.println("Magazijn leeg, reset!");

  }
  
  lcd.setCursor(0, 0);
  lcd.print("Schietstand: ");
  lcd.print(huidigeStand);

  lcd.setCursor(0, 1);
  lcd.print("Schijven: ");
  lcd.print(aantalSchijven);
}
