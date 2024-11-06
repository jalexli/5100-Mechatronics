#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "TP-Link_8A8C"; // Replace with your network SSID
const char* password = "12488674"; // Replace with your network password

IPAddress TargetIP(192, 168, 1, 162); // Replace with the receiver's IP address
IPAddress myIP(192, 168, 1, 138);

const int SenderUDPPort = 3333; 
const int ReceiverUDPPort = 6969; 
WiFiUDP UDPTestServer; // Declare the WiFiUDP instance
char udpBuffer[100];

#define POT_PIN 1
#define LEDC_RESOLUTION_BITS 12
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 50
#define LED_PIN 4

void ledcAnalogWrite(uint8_t pin, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(LED_PIN, duty); // write duty to LEDC
}

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(myIP, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  UDPTestServer.begin(ReceiverUDPPort); // Initialize the UDP instance
}

void fncUdpSend() {
  int potValue = analogRead(POT_PIN);
  int mappedValue = map(potValue, 0, 4095, 0, LEDC_RESOLUTION);
  sprintf(udpBuffer, "%d", mappedValue); // Format the potentiometer value as a string

  UDPTestServer.beginPacket(TargetIP, SenderUDPPort);
  UDPTestServer.write(mappedValue);
  UDPTestServer.endPacket();
}

void handleUDPServer(){
  int cb = UDPTestServer.parsePacket();
  if (cb >= sizeof(int)) { // Ensure the packet is at least the size of an int
    int receivedValue;
    UDPTestServer.read((uint8_t*)&receivedValue, sizeof(receivedValue)); // Read data as an integer

    // Print the received integer value
    Serial.printf("Received value: %d\n", receivedValue);

    // Map the value if needed (assuming 0-4095 is the range)

    // Adjust LED brightness using the mapped value
    ledcAnalogWrite(LED_PIN, receivedValue);
  }
}

void loop() {
  Serial.printf("Sending value: %s\n", udpBuffer);
  fncUdpSend(); // Call the correct function name
  handleUDPServer();

  delay(100); // Adjust delay as needed
}
