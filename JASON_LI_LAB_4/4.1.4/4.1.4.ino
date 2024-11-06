//code adapted from class notes and webLED
//used Chatgpt for debugging and syntax fixing
#include <WiFi.h>
#include <html510.h>

//set up the pins and ledc parameters
#define POT_PIN 1
#define LEDC_RESOLUTION_BITS 14
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 20
#define LED_PIN 4
#define MOTOR_PIN1 18
#define MOTOR_PIN2 19

HTML510Server h(80);

//wifi name
const char* ssid = "peepee";

//html code
const char body[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <body>
    <h1>MMMM YUMMY MOTOR Sliders</h1>

    <pre>
      ______________
      /              \
    |                |
    |   ~~~~~~~~     |
    |   [======]     |
    |   [======]     |
    |   ~~~~~~~~     |
      \______________/
    </pre>

    <label for="duty">Duty Cycle (%):</label>
    <input type="range" min="0" max="100" value="50" id="sliderDuty">
    <span id="outputDuty">50</span><br><br>

    <button onclick="sendDirection('forward')">Forward</button>
    <button onclick="sendDirection('backward')">Backward</button>
    <button onclick="sendDirection('off')">Off</button>

    <script>
      var sliderDuty = document.getElementById("sliderDuty");

      // Update the displayed value and send the duty value to the server
      sliderDuty.oninput = function() {
        document.getElementById("outputDuty").innerText = this.value;
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "sliderDuty?val=" + this.value, true);
        xhttp.send();
      };

      // Function to send motor direction commands to the server
      function sendDirection(direction) {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "controlMotor?dir=" + direction, true);
        xhttp.send();
      }
    </script>
  </body>
</html>
)=====";

void ledcAnalogWrite(uint8_t pin, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(LED_PIN, duty); // write duty to LEDC
}

//adapted from chatgpt prompt: how do i adapt a function for LED for motors
//chaned the digital write functions, changed variable names
//added off else case
void handleMotorControl() {
  String direction = h.getText(); // Get the direction parameter from the URL

  if (direction == "forward") {
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
  } else if (direction == "backward") {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
  } else if (direction == "off") {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
    ledcWrite(LED_PIN, 0); // Stop the motor
  }
  //this was recommended by chatgpt for debugging
  h.sendplain("Direction: " + direction);
}

void handleRoot() {
  h.sendhtml(body); // Send the main HTML page
}

//grab slider values and put them into ledanologwrite
void handleSlider() {
  int val = h.getVal(); // Get the value from the slider query parameter

  // Constrain the value to ensure it is between 0 and 255
  //not sure if needed but keeping in just in cas
  val = constrain(val, 0, 100);

  //https://github.com/espressif/arduino-esp32/issues/5089
  //from above, and Matts help, it seems like ledcwrite has a max value of 2 to
  //the power of the resolution, so around 16000
  //the below maps 0 to 100 to 0 to max value (aka resolution)
  ledcAnalogWrite(LED_PIN, val, 100);

  // Prepare a response string
  String s = String(val);

  // Send a plain text response to the client
  h.sendplain(s);
}
                          
void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, ""); // Set up the ESP32 as an access point
  Serial.println("AP SSID: " + String(ssid));
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start the server and attach handlers
  h.begin();
  h.attachHandler("/", handleRoot);
  h.attachHandler("/sliderDuty?val=", handleSlider);
  h.attachHandler("/controlMotor?dir=", handleMotorControl);

  
  pinMode(MOTOR_PIN1,OUTPUT);
  pinMode(MOTOR_PIN2,OUTPUT);
  
  pinMode(LED_PIN, OUTPUT);
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
}

void loop() {
  h.serve();
  delay(10);
}
