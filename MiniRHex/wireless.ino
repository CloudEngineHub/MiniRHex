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
            display_website(client);        // display the webpage
            break;
          } else {
            currentLine = "";    // clear currentLine
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          if (currentLine.substring(currentLine.length() - 6, currentLine.length() - 1) == "GET /") {
            output = currentLine[currentLine.length() - 1]; // return the command
          }
        }
      }
    }
    client.stop();                                   // close the connection
  }
  if (output == ' ') output = 'q';                   // space is also e-stop
  return (output);
}

void display_website(WiFiClient client) {
  // HTTP headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("<!doctype html>");

  // Controls
  client.print("<div class=buttons>");
  client.print("<b></b><button onclick=\"window.location.href='/r';\">Run<br>(r)</button>");
  client.print("<b></b><button onclick=\"window.location.href='/z';\">Reboot<br>(z)</button>");
  client.print("<b></b><button onclick=\"window.location.href='/w';\">Forward<br>(w)</button><b></b><b></b>");
  client.print("<button onclick=\"window.location.href='/a';\">Left<br>(a)</button>");
  client.print("<button onclick=\"window.location.href='/q';\">Stop<br>(space)</button>");
  client.print("<button onclick=\"window.location.href='/d';\">Right<br>(d)</button><b></b>");
  client.print("<b></b><button onclick=\"window.location.href='/s';\">Reverse<br>(s)</button><b></b><b></b></div>");

  // Style
  client.print("<style>");
  client.print(".buttons {display:inline-grid; grid-template-columns: 25% 25% 25% 25%; grid-template-rows: 25% 25% 25% 25%; width: 400px; height: 400px;}");
  client.print("button {font-size: 12pt; border-width: 4px;}");
  client.print("@media (max-aspect-ratio: 1) {.buttons {width: 90vw; height: 90vw; padding: 5vw;}");
  client.print("button {font-size: 4vw; border-width: 0.5vw;}}");
  client.print("</style>");

  // Key listener
  client.print("<script>");
  client.print("document.onkeydown = function(evt) {");
  client.print("evt = evt || window.event;");
  client.print("window.location.href = \"/\"+evt.key;");
  client.print("};");
  client.print("</script>");

  // Terminate http response
  client.println();
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
