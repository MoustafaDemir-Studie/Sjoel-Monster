#include <ServoTimer2.h>

const int buttonPin = 7;

ServoTimer2 myServo;

int defaultValue = 1500
void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);

  // servo op pin 3
  myServo.attach(11);
  myServo.write(defaultValue); //initialisatie servo.

  // ESC pins
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Timer1 setup voor ESC
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  TCCR1B |= (1 << CS11);

  ICR1 = 5000;

  // ESC arm
  OCR1A = 2000;
  OCR1B = 2000;

  delay(4000);

  // constant speed
  OCR1A = 2300;
  OCR1B = 2300;
}

void loop()
{
  if(digitalRead(buttonPin) == LOW)
  {
    myServo.write(850); // 0°
  }
  else
  {
    myServo.write(defaultValue); // 65°
  }
}