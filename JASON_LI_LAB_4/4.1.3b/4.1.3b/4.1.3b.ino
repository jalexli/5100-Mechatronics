//adapted form webled and class ntoes
//used Chatgpt for debugging and syntax fixing
#include <WiFi.h>
#include <html510.h>

//set up the pins and ledc parameters
#define POT_PIN 1
#define LEDC_RESOLUTION_BITS 14
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 20
#define LED_PIN 4

//from class notes
HTML510Server h(80);

//wifi name
const char* ssid = "peepee";

//html code
//adapted form ChatGPT prompt: how do i make a webpage with 2 sliders
//adapted the sliders to be duty and frequency sliders
//added min and max values
const char body[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <body>
    <h1>MMMM YUMMY LED Sliders</h1>

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
    <span id="outputDuty">50</span><br><br> <!-- Added default value display -->

    <label for="frequency">Frequency (Hz):</label>
    <input type="range" min="3" max="20" value="20" id="sliderFrequency">
    <span id="outputFrequency">20</span><br><br> <!-- Added default value display -->

    <script>
      var sliderDuty = document.getElementById("sliderDuty");
      var sliderFrequency = document.getElementById("sliderFrequency");

      sliderDuty.oninput = function() {
        document.getElementById("outputDuty").innerText = this.value; // Display current value
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("outputDuty").innerHTML = this.responseText;
          }
        };
        var str = "sliderDuty?val=";
        var res = str.concat(this.value);
        xhttp.open("GET", res, true);
        xhttp.send();
      };

      sliderFrequency.oninput = function() {
        document.getElementById("outputFrequency").innerText = this.value; // Display current value
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("outputFrequency").innerHTML = this.responseText;
          }
        };
        var str = "sliderFrequency?val=";
        var res = str.concat(this.value);
        xhttp.open("GET", res, true);
        xhttp.send();
      };
    </script>
  </body>
</html>
)=====";

//maps values to something that can be used in ledcWrite
//from class ntoes
void ledcAnalogWrite(uint8_t pin, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(LED_PIN, duty); // write duty to LEDC
}

//send the main HTML page
//came from CHAGPT prompt: how do i send in html page
void handleRoot() {
  h.sendhtml(body); // Send the main HTML page
}

//from chatgpt prompt: how can I use value from html slider to change frequency of ledc
//had to change it to use ledcchangeFrequency() function
void handleFrequency() {
  //get frequency value from slider
  int freqVal = h.getVal(); // Get the value from the query parameter

  //Steyn helped me figure out that this function takes in LED_RESOLUTION_BIT not LED_RESOLUTION
  ledcChangeFrequency(LED_PIN, freqVal, LEDC_RESOLUTION_BITS);

  // Prepare a response string
  String s = String(freqVal);
  h.sendplain(s); // Send a plain text response to the client
}

//taken from class lecture slides
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

  //taken from class slides
  WiFi.softAP(ssid, ""); // Set up the ESP32 as an access point
  Serial.println("AP SSID: " + String(ssid));
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start the server and attach handlers
  //taken from class lecture slides
  //adapted form chatgpt prompt: how do i adapt this for html sliders
  h.begin();
  h.attachHandler("/", handleRoot);
  h.attachHandler("/sliderDuty?val=", handleSlider); // Handler for duty cycle
  h.attachHandler("/sliderFrequency?val=", handleFrequency); // Handler for frequency

  //set up the LED pins
  pinMode(LED_PIN, OUTPUT);
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
}

void loop() {
  h.serve();
  delay(10);
}
