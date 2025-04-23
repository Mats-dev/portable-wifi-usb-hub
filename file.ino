#include <WiFi.h>
#include <WebServer.h>
#include <Ethernet.h>

// Define constants
const char* ssid = "ESP32-NAS";
const char* password = "yourpassword";
const char* piIp = "192.168.4.2";
const int piPort = 5000;
const int BUTTON_PIN = 0;  // GPIO pin for the button

WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

  // Start Wi-Fi in AP mode
  WiFi.softAP(ssid, password);

  // Start the web server
  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/mount", handleMount);
  server.on("/unmount", handleUnmount);
  server.on("/eject", handleEject);
  server.on("/usage", handleUsage);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleButtonPress() {
  // Send HTTP GET request to Raspberry Pi to scan drives
  sendHttpRequest(piIp, piPort, "/scan");
}

void handleRoot() {
  // Serve web interface
  String html = "<html><body><h1>ESP32-S2 Wi-Fi Drive Manager</h1>";
  html += "<a href='/'>Home</a><br>";
  html += "<a href='/scan'>Scan Drives</a><br>";
  html += "<a href='/usage'>Check Usage</a><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleScan() {
  sendHttpRequest(piIp, piPort, "/scan");
  server.send(200, "text/html", "<html><body><h1>Scanning Drives...</h1><a href='/'>Back to Home</a></body></html>");
}

void handleMount() {
  String drive = server.arg("drive");
  String url = "/mount?drive=" + drive;
  sendHttpRequest(piIp, piPort, url.c_str());
  server.send(200, "text/html", "<html><body><h1>Mounting Drive...</h1><a href='/'>Back to Home</a></body></html>");
}

void handleUnmount() {
  String drive = server.arg("drive");
  String url = "/unmount?drive=" + drive;
  sendHttpRequest(piIp, piPort, url.c_str());
  server.send(200, "text/html", "<html><body><h1>Unmounting Drive...</h1><a href='/'>Back to Home</a></body></html>");
}

void handleEject() {
  String drive = server.arg("drive");
  String url = "/eject?drive=" + drive;
  sendHttpRequest(piIp, piPort, url.c_str());
  server.send(200, "text/html", "<html><body><h1>Ejecting Drive...</h1><a href='/'>Back to Home</a></body></html>");
}

void handleUsage() {
  sendHttpRequest(piIp, piPort, "/usage");
  server.send(200, "text/html", "<html><body><h1>Checking Usage...</h1><a href='/'>Back to Home</a></body></html>");
}

void sendHttpRequest(const char* ip, int port, const char* path) {
  WiFiClient client;
  if (client.connect(ip, port)) {
    client.println("GET " + String(path) + " HTTP/1.1");
    client.println("Host: " + String(ip));
    client.println("Connection: close");
    client.println();
  }
}