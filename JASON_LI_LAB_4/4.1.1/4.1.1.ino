//set up the pins and ledc parameters
#define LEDC_CHANNEL 0  // use first of 6 channels
#define LEDC_RESOLUTION_BITS 12
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 50
#define LED_PIN 4
#define SWITCH_PIN 5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //if button pressed light on
  if (digitalRead(SWITCH_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("low");
  } 
  //if button not pressed light off
  else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("high");
  }
}
