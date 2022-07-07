#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <stdio.h>
#include "athttp.h"
#include "effects.h"
#include "debug.h"
#include "private.h"

#define LED_STRIP_PIN 15

// TODO increase to 60 once I've for a power supply that will handle it.
#define NUMPIXELS 50

#define CYCLES_PER_RENDER 2048

Adafruit_NeoPixel pixels(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
UART Serial2(4, 5, 0, 0);

void debugOut(const char *msg) {
  Serial.println(msg);
}

// Send AT command, wait for a delay, and write the response we get back.
// TODO: poll for result availability rather than delay.
void sendCommand(char *command, int msDelay, String what) {
  Serial2.write(command);
  delay(msDelay);
  if (Serial2.available()) {
    String response = Serial2.readString();
    Serial.print("Response to ");
    Serial.print(what);
    Serial.print(": ");
    Serial.println(response);
  }
}

#define BUF_LEN 1024
char buf[BUF_LEN];

void setup() {
  pixels.begin();

  delay(3000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("== Starting");

  // Set up ESP8266
  Serial2.begin(115200);

  digitalWrite(LED_BUILTIN, HIGH);

#if true
  sendCommand("AT+CIFSR\r\n", 500, "getting info");
#endif

// #define FIRST_TIME_SETUP
#ifdef FIRST_TIME_SETUP

  sendCommand("AT+CWMODE=1\r\n", 2000, "setting AP mode");
  // sendCommand("AT+CWLAP\r\n", 5000, "getting access points");

  snprintf(buf, BUF_LEN, "AT+CWJAP=\"%s\",\"%s\"\r\n", NETWORK_SSID, NETWORK_PASSWORD);
  sendCommand(buf, 15000, "connecting to wifi");
#endif

#if true
  sendCommand("AT+CIPMUX=1\r\n", 2000, "setting connection mode");
  sendCommand("AT+CIPSERVER=1,80\r\n", 2000, "starting webserver");

  Serial.println ("== Webserver Ready!");
  sendCommand("AT+CIFSR\r\n", 500, "getting info");

  startAmbientDisplay();
#endif
}

// Index into rgbPalette
int colourIndex = 0;

uint32_t parseColour(char *colourName) {

}

// Given a parsed request, process the command.
// Current commands are:
//   /colour/{colour} where {colour is blue, white}
//   /robot
//   /ambient
void processCommand(http_request_t *request) {
  debugOut("processCommand: entered");

  if (request->numUrlParts < 1) {
    return;
  }

  char *cmd = request->urlParts[0];
  Serial.print("Command verb is ");
  Serial.println(cmd);

  if (strcmp(cmd, "colour") == 0) {
    Serial.println("Setting colour");
    uint32_t colour = request->numUrlParts > 1 ?
      parseColour(request->urlParts[1]) :
      0x000000ff; // blue is default
    startColourDisplay(colour);
    // currentMode = WHITEOUT;
  } else if (strcmp(cmd, "ambient") == 0) {
    Serial.println("Setting ambient");
    startAmbientDisplay();
    // currentMode = AMBIENT_DRIFT;
  } else if (strcmp(cmd, "robot") == 0) {
    Serial.println("Setting robot");
    startRobotDisplay();
    // currentMode = ROBOT;
  }
  else if (strcmp(cmd, "cylon") == 0) {
    Serial.println("Setting cylon");
    startCylonDisplay();
  }
}

void sendHttpResponse(String response, http_request_t *request) {
  String header = "AT+CIPSEND=";
  header.concat(request->ipdLink);
  header.concat(",");
  header.concat(response.length());
  header.concat("\r\n");

  // debugOut("HTTP response header: ");
  // debugOut(header.c_str());

  // debugOut("HTTP response header: ");
  // debugOut(response.c_str());
  // debugOut("========");

  // Write the command that says we're going to send data.
  Serial2.write(header.c_str());
  // debugOut("Sent response HEADER");
  // debugOut(header.c_str());

  // wait for a response. should include a timeout.
  while (!Serial2.available()) ;

  // Read response.
  // TODO look for errors. Typically contains the command and "OK" and ">"
  String r = Serial2.readString();
  // debugOut("After sending header, this is first response: ");
  // debugOut(r.c_str());

  // // Response should be a ">" to say it's waiting.
  // if (Serial2.find(">")) {
  //   debugOut("Got a >");
  //   // OK, send the actual data.
  //   Serial2.write(response.c_str());
  //   debugOut("Wrote response data");
  // }

  Serial2.write(response.c_str());
}

void loop() {
  rgb_struct rgb;

  if (Serial2.available()) {
    debugOut("Got something:");
    String rawRequest = Serial2.readString();
    debugOut(&rawRequest[0]);
    debugOut("=======");

    // processCommand(request);
    http_request_t *parsed = parseHttpRequest(rawRequest);
    if (!parsed) {
      return;
    }

    Serial.println("Request is:");
    Serial.println(parsed->valid);
    Serial.println(parsed->isHTTP);
    Serial.println(parsed->numUrlParts);
    for (int i = 0; i < 3; i++) {
      Serial.print("  ");
      Serial.println(parsed->urlParts[i]);
    }

    if (parsed->isHTTP) {
      String response = "HTTP/1.1 200 OK\r\n";
      response.concat("Content-Length: 2\r\n");
      response.concat("Content-Type: text/plain\r\n");
      response.concat("\r\nOK\r\n");

      sendHttpResponse(response, parsed);
      // Serial2.write("HTTP/1.1 200 OK\r\n");
      // Serial2.write("Content-Length: 2\r\n");
      // Serial2.write("Content-Type: text/plain\r\n");
      // Serial2.write("\r\nOK\r\n");
      
    }

    if (parsed->valid) {
      processCommand(parsed);
    }
    return;
  }

#if true
  pixels.clear();

  for(int pixel=0; pixel<NUMPIXELS; pixel++) {
    int ci = colourIndex + (pixel * 2);
    ci &= 0x7ff;
    colourFromIndex(&rgb, ci, pixel);
    pixels.setPixelColor(
      pixel,
      rgb.red,
      rgb.green,
      rgb.blue
    );
  }

  colourIndex ++;
  if (colourIndex >= CYCLES_PER_RENDER) {
    colourIndex = 0;
  }

  pixels.show();
#endif

  // Serial.print(".");
}