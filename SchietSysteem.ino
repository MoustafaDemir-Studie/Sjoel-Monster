// DJI 420 Lite ESC – 400 Hz PWM
// Pins: D9 (OC1A) & D10 (OC1B)

const int potPin = A0;

const int escMin = 1000; // µs
const int escMax = 2000;

const int deadZone = 50;

void setup() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(potPin, INPUT);

  // -------- TIMER1 SETUP --------
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

  // Start at minimum throttle
  OCR1A = escMin * 2;
  OCR1B = escMin * 2;

  delay(6000); // ESC arming time
}

void loop() {
  // langzaam op naar 1200 µs

  int potWaarde = analogRead(potPin);
  int pulse = map(potWaarde, 0,1023, 1000, 2000); 


  OCR1A = pulse * 2;
  OCR1B = pulse * 2;
  
  /*
    OCR1A = 1200 * 2;
  OCR1B = 1200 * 2;
  delay(3000);

  OCR1A = 1400 * 2;
  OCR1B = 1400 * 2;
  delay(3000);

  OCR1A = 1000 * 2;
  OCR1B = 1000 * 2;
  delay(5000);
  */

}