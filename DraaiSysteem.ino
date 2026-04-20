//Stappenmotor richting test met 2 knoppen.

const int knop1 = 2;
const int knop2 = 3;

const int stepPin1 = 8;
const int stepPin2 = 13;

const int directionPin1 = 12;
const int directionPin2 = A1;

const int stappenmotor_snelheid = 1500;

void setup() {
  pinMode(knop1, INPUT_PULLUP);
  pinMode(knop2, INPUT_PULLUP);
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(directionPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);
}

void loop() {
  if (digitalRead(knop1) == LOW && digitalRead(knop2) == HIGH) {
    digitalWrite(directionPin1, HIGH);
    digitalWrite(directionPin2, LOW);

    stappenmotor_draaien();
  }

  if (digitalRead(knop1) == HIGH && digitalRead(knop2) == LOW) {
    digitalWrite(directionPin1, LOW);
    digitalWrite(directionPin2, HIGH);

    stappenmotor_draaien();
  }

}

void stappenmotor_draaien () {
  digitalWrite(stepPin1, HIGH);
  digitalWrite(stepPin2, HIGH);
  delayMicroseconds(stappenmotor_snelheid);

  digitalWrite(stepPin1, LOW);
  digitalWrite(stepPin2, LOW);
  delayMicroseconds(stappenmotor_snelheid);
}