Certainly! To achieve this, we'll create a static HTML file that will be served by the Raspberry Pi's Flask application. This HTML file will include forms and buttons to manage the USB drives and display their usage. The Flask application will handle the backend logic and serve the HTML file.

### **Enhanced Project: ESP32-S2 Wi-Fi Drive Manager with Raspberry Pi Zero 2W for Multiple USB Drives**

#### **Hardware Setup:**

**Components:**
- **ESP32-S2** (with USB-to-Serial converter)
- **Raspberry Pi Zero 2W**
- **USB Hub** (to connect multiple USB drives)
- **USB Drives** (multiple)
- **Physical Button** (for triggering actions)
- **Micro-USB cables** (for power and data)

**Connections:**
- Connect the ESP32-S2 to your PC via USB.
- Connect the Raspberry Pi Zero 2W to the same USB hub or directly to the ESP32-S2 if you have a USB OTG cable.
- Connect the USB drives to the USB hub.
- Connect the physical button to the ESP32-S2 GPIO pins.

#### **Software Setup:**

### **ESP32-S2:**

**Install the ESP32 Board in Arduino IDE:**
1. Open Arduino IDE.
2. Go to `File > Preferences`.
3. In the `Additional Boards Manager URLs` field, add the following URL:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Go to `Tools > Board > Boards Manager`.
5. Search for `esp32` and install the ESP32 board package.
6. Select `ESP32-S2` from the `Board` menu.

**ESP32-S2 Code:**

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <Ethernet.h>

// Define constants
const char* ssid = "ESP32-NAS";
const char* password = "yourpassword";
const char* piIp = "192.168.4.2";
const int piPort = 5000;
const int BUTTON_PIN = 0; // GPIO pin for the button

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
```

### **Raspberry Pi Zero 2W:**

**Install Raspberry Pi OS:**
1. Download and install Raspberry Pi Imager from the official Raspberry Pi website.
2. Insert an SD card into your computer.
3. Open Raspberry Pi Imager and select the latest version of Raspberry Pi OS Lite.
4. Write the image to the SD card.
5. Insert the SD card into your Raspberry Pi Zero 2W and power it on.

**Configure Wi-Fi and SSH:**
1. Create a file named `wpa_supplicant.conf` on the boot partition of the SD card with the following content:
   ```
   country=US
   ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
   update_config=1

   network={
       ssid="ESP32-NAS"
       psk="yourpassword"
   }
   ```
2. Create an empty file named `ssh` on the boot partition to enable SSH.

**Install Required Packages:**
1. Connect to your Raspberry Pi via SSH.
2. Update the package list and install necessary packages:
   ```sh
   sudo apt update
   sudo apt install python3-flask
   ```

**Create the HTML File:**

Create a file named `index.html` in the `/var/www/html` directory:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-S2 Wi-Fi Drive Manager</title>
</head>
<body>
    <h1>ESP32-S2 Wi-Fi Drive Manager</h1>
    <a href="/">Home</a><br>
    <a href="/scan">Scan Drives</a><br>
    <a href="/usage">Check Usage</a><br>

    <h2>Manage Drives</h2>
    <form action="/mount" method="get">
        <label for="drive">Mount Drive:</label>
        <input type="text" id="drive" name="drive" placeholder="Enter drive (e.g., /dev/sda1)">
        <input type="submit" value="Mount">
    </form>

    <form action="/unmount" method="get">
        <label for="drive">Unmount Drive:</label>
        <input type="text" id="drive" name="drive" placeholder="Enter drive (e.g., /dev/sda1)">
        <input type="submit" value="Unmount">
    </form>

    <form action="/eject" method="get">
        <label for="drive">Eject Drive:</label>
        <input type="text" id="drive" name="drive" placeholder="Enter drive (e.g., /dev/sda1)">
        <input type="submit" value="Eject">
    </form>

    <h2>Drive Usage</h2>
    <pre id="usage"></pre>

    <script>
        function fetchUsage() {
            fetch('/usage')
                .then(response => response.text())
                .then(data => document.getElementById('usage').innerText = data)
                .catch(error => console.error('Error:', error));
        }

        // Fetch usage on page load
        window.onload = fetchUsage;
    </script>
</body>
</html>
```

**Raspberry Pi Code:**

