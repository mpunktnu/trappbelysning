#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "mySSID";             //your SSID
const char* password = "myPassword";   //your password
int currentMode = 1;                  //default mode
bool topToBottom = true;             //is first LED in the top of the stairs?

ESP8266WebServer server(80);

#define LED_PIN    D7
#define FIRST_PIR_PIN    D1         //The pir that is on the top of the stairs
#define LAST_PIR_PIN    D2          //The pir that is on the bottom of the stairs

int theStair[] = {7, 5, 3, 5, 5, 5};
int LED_COUNT = 30;  //Because C is a weird language


const int STEPS_COUNT = int(sizeof(theStair) / sizeof(int));

int customColor[] = {254, 254, 254, 254}; //GRBW
bool checked = true;

//// Declare our NeoPixel strip object:
////It is a G,R,B,W strip....
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGBW + NEO_KHZ800);
//// Argument 1 = Number of pixels in NeoPixel strip
//// Argument 2 = Arduino pin number (most are valid)
//// Argument 3 = Pixel type flags, add together as needed:
////   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
////   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
////   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
////   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
////   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//


//------------------------WEBPAGE HOOKS---------------------

void handleRoot() {
  server.send(200, "text/html", SendHTML(currentMode));
}

void setMode0() {
  currentMode = 0;
  server.send(200, "text/html", SendHTML(currentMode));
}

void setMode1() {
  currentMode = 1;
  server.send(200, "text/html", SendHTML(currentMode));
}

void setMode2() {
  currentMode = 2;
  int g = 0;
  int r = 0;
  int b = 0;
  int w = 0;
  if (server.arg("g") != "") {
    g = server.arg("g").toInt();
  }
  if (server.arg("r") != "") {
    r = server.arg("r").toInt();
  }
  if (server.arg("b") != "") {
    b = server.arg("b").toInt();
  }
  if (server.arg("w") != "") {
    w = server.arg("w").toInt();
  }

  customColor[0] = g;
  customColor[1] = r;
  customColor[2] = b;
  customColor[3] = w;

  server.send(200, "text/html", SendHTML(currentMode));
}

void setMode3() {
  currentMode = 3;
  int g = 0;
  int r = 0;
  int b = 0;
  int w = 0;
  if (server.arg("g") != "") {
    g = server.arg("g").toInt();
  }
  if (server.arg("r") != "") {
    r = server.arg("r").toInt();
  }
  if (server.arg("b") != "") {
    b = server.arg("b").toInt();
  }
  if (server.arg("w") != "") {
    w = server.arg("w").toInt();
  }

  customColor[0] = g;
  customColor[1] = r;
  customColor[2] = b;
  customColor[3] = w;

  server.send(200, "text/html", SendHTML(currentMode));
}

void setMode4() {
  currentMode = 4;
  server.send(200, "text/html", SendHTML(currentMode));
}


void setMode5() {
  currentMode = 5;
  int g = 0;
  int r = 0;
  int b = 0;
  int w = 0;
  if (server.arg("g") != "") {
    g = server.arg("g").toInt();
  }
  if (server.arg("r") != "") {
    r = server.arg("r").toInt();
  }
  if (server.arg("b") != "") {
    b = server.arg("b").toInt();
  }
  if (server.arg("w") != "") {
    w = server.arg("w").toInt();
  }

  customColor[0] = g;
  customColor[1] = r;
  customColor[2] = b;
  customColor[3] = w;

  server.send(200, "text/html", SendHTML(currentMode));
}

void setMode6() {
  currentMode = 6;
  server.send(200, "text/html", SendHTML(currentMode));
}

void check() {
  if (checked) {
    checked = false;
  } else {
    checked = true;
  }

  server.send(200, "text/html", SendHTML(currentMode));
}

// setup() function -- runs once at startup --------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(FIRST_PIR_PIN, INPUT);
  pinMode(LAST_PIR_PIN, INPUT);
  digitalWrite(FIRST_PIR_PIN, LOW);
  digitalWrite(LAST_PIR_PIN, LOW);

  strip.setBrightness(255);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP

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

  //------------------------WEBPAGE HOOKS---------------------
  server.on("/", handleRoot);             // what happens in the root
  server.on("/setmode0", setMode0);
  server.on("/setmode1", setMode1);
  server.on("/setmode2", setMode2);
  server.on("/setmode3", setMode3);
  server.on("/setmode4", setMode4);
  server.on("/setmode5", setMode5);
  server.on("/setmode6", setMode6);
  server.on("/checked", check);           //When checkbox is checked

  server.begin();
  Serial.println("HTTP server started");
}

