#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>

// ============================================================================
// PIN CONFIGURATION
// ============================================================================
// Pins 2-6: Position selection buttons
// Pin 7: Fire button
// Pins 9-10: PWM outputs for Electronic Speed Controllers (ESC)
// Pin A0: Potentiometer for adjustable firing speed

#define POTENTIONMETER_PIN A0

const int INPUT_PINS[] = {2, 3, 4, 5, 6, 7};
const int OUTPUT_PINS[] = {9, 10};


const int POSITION_BUTTONS[] = {2, 3, 4, 5, 6};
const int FIRE_BUTTON_PIN = 7;
const int ESC_PINS[] = {9, 10};
const int SERVO_PIN = 11;


// ============================================================================
// SYSTEM CONSTANTS
// ============================================================================
const int MAX_SCHIJVEN = 15;
const int SERVO_HOME_ANGLE = 0;
const int SERVO_FIRE_ANGLE = 180;
const int ESC_MIN_PULSE_US = 1000;  // Minimum ESC pulse width in microseconds
const int ESC_MAX_PULSE_US = 2000;  // Maximum ESC pulse width in microseconds
const int PWM_FREQUENCY_HZ = 400;
const int TIMER1_PRESCALER = 8;
const int TIMER1_TOP_VALUE = 5000;  // 2MHz / 400Hz
const int SERVO_FIRE_DELAY_MS = 500;
const int SERVO_RETURN_DELAY_MS = 300;
const int LCD_I2C_ADDRESS = 0x27;
const int LCD_COLUMNS = 16;
const int LCD_ROWS = 2;
const int BUTTON_DEBOUNCE_DELAY_MS = 200;
const int FIRE_DEBOUNCE_DELAY_MS = 50;

// ============================================================================
// SYSTEM STATE VARIABLES
// ============================================================================
int SCHIJVEN = MAX_SCHIJVEN;
int currentPosition = 0;


// ============================================================================
// HARDWARE OBJECTS
// ============================================================================
Servo firingServo;
LiquidCrystal_I2C displayLCD(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

/**
 * Initialize 16-bit Timer1 for ESC PWM control (400 Hz)
 * Configures Fast PWM mode with TOP = ICR1 (mode 14)
 * Timer frequency: 2 MHz (prescaler 8)
 */
void initializeESCTimer() {
  TCCR1A = 0;
  TCCR1B = 0;

  // Configure output compare pins (9 and 10)
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
  
  // Set Fast PWM mode (mode 14: WGM = 1110)
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // Set prescaler to 8 (2 MHz timer frequency)
  TCCR1B |= (1 << CS11);

  // Set PWM frequency: 2 MHz / 5000 = 400 Hz
  ICR1 = TIMER1_TOP_VALUE;

  OCR1A = ESC_MIN_PULSE_US * 2;
  OCR1B = ESC_MIN_PULSE_US * 2;
}


void initializeLCD() {
  displayLCD.init();
  displayLCD.backlight();
  UPDATE_DISPLAY(false);
}


void initializeServo() {
  firingServo.attach(SERVO_PIN);
  firingServo.write(SERVO_HOME_ANGLE);
  firingServo.writeMicroseconds(ESC_MIN_PULSE_US);
}


void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 6; i++) {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
  }
  pinMode(POTENTIONMETER_PIN, INPUT);

  for (int i = 0; i < 2; i++) {
    pinMode(OUTPUT_PINS[i], OUTPUT);
  }

  initializeESCTimer();
  initializeLCD();
  initializeServo();
}


void loop() {
  UPDATE_FIRING_SPEED();

  for (int i = 0; i < 5; i++) {
    if (digitalRead(POSITION_BUTTONS[i]) == LOW) {
      currentPosition = i + 1;
      UPDATE_DISPLAY(false);
      delay(BUTTON_DEBOUNCE_DELAY_MS);
    }
  }

  if (digitalRead(FIRE_BUTTON_PIN) == LOW) {
    FIRE_MECHANISM();
  }
}


// ============================================================================
// OPERATIONAL FUNCTIONS
// ============================================================================

void FIRE_MECHANISM() {
  if (SCHIJVEN > 0) {
    firingServo.write(SERVO_FIRE_ANGLE);
    delay(SERVO_FIRE_DELAY_MS);
    
    firingServo.write(SERVO_HOME_ANGLE);
    delay(SERVO_RETURN_DELAY_MS);

    SCHIJVEN--;

    if (SCHIJVEN == 0) {
      resetSystem();
    }
  }

  UPDATE_DISPLAY(false);
  delay(FIRE_DEBOUNCE_DELAY_MS);
}


/**
 * Read potentiometer input and adjust ESC speed
 * Maps analog input (0-1023) to PWM pulse width (1000-2000 µs)
 */
void UPDATE_FIRING_SPEED() {
  int potentiometerValue = analogRead(POTENTIONMETER_PIN);
  int pwmPulseWidth = map(potentiometerValue, 0, 1023, ESC_MIN_PULSE_US, ESC_MAX_PULSE_US);


  OCR1A = pwmPulseWidth * 2;
  OCR1B = pwmPulseWidth * 2;
}


void resetSystem() {
  SCHIJVEN = MAX_SCHIJVEN;
  currentPosition = 0;
  firingServo.write(SERVO_HOME_ANGLE);
  UPDATE_DISPLAY(true);
}


/**
 * Update LCD display with current system state
 * @param isResetState If true, shows "magazine empty" message
 */
void UPDATE_DISPLAY(bool isResetState) {
  displayLCD.clear();

  displayLCD.setCursor(0, 0);
  displayLCD.print("Stand: ");
  displayLCD.print(currentPosition);

  displayLCD.setCursor(0, 1);
  if (isResetState && SCHIJVEN == 0) {
    displayLCD.print("Herladen magazijn!");
    Serial.println("System reset: Magazijn is op.");
  } else {
    displayLCD.print("Schijven: ");
    displayLCD.print(SCHIJVEN);
  }
}
