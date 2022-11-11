#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Wire.h>

AsyncWebServer server(80);

#define FAN_PIN 2
#define DELAY_TIME 1000 // time between measurements [ms]

// This next section specifies variables and constants needed by the code snippet to convert serial input from ASCII to int
#define numberOfDigits 3
char theNumberString[numberOfDigits + 1];
int input;

// HTML based manual Fan speed input
const char* PARAM_INPUT_1 = "input1";

// WiFi settings
const char* ssid = "SSID";   //replace with your SSID
const char* password = "PASS"; //password
String hostname = "Ventole_Einstein";

// Default Fan Speed
int fanSpeedPercent = 20;

// HTML WEB interface
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ControllerVentoleEinstein</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h2>Ciao! Sono l'ESP8266 che gestisce le ventole di Einstein (HP DL380p G8)</h2>
      <form action="/get">
        % manuale: <input type="number" name="input1">
        <input type="submit" value="Submit">
      </form><br>
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 2.0rem;}
    h3 {font-size: 2.0rem; color: #FF0000;}
  </style>
  
  <h2>Clicca qui sotto per aggiornare il firmware, o il secondo link per il sorgente.</h2>
  <a href='/update'>&#11014;&#65039; OTA Firmware_Update &#11014;&#65039;</a>
  <a href='https://github.com/francescocy/ESP_fan_controller'>&#128049;GitHub&#128025;</a><br><br>

</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// the setup function runs once when you press reset or power the board
void setup(void) {
  pinMode(FAN_PIN, OUTPUT);
  analogWriteFreq(25000);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  //  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = inputMessage;
      if (inputMessage.toFloat() > 5 && inputMessage.toFloat() < 101) {
        fanSpeedPercent = inputMessage.toFloat();
      }
    }
  else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.print("Speed received from HTML page: " );
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");  
}

// the loop function runs over and over again forever
void loop() {
  // Check if a serial interface is running
  if(Serial.available()){
    // Check the first character: if it is "S" it will be followed by 3 digits for the fan speed
    // S025 = 25%
      if(Serial.read() == 'S')
        {
        for (int i = 0; i < numberOfDigits; theNumberString[i++] = Serial.read());
        theNumberString[numberOfDigits] = 0x00;
        input = atoi(theNumberString);
        Serial.print("You typed: " );
        Serial.println(input);
        if (input > 4 && input < 101) {
          fanSpeedPercent = input;
        }
        else {
          Serial.print("Fan Speed out of Range!" );
        }
      }
    }
  
  Serial.print("Fan speed is ");
  Serial.print(fanSpeedPercent);
  Serial.println(" %");

  Serial.println();

  analogWrite(FAN_PIN, ((100 - fanSpeedPercent) / 100.0) * 255);
  delay(1000);
}