//------------------------Custom modes---------------------

void mode1(bool fromTop) {
  //Mode 1 - White fade
  int color[] = {0, 0, 0, 254};
  strip.clear();
  strip.show();
  if (fromTop) {
    for (int i = 0; i < STEPS_COUNT ; i++) {
      server.handleClient();
      dimColorOnSpecificStep(i, 1, color, 25);
      delay(100);
    }

    delay(2000);

    for (int i = 0; i < STEPS_COUNT ; i++) {
      server.handleClient();
      dimColorOnSpecificStep(i, 0, color, 25);
      delay(100);
    }

    delay(2);

  } else {
    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
      server.handleClient();
      dimColorOnSpecificStep(i, 1, color, 25);
      delay(100);
    }

    delay(2000);

    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
      server.handleClient();
      dimColorOnSpecificStep(i, 0, color, 25);
      delay(100);
    }

    delay(2);
  }
}

void mode2(bool fromTop) {
  //Mode 2 - Custom color fade when PIR is active.
  strip.clear();
  strip.show();
  if (fromTop) {
    for (int i = 0; i < STEPS_COUNT ; i++) {
      server.handleClient();
      dimColorOnSpecificStep(i, 1, customColor, 25);
      delay(100);
    }

    delay(2000);

    for (int i = 0; i < STEPS_COUNT ; i++) {
      server.handleClient();
      dimColorOnSpecificStep(i, 0, customColor, 25);
      delay(100);
    }

    delay(2);

  } else {
    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
      server.handleClient();
      dimColorOnSpecificStep(i, 1, customColor, 25);
      delay(100);
    }

    delay(2000);

    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
      server.handleClient();
      dimColorOnSpecificStep(i, 0, customColor, 25);
      delay(100);
    }

    delay(2);
  }
}

void mode3() {
  //Always on in custom color
  paintIt(customColor);
}

void mode4(int snakeLength, bool fromTop) {
  //Snake in random colors!
  if (fromTop) {
    for (int i = 0; i < LED_COUNT + snakeLength; i++) {
      server.handleClient();
      int pixelHue = 20000 + (i * 65536 / strip.numPixels());

      if (i < LED_COUNT) { //If we are within the limits of the stairs
        //strip.setPixelColor(i, strip.Color(0,0,255,0));   //  Set pixel's color (in RAM)
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }

      if (i >= snakeLength) { //turn off LED after the snake
        strip.setPixelColor(i - snakeLength, strip.Color(0, 0, 0, 0));
      }

      strip.show();
      delay(100);

    }
  } else {

    for (int i = LED_COUNT; i >= -snakeLength; i--) {
      server.handleClient();
      int pixelHue = 20000 + (i * 65536 / strip.numPixels());

      if (i < LED_COUNT) { //If we are within the limits of the stairs
        //strip.setPixelColor(i, strip.Color(0,0,255,0));   //  Set pixel's color (in RAM)
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }

      if (LED_COUNT - 1 - i >= snakeLength) { //turn off LED after the snake
        strip.setPixelColor(i + snakeLength, strip.Color(0, 0, 0, 0));
      }

      strip.show();
      delay(100);

    }
  }
}