```python
from flask import Flask, request, jsonify, render_template_string
import os
import subprocess

app = Flask(__name__)

MOUNT_POINT = "/media/usb"

def scan_drives():
    result = subprocess.run(['lsblk', '-o', 'NAME,SIZE,MOUNTPOINT'], stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8')

def mount_drive(drive):
    mount_point = f"{MOUNT_POINT}/{os.path.basename(drive)}"
    if not os.path.ismount(mount_point):
        os.system(f"sudo mkdir -p {mount_point}")
        os.system(f"sudo mount {drive} {mount_point}")
    return f"Drive {drive} mounted successfully at {mount_point}"

def unmount_drive(drive):
    mount_point = f"{MOUNT_POINT}/{os.path.basename(drive)}"
    if os.path.ismount(mount_point):
        os.system(f"sudo umount {mount_point}")
    return f"Drive {drive} unmounted successfully"

def eject_drive(drive):
    mount_point = f"{MOUNT_POINT}/{os.path.basename(drive)}"
    if os.path.ismount(mount_point):
        os.system(f"sudo umount {mount_point}")
        os.system(f"sudo eject {drive}")
    return f"Drive {drive} ejected successfully"

def get_drive_usage():
    result = subprocess.run(['df', '-h', MOUNT_POINT], stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8')

@app.route('/')
def index():
    return render_template_string(open('/var/www/html/index.html').read())

@app.route('/scan', methods=['GET'])
def scan():
    return scan_drives()

@app.route('/mount', methods=['GET'])
def mount():
    drive = request.args.get('drive')
    return mount_drive(drive)

@app.route('/unmount', methods=['GET'])
def unmount():
    drive = request.args.get('drive')
    return unmount_drive(drive)

@app.route('/eject', methods=['GET'])
def eject():
    drive = request.args.get('drive')
    return eject_drive(drive)

@app.route('/usage', methods=['GET'])
def usage():
    return get_drive_usage()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
```

### **Testing and Usage:**

1. **Connect the ESP32-S2 to your PC via USB.**
2. **Power on the Raspberry Pi Zero 2W and ensure it connects to the ESP32-S2 Wi-Fi network.**
3. **Open a web browser on your PC and navigate to `http://esp32.local` or `http://192.168.4.1`.**
4. **Use the web interface to:**
   - **Scan Drives:** Click the "Scan Drives" button to list all connected USB drives.
   - **Mount Drives:** Enter the drive identifier (e.g., `/dev/sda1`) and click "Mount".
   - **Unmount Drives:** Enter the drive identifier (e.g., `/dev/sda1`) and click "Unmount".
   - **Eject Drives:** Enter the drive identifier (e.g., `/dev/sda1`) and click "Eject".
   - **Check Usage:** Click "Check Usage" to see the usage statistics of the mounted drives.

### **Explanation:**

- **ESP32-S2:**
  - **Wi-Fi AP:** The ESP32-S2 creates a Wi-Fi network that the Raspberry Pi can connect to.
  - **Web Server:** The ESP32-S2 hosts a simple web server that redirects to the Raspberry Pi's web interface.
  - **Button Handling:** A physical button is connected to the ESP32-S2 to trigger HTTP requests to the Raspberry Pi.
  - **HTTP Requests:** The ESP32-S2 sends HTTP GET requests to the Raspberry Pi to perform actions like scanning, mounting, unmounting, and ejecting USB drives.

- **Raspberry Pi Zero 2W:**
  - **Wi-Fi Connection:** The Raspberry Pi automatically connects to the ESP32-S2 Wi-Fi network.
  - **Flask Web API:** The Raspberry Pi runs a Flask web application to handle HTTP requests from the ESP32-S2.
  - **HTML Interface:** The Raspberry Pi serves a static HTML file that provides a user-friendly interface for managing USB drives.
  - **Drive Management:** The Raspberry Pi supports multiple drives by dynamically creating mount points and handling actions based on the drive identifier.
  - **Usage Statistics:** The Raspberry Pi can provide disk usage statistics for all mounted drives via the web interface.

### **Conclusion:**

This enhanced project allows you to manage multiple USB drives efficiently using a combination of an ESP32-S2 and a Raspberry Pi Zero 2W. The web interface, served by the Raspberry Pi, provides a user-friendly way to scan, mount, unmount, eject, and check the usage of multiple USB drives, making it a versatile tool for various applications.