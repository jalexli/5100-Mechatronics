//code adapted from class notes and webLED
//used Chatgpt for debugging and syntax fixing
//set up the pins and ledc parameters
#define LEDC_CHANNEL 0  // use first of 6 channels
#define LEDC_RESOLUTION_BITS 12
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 20
#define LED_PIN 4
#define POT_PIN 1

//maps values to something that can be used in ledcWrite
void ledcAnalogWrite(uint8_t pin, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(LED_PIN, duty); // write duty to LEDC
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

  //set up LED
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
}

void loop() {
  // put your main code here, to run repeatedly:
  int mv;
  mv = map(analogRead(POT_PIN),0,4095,0,255);
  Serial.println(mv);
  ledcAnalogWrite(LED_PIN, mv);
}