void mode5(int waterfallLength, bool fromTop) {
  //Mode 5 - Waterfall
  if (fromTop) {
    for (int i = 0; i < STEPS_COUNT + waterfallLength ; i++) {
      server.handleClient();
      if (i < STEPS_COUNT) {
        onStep(i, customColor);
      }

      if (i >= waterfallLength) { //turn off step after the fall
        offStep(i - waterfallLength, customColor);
      }
      strip.show();
      delay(200);
    }
  } else {
    for (int i = STEPS_COUNT; i >= -waterfallLength ; i--) {
      server.handleClient();
      if (i < STEPS_COUNT) {
        onStep(i, customColor);
      }

      if (STEPS_COUNT - i >= waterfallLength) { //turn off step after the fall
        offStep(waterfallLength + i, customColor);
      }
      strip.show();
      delay(200);
    }
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void mode6(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) { // Repeat 30 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      server.handleClient();
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void loop() {
  server.handleClient();  //turns first LED to green!?

  switch (currentMode) {
    case 0:
      strip.clear();
      strip.show();
      break;
    case 1:
      if (checked) {
        if (digitalRead(FIRST_PIR_PIN) == HIGH) {
          mode1(topToBottom);
        } else if (digitalRead(LAST_PIR_PIN) == HIGH) {
          mode1(!topToBottom);
        }
      } else {
        mode1(topToBottom);
      }
      break;
    case 2:
      if (checked) {
        if (digitalRead(FIRST_PIR_PIN) == HIGH) {
          mode2(topToBottom);
        } else if (digitalRead(LAST_PIR_PIN) == HIGH) {
          mode2(!topToBottom);
        }
      } else {
        mode2(topToBottom);
      }
      break;
    case 3:
      if (checked) {
        if (digitalRead(FIRST_PIR_PIN) == HIGH || digitalRead(LAST_PIR_PIN) == HIGH) {
          mode3();
          delay(5000);
          strip.clear();
          strip.show();
        }
      } else {
        mode3();
      }
      break;
    case 4:
      if (checked) {
        if (digitalRead(FIRST_PIR_PIN) == HIGH) {
          mode4(10, topToBottom);
        } else if (digitalRead(LAST_PIR_PIN) == HIGH) {
          mode4(10, !topToBottom);
        }
      } else {
        mode4(10, topToBottom);
      }
      break;
    case 5:
      if (checked) {
        if (digitalRead(FIRST_PIR_PIN) == HIGH || digitalRead(LAST_PIR_PIN) == HIGH) {
          mode5(2, topToBottom);
          delay(5000);
          strip.clear();
          strip.show();
        }
      } else {
        mode5(2, topToBottom);
      }
      break;
    case 6:
      if (checked) {
        if (digitalRead(FIRST_PIR_PIN) == HIGH || digitalRead(LAST_PIR_PIN) == HIGH) {
          mode6(75);
          strip.clear();
          strip.show();
        }
      } else {
        mode6(75);
      }
      break;
  }
}

//---------------Help functions

void paintIt(int color[]) {
  for (int i = 0; i < LED_COUNT; i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(color[0], color[1], color[2], color[3]));   //  Set pixel's color (in RAM)
  }
  strip.show();
}

void onStep(int theStep, int color[]) {
  int firstLED = 0;
  int lastLED = -1; //compensate for starting at 0

  for (int i = 0; i < theStep; i++) {
    firstLED += theStair[i];
  }

  for (int i = 0; i <= theStep; i++) {
    lastLED += theStair[i];
  }

  for (int i = firstLED; i <= lastLED; i++) {
    strip.setPixelColor(i, strip.Color(color[0], color[1], color[2], color[3])); //  Set pixel's color (in RAM)
  }


}

void offStep(int theStep, int color[]) {
  int firstLED = 0;
  int lastLED = -1; //compensate for starting at 0

  for (int i = 0; i < theStep; i++) {
    firstLED += theStair[i];
  }

  for (int i = 0; i <= theStep; i++) {
    lastLED += theStair[i];
  }

  for (int i = firstLED; i <= lastLED; i++) {
    strip.setPixelColor(i, strip.Color(color[0] * 0.25, color[1] * 0.25, color[2] * 0.25, color[3] * 0.25));   //  Turn off completely
  }

}

void dimColorOnSpecificStep(int theStep, bool up, int color[], int fade) {
  int firstLED = 0;
  int lastLED = -1; //compensate for starting at 0

  for (int i = 0; i < theStep; i++) {
    firstLED += theStair[i];
  }

  for (int i = 0; i <= theStep; i++) {
    lastLED += theStair[i];
  }

  if (up == 1) {
    for (float j = 0; j <= 100; j += fade) {
      for (int i = firstLED; i <= lastLED; i++) {
        strip.setPixelColor(i, strip.Color(color[0] * (j / 100), color[1] * (j / 100), color[2] * (j / 100), color[3] * (j / 100))); //  Set pixel's color (in RAM)
      }
      strip.show();
      delay(80);
    }
  } else {
    for (float j = 100; j > 0; j -= fade) {
      for (int i = firstLED; i <= lastLED; i++) {
        strip.setPixelColor(i, strip.Color(color[0] * (j / 100), color[1] * (j / 100), color[2] * (j / 100), color[3] * (j / 100))); //  Set pixel's color (in RAM
      }
      strip.show();
      delay(80);
    }
    for (int i = firstLED; i <= lastLED; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0, 0));   //  Turn off completely
    }
    strip.show();
  }
}

//-----BUILD WEBPAGE------

