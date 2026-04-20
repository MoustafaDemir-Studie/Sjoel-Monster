//Stappenmotor richting met flywheels, LCD-scherm en servo-herlader

# include <ServoTimer2.h>
# include <LiquidCrystal_I2C.h>
# include <Wire.h>

const int knop1 = 2;
const int knop2 = 3;
const int shPin = 7;

const int stepPin1 = 8;
const int stepPin2 = 13;

const int directionPin1 = 12;
const int directionPin2 = A1;

const int potPin = A0;

const int stappenmotor_snelheid = 1500;

int max_schijven = 15;
int aantal_schijven = 15;

LiquidCrystal_I2C lcd(0x27, 16, 2);
ServoTimer2 myServo;

//  voor edge detectie
bool vorigeStatus = HIGH;

void setup()
{
  pinMode(knop1, INPUT_PULLUP);
  pinMode(knop2, INPUT_PULLUP);
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(directionPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);

  pinMode(shPin, INPUT_PULLUP);
  pinMode(potPin, INPUT);

  myServo.attach(11);
  myServo.write(1500);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Timer1 setup (ESC)
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  TCCR1B |= (1 << CS11);

  ICR1 = 5000;

  OCR1A = 2000;
  OCR1B = 2000;

  lcd.init();
  lcd.backlight();
  lcd.clear();

  update_LCD();

  delay(4000);
}

void loop()
{
  int potWaarde = analogRead(potPin);
  int snelheid = map(potWaarde, 0, 1023, 1150, 1220);

  OCR1A = snelheid * 2;
  OCR1B = snelheid * 2;

  // richting stappenmotor
  if (digitalRead(knop2) == LOW && digitalRead(knop1) == HIGH) {
    digitalWrite(directionPin1, HIGH);
    digitalWrite(directionPin2, LOW);
    stappenmotor_draaien();
  }

  if (digitalRead(knop2) == HIGH && digitalRead(knop1) == LOW) {
    digitalWrite(directionPin1, LOW);
    digitalWrite(directionPin2, HIGH);
    stappenmotor_draaien();
  }

  //  edge detectie voor schijven tellen
  bool huidigeStatus = digitalRead(shPin);

  if (huidigeStatus == LOW && vorigeStatus == HIGH)
  {
    myServo.write(750);

    if (aantal_schijven > 0) {
      aantal_schijven--;
    } else {
      aantal_schijven = 15;
    }
  }

  if (huidigeStatus == HIGH) {
    myServo.write(1500);
  }

  vorigeStatus = huidigeStatus;

  update_LCD();
}

void stappenmotor_draaien () {
  digitalWrite(stepPin1, HIGH);
  digitalWrite(stepPin2, HIGH);
  delayMicroseconds(stappenmotor_snelheid);

  digitalWrite(stepPin1, LOW);
  digitalWrite(stepPin2, LOW);
  delayMicroseconds(stappenmotor_snelheid);
}

void update_LCD() {
  lcd.setCursor(0, 0);
  lcd.print("SJOELMONSTER V1");

  lcd.setCursor(0, 1);
  lcd.print("                "); // lijn wissen

  lcd.setCursor(0, 1);
  lcd.print("SCHIJVEN: ");

  if (aantal_schijven < 10) {
    lcd.print(" "); // mooie uitlijning
  }

  lcd.print(aantal_schijven);
}
