# include <LiquidCrystal_I2C.h>
# include <Wire.h>
# include <Servo.h>


// PINS VARIABLES
const int knopPins[] = {2, 3, 4, 5, 6};
const int schietKnop = 7;
const int servoKnop = 11;
const int potPin = A0;

// VARIABLES
const int maxSchijven = 15;
int aantalSchijven = maxSchijven;
int huidigeStand = 0;

// SERVO VARIABLE
const int SERVO_HOME = 0;
const int SERVO_FIRE = 180;

const int dodeZone = 50;
const int escMin = 1000;
const int escMax = 2000;

int servoPos = 0;




Servo ServoModule;
LiquidCrystal_I2C lcd(0x27, 16, 2);



void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  updateLCD(false);


  ServoModule.attach(servoKnop);
  ServoModule.write(SERVO_HOME);

  ServoModule.writeMicroseconds(escMin);
  
  for(int i=0; i<5; i++) {
    pinMode(knopPins[i], INPUT_PULLUP);
  }
  pinMode(schietKnop, INPUT_PULLUP);

}

void loop() {
  schietSnelheidAanpassen();
  for (int i = 0; i < 5; i++) {
    if (digitalRead(knopPins[i]) == LOW) {
      huidigeStand = i + 1;
      updateLCD(false);
      delay(200); // Knoppen debouncen.
    }
  }

  if (digitalRead(schietKnop) == LOW) {
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
  int potWaarde = analogRead(potPin);
  int snelheid;
  
  if (potWaarde < dodeZone) {
    snelheid = escMin; 
  }
  else {
    snelheid = map(potWaarde, 0, 1023, escMin, escMax);
  }
  ServoModule.writeMicroseconds(snelheid);
  delay(10);
}


void resetSysteem() {
  aantalSchijven = maxSchijven;
  huidigeStand = 0;
  ServoModule.write(SERVO_HOME);
  updateLCD(true);
}

void updateLCD(bool reset) {
  lcd.clear();

  if (!reset) {
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
