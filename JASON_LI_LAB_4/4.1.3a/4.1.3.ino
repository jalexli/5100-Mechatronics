//My teammate is Matt Rabin. His code sends 2 bytes of data, can receive data, and can determine whether it is a string or a number and convert it to an integer. 
//In my code, I send data as a string type and receive data in 2-byte format.
//I also did this data transmittion with Jason with the same code.(just changing the target ip and port)

#include <WiFi.h>
#include <WiFiUDP.h>

#define ANALOG_PIN 4 // Analog input pin connected to potentiometer
#define LED_PIN 5     // LED output pin (PWM control)

// WiFi network information
const char* ssid     = "TP-Link_8A8C";
const char* password = "12488674";

// UDP settings
WiFiUDP UDPTestServer;
IPAddress myIP(192,168,1,146); // Local IP address

unsigned int localPort = 4200; // Local UDP listening port

const char* remoteIP = "192.168.1.162"; // Remote device's IP address
unsigned int remotePort = 3333; // UDP port to send data to

const int UDP_PACKET_SIZE = 100; // Allow packets up to 100 bytes
byte packetBuffer[UDP_PACKET_SIZE]; // Buffer to hold incoming UDP packets

void handleUDPServer() {
  // Function to handle incoming UDP packets
  int cb = UDPTestServer.parsePacket();
  if (cb == 2) { // Ensure the received data length is 2 bytes
    uint8_t buffer[2]; // Used to store the two received bytes
    int len = UDPTestServer.read(buffer, 2);
    if (len == 2) {
      // Reconstruct integer in LSB first format
      int brightness = buffer[0] | (buffer[1] << 8);

      // Ensure brightness value is between 0 and 16383
      if (brightness < 0) brightness = 0;
      if (brightness > 16383) brightness = 16383;

      Serial.printf("DATA RECEIVED: %d\n", brightness);

      // Set LED brightness
      ledcWrite(LED_PIN, brightness);
    } else {
      Serial.println("Error: Incorrect data length received.");
    }
  } else if (cb > 0) {
    Serial.println("Error: Expected 2 bytes of data.");
    // Clear receive buffer to avoid residual data
    while (UDPTestServer.available()) {
      UDPTestServer.read();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  ledcAttach(LED_PIN, 10, 14); // Configure LED pin for PWM control
  WiFi.config(myIP, IPAddress(192,168,1,1), IPAddress(255,255,255,0)); // Set static IP
  WiFi.begin(ssid, password);

  // Initialize WiFi connection
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize UDP communication
  UDPTestServer.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);
}

char udpBuffer[100]; // Buffer to hold outgoing UDP data
void fncUdpSend() {
  // Function to send data via UDP
  UDPTestServer.beginPacket(remoteIP, remotePort);
  int brightnesssend = atoi(udpBuffer); // Convert string to integer
  UDPTestServer.printf("%d", brightnesssend); // Send integer as formatted string
  UDPTestServer.endPacket(); // Finish UDP packet
  Serial.println(udpBuffer); // Print sent data to Serial monitor
}

void loop() {
  // Read analog value from potentiometer and map to 0-16383
  int mv = map(analogRead(ANALOG_PIN), 0, 4095, 0, 16383);

  // Convert value to string
  char valueStr[10];
  sprintf(valueStr, "%d", mv);

  // Copy value string to UDP buffer
  strcpy(udpBuffer, valueStr);
  // Send the data via UDP
  fncUdpSend();

  // Handle incoming UDP packets
  handleUDPServer();

  delay(10);
}