String SendHTML(int currentMode) {
  String page = "<!DOCTYPE html> <html>\n";
  page += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  page += "<title>m.nu Stairway to heaven!</title>\n";
  page += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  page += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  page += ".dropdown {position: relative;display: inline-block;}";
  page += ".dropdown-content {display: none;position: absolute;background-color: #f1f1f1;min-width: 240px;box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);z-index: 1;}";
  page += ".dropdown-list {color: black;padding: 12px 16px;text-decoration: none;display: block;}";
  page += ".dropdown-list:hover {cursor: pointer;background-color: #ddd;}";
  page += ".dropdown:hover .dropdown-content {display: block;}";
  page += ".dropdown:hover .button {background-color: #3e8e41;}";
  page += ".button {display: block;background-color: #1abc9c;min-width:240px;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto;cursor: pointer;border-radius: 4px;}\n";
  page += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  page += "</style>\n";
  page += "</head>\n";
  page += "<body>\n";
  page += "<h1>Controll your stair with m.nu</h1>\n";
  page += "<h3>Current mode: ";
  page += currentMode;
  page += "</h3>\n";
  page += "<div>";
  page += "  Color: <br><br>";
  page += "  G:<input type='text' maxlength='3' name='customColorG' id='customColorG' value='";
  page += customColor[0];
  page += "' size=1 autofocus>";
  page += "  R:<input type='text' maxlength='3' name='customColorR' id='customColorR' value='";
  page += customColor[1];
  page += "' size=1 autofocus>";
  page += "  B:<input type='text' maxlength='3' name='customColorB' id='customColorB' value='";
  page += customColor[2];
  page += "' size=1 autofocus>";
  page += "  W:<input type='text' maxlength='3' name='customColorW' id='customColorW' value='";
  page += customColor[3];
  page += "' size=1 autofocus>";
  page += "</div><br>";
  if (checked) {
    page += "<div><input type='checkbox' name='movement' id='movement' checked>Activate on movement</input></div>";
  } else {
    page += "<div><input type='checkbox' name='movement' id='movement'>Activate on movement</input></div>";
  }
  page += "<div class=\"dropdown\"><br><br>\n";
  page += "<button class=\"button\">Change mode:</button>\n";
  page += "<div class=\"dropdown-content\">\n";
  page += "<div class='dropdown-list' id='setmode0'>0: Turn off</div>";
  page += "<div class='dropdown-list' id='setmode1'>1: White dim</div>";
  page += "<div class='dropdown-list' id='setmode2'>2: Custom color dim</div>";
  page += "<div class='dropdown-list' id='setmode3'>3: Custom color ON</div>";
  page += "<div class='dropdown-list' id='setmode4'>4: Rainbow snake</div>";
  page += "<div class='dropdown-list' id='setmode5'>5: Waterfall</div>";
  page += "<div class='dropdown-list' id='setmode6'>6: Partymode</div>";
  page += "</div>\n";
  page += "</div>\n";
  page += "<script src='https://code.jquery.com/jquery-2.2.4.min.js'></script>";
  page += " <script type='text/javascript'>";
  page += "if(window.location.pathname != '/'){";
  page += "  window.location.href = window.location.origin;";
  page += "  }";
  page += "$(document).ready(function(){";
  page += "  $('#setmode0').click(function(){";
  page += "      window.location.href = '/setmode0';";
  page += "      });";
  page += "  $('#setmode1').click(function(){";
  page += "      window.location.href = '/setmode1';";
  page += "      });";
  page += "  $('#setmode2').click(function(){";
  page += "G = $('#customColorG').val();";
  page += "R = $('#customColorR').val();";
  page += "B = $('#customColorB').val();";
  page += "W = $('#customColorW').val();";
  page += "      window.location.href = '/setmode2?g='+G+'&r='+R+'&b='+B+'&w='+W;";
  page += "      });";
  page += "  $('#setmode3').click(function(){";
  page += "G = $('#customColorG').val();";
  page += "R = $('#customColorR').val();";
  page += "B = $('#customColorB').val();";
  page += "W = $('#customColorW').val();";
  page += "      window.location.href = '/setmode3?g='+G+'&r='+R+'&b='+B+'&w='+W;";
  page += "      });";
  page += "  $('#setmode4').click(function(){";
  page += "      window.location.href = '/setmode4';";
  page += "      });";
  page += "  $('#setmode5').click(function(){";
  page += "G = $('#customColorG').val();";
  page += "R = $('#customColorR').val();";
  page += "B = $('#customColorB').val();";
  page += "W = $('#customColorW').val();";
  page += "      window.location.href = '/setmode5?g='+G+'&r='+R+'&b='+B+'&w='+W;";
  page += "      });";
  page += "  $('#setmode6').click(function(){";
  page += "      window.location.href = '/setmode6';";
  page += "      });";
  page += "  $('#movement').change(function(){";
  page += "      window.location.href = '/checked';";
  page += "      });";
  page += "});";
  page += "</script>";
  page += "</body>\n";
  page += "</html>\n";
  return page;
}
