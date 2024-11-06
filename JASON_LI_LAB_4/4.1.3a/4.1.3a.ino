//code adapted from class notes and webLED
//used Chatgpt for debugging and syntax fixing
//My partner for the video was Mubai
//adjusted my code accordingly to recieve strings and to send integers
//I helped Solomon with his video
//class router not working i used my own hotspot
#include <WiFi.h>
#include <WiFiUdp.h>

//wifi name and password
const char* ssid = "yomama";
const char* password = "15261526";

//set up ip adresses
IPAddress TargetIP(192, 168, 1, 146); // Mubai's IP Address
IPAddress myIP(192, 168, 1, 138);

//port values, target port is mubai's port
//myudp port is mine
const int TargetUDPPort = 2808; 
const int MyUDPPort = 6969; 
WiFiUDP UDPTestServer; // Declare the WiFiUDP instance

//set up the pins and ledc parameters
#define POT_PIN 1
#define LEDC_RESOLUTION_BITS 14
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 20
#define LED_PIN 4

void setup() {
  //adapted from class notes
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //set up led and potentiometer pin
  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);

  //set up wifi server taken from class notes
  WiFi.config(myIP, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  UDPTestServer.begin(MyUDPPort); // Initialize the UDP instance
}

//holds udp info
byte udpBuffer[100];

//sends udp
void fncUdpSend() {
  int potValue = analogRead(POT_PIN);
  int mappedValue = map(potValue, 0, 4095, 0, LEDC_RESOLUTION);
  udpBuffer[0] = mappedValue & 0x00ff;
  udpBuffer[1] = mappedValue>>8;
  UDPTestServer.beginPacket(TargetIP, TargetUDPPort);
  UDPTestServer.write(udpBuffer,2);
  UDPTestServer.endPacket();
}

//set up packet to be handled
const int UDP_PACKET_SIZE = 100;
byte packetBuffer[UDP_PACKET_SIZE];

//handles recieving of packets
void handleUDPServer() {
  int cb = UDPTestServer.parsePacket(); // Check if a packet has been received
  if (cb) { // Ensure the packet is at least the size of an int
    int len = UDPTestServer.read(packetBuffer, UDP_PACKET_SIZE-1);
    packetBuffer[len]=0;
    Serial.printf("\nRecieved value:");
    Serial.printf("%s",packetBuffer);
    //from CHATGPT prompt: "how do i convert string to an integer variable"
    int LED_VALUE = atoi((char*)packetBuffer);
    ledcWrite(LED_PIN, LED_VALUE);
  } 
  else {
    //this part from chatgpt prompt: help me debug this (paste code)
    Serial.println("No valid packet received");
  }
}

void loop() {
  fncUdpSend(); // Call the correct function name
  handleUDPServer();
  delay(100); // Adjust delay as needed
}
