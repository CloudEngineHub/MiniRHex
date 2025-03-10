/*
  Based on the "Web Server Access Point (AP) Mode with MKR WiFi 1010" tutorial
  https://docs.arduino.cc/tutorials/mkr-wifi-1010/web-server-ap-mode
*/

#include <WiFiNINA.h>
#include <utility/wifi_drv.h>

WiFiServer server(80);
const int RED = 25;
const int GREEN = 26;
const int BLUE = 27;
String gait_pattern = "tripod";

void begin_wifi() {
  WiFiDrv::pinMode(GREEN, OUTPUT);
  WiFiDrv::pinMode(RED, OUTPUT);
  WiFiDrv::pinMode(BLUE, OUTPUT);
  WiFiDrv::analogWrite(GREEN, 0);
  WiFiDrv::analogWrite(RED, 0);
  WiFiDrv::analogWrite(BLUE, 0);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("No WiFi module present");
    return;
  }
  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the WiFi firmware");
  }
  WiFi.config(IPAddress(10, 0, 0, 1));
  if (WiFi.beginAP(get_ssid().c_str()) != WL_AP_LISTENING) {
    Serial.println("Creating WiFi network failed");
    return;
  }
  server.begin();
  wifi = true;

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("Website: http://");
  Serial.println(WiFi.localIP());
}

char update_wifi() {
  if (WiFi.status() == WL_AP_LISTENING) {
    WiFiDrv::analogWrite(RED, 0);
    WiFiDrv::analogWrite(GREEN, 20);
    WiFiDrv::analogWrite(BLUE, 0);
  } else if (WiFi.status() == WL_AP_CONNECTED) {
    WiFiDrv::analogWrite(RED, 0);
    WiFiDrv::analogWrite(GREEN, 0);
    if (millis() % 250 > 125) {
      WiFiDrv::analogWrite(BLUE, 20);
    } else {
      WiFiDrv::analogWrite(BLUE, 0);
    }
  } else {
    WiFiDrv::analogWrite(RED, 20);
    WiFiDrv::analogWrite(GREEN, 0);
    WiFiDrv::analogWrite(BLUE, 0);
  }
  WiFiClient client = server.available();   // listen for incoming clients
  char output = 0;
  if (client) {                             // if you get a client,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {  // empty line is end of http request
            display_webpage(client, WEBPAGE);   // display the webpage
            break;
          }
          if (currentLine.substring(0, 6) == "GET /?") {
            int first = currentLine.indexOf("?") + 1;
            int last = currentLine.indexOf(" ", first);
            String message = currentLine.substring(first, last);
            if (message.substring(0, 4) == "key=") {
              output = message[4];
            } else {
              parseParams(message);
              updateGaitParams(walk_gait);
              for (int i = 1; i <= legs_active; i++) {
                update_gait(i, legs[i].gait, legs[i].startMillis);
              }
            }
          }
          currentLine = "";
        }
        else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();                    // close the connection
  }
  if (output == ' ') output = 'q';    // space is also e-stop
  return (output);
}

void display_webpage(WiFiClient client, const char * webpage) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  const char *lineStart = webpage;
  const char *lineEnd = lineStart;
  char buffer[256];
  String line;
  for (int i = 0; i < 1000; ++i) {
    lineEnd = strchr(lineStart, '\n');
    size_t lineLength = lineEnd ? (size_t)(lineEnd - lineStart) : strlen(lineStart);
    lineLength = min(lineLength, sizeof(buffer) - 1);
    strncpy(buffer, lineStart, lineLength);
    buffer[lineLength] = '\0';
    line = String(buffer);
    subParams(line);
    client.println(line);
    if (!lineEnd) break;
    lineStart = lineEnd + 1;
  }
}

String get_ssid() {
  int id = get_id();
  switch(id) {
    case 0x0A0A: return "GANDALF";
    case 0x2245: return "IRIS";
    case 0x2238: return "PUMPKIN";
    case 0x322D: return "GOLDILOCKS";
    case 0x0A2B: return "SHADOW";
    case 0x1518: return "ZEBRA";
    // Add your robot's name here!
    // You can find the last 4 digits of the SN under Tools > Get Board Info,
    // or just look at the default name of the robot's Wi-Fi network
    default:
      String ssid = String(id, HEX);
      while (ssid.length() < 4) ssid = "0" + ssid;
      ssid.toUpperCase();
      return "MINIRHEX-" + ssid;
  }
}

int get_id() {
  #define SERIAL_NUMBER_WORD_0  *(volatile uint32_t*)(0x0080A00C)
  #define SERIAL_NUMBER_WORD_1  *(volatile uint32_t*)(0x0080A040)
  #define SERIAL_NUMBER_WORD_2  *(volatile uint32_t*)(0x0080A044)
  #define SERIAL_NUMBER_WORD_3  *(volatile uint32_t*)(0x0080A048)
  return SERIAL_NUMBER_WORD_3 & 0xFFFF;  // Last 4 hexadecimal digits of SN
}
