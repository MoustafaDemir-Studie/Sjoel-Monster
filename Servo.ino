#include <Servo.h>

#define BUTTON_PIN 4
int rotation = 0;
bool hold = false;
bool debounce = false;
Servo myServo;

void setup() {
  myServo.attach(11);
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);


}

void loop() {

  

  /*myServo.write(50);
  rotation += 20;*/

  byte buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == LOW && hold == false) {
      hold = true;
      onClick();
  } else if (buttonState != LOW && hold == true) {
      hold = false;
      onClick();

  }
  delay(20);
}

void onClick() {
    if (debounce == true | hold != true) {
      return 0;
    }

    debounce = true;
    Serial.println("Button is pressed");
    myServo.write(0);
    delay(500);
    myServo.write(30);

    delay(500);
    
    debounce = false;
}

