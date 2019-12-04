#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

//Configuration of staircase
int currentMode = 1;                  //default mode
bool topToBottom = true;             //is first LED in the top of the stairs?

int theStair[] = {7, 5, 2, 5, 2, 3}; //Define each step
int LED_COUNT = 24;  //Because C is a weird language you need to add total number of LEDs

//Handle network
const char* ssid = "mySSID";             //your SSID
const char* password = "myPassword";   //your password

//Handle mqtt
const char* mqtt_server = "192.168.1.10";
const char* topic = "wemos";    // this is the [root topic]
const char* mqttUser = "mqttUsername";
const char* mqttPassword = "mqttPasword";
const char* mqttClientId = "trappen";
long timeBetweenMessages = 1000 * 20 * 1;
long lastMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient);


#define LED_PIN    D7
#define FIRST_PIR_PIN    D1         //The pir that is on the top of the stairs
#define LAST_PIR_PIN    D2          //The pir that is on the bottom of the stairs

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

//Connect to WIFI
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Handle MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0'; //fix for handling payload as a string
  Serial.print("payload: [");
  Serial.print((char *)payload);
  Serial.println("]");
  Serial.println();


  //Decode the command
  currentMode = atoi(strtok((char *)payload, ","));
  
  int checkedTemp = atoi(strtok(0, ","));
  if (checkedTemp){
    checked = true;
    } else {
    checked = false;  
      }

  customColor[0] = atoi(strtok(0, ","));
  customColor[1] = atoi(strtok(0, ","));
  customColor[2] = atoi(strtok(0, ","));
  customColor[3] = atoi(strtok(0, ","));
}

//Reconnect to MQTT server
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, mqttClientId , true );
      // ... and resubscribe
      String subscription;
      subscription += topic;
      subscription += "/";
      subscription += mqttClientId ;
      subscription += "/in";
      client.subscribe(subscription.c_str() );
      Serial.print("subscribed to : ");
      Serial.println(subscription);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" wifi=");
      Serial.print(WiFi.status());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// setup() function -- runs once at startup --------------------------------

void setup() {
  Serial.begin(115200);
  pinMode(FIRST_PIR_PIN, INPUT);
  pinMode(LAST_PIR_PIN, INPUT);
  digitalWrite(FIRST_PIR_PIN, LOW);
  digitalWrite(LAST_PIR_PIN, LOW);

  strip.setBrightness(255);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP

  setup_wifi(); //connect to WIFI

  client.setServer(mqtt_server, 1883); //connect to MQTT Server
  client.setCallback(callback);  //Define how to handle responses
}

void loop() {
  delay(100);
  // confirm still connected to mqtt server
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > timeBetweenMessages ) {
    lastMsg = now;
    String payload = String(currentMode);
    payload += ",";
    if (checked){
      payload += "1";
      } else {
        payload += "0"; 
        }
    payload += ",";
    for (int i = 0; i < 4; i++) {
      payload += customColor[i];
      if (i < 3){
        payload += ",";
        } 
    }
    String pubTopic;
    pubTopic += topic ;
    pubTopic += "/";
    pubTopic += mqttClientId;
    pubTopic += "/out";
    Serial.print("Publish topic: ");
    Serial.println(pubTopic);
    Serial.print("Publish message: ");
    Serial.println(payload);
    client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
  }
  
  switch (currentMode) {
    case 0:
      strip.clear();
      strip.show();
      break;
    case 1:
      if (checked == true) {
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

//------------------------Custom modes---------------------

void mode1(bool fromTop) {
  //Mode 1 - White fade
  int color[] = {0, 0, 0, 254};
  strip.clear();
  strip.show();
  if (fromTop) {
    for (int i = 0; i < STEPS_COUNT ; i++) {
      dimColorOnSpecificStep(i, 1, color, 25);
      delay(100);
    }

    delay(2000);

    for (int i = 0; i < STEPS_COUNT ; i++) {
      dimColorOnSpecificStep(i, 0, color, 25);
      delay(100);
    }

    delay(2);

  } else {
    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
      dimColorOnSpecificStep(i, 1, color, 25);
      delay(100);
    }

    delay(2000);

    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
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
      dimColorOnSpecificStep(i, 1, customColor, 25);
      delay(100);
    }

    delay(2000);

    for (int i = 0; i < STEPS_COUNT ; i++) {
      dimColorOnSpecificStep(i, 0, customColor, 25);
      delay(100);
    }

    delay(2);

  } else {
    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
      dimColorOnSpecificStep(i, 1, customColor, 25);
      delay(100);
    }

    delay(2000);

    for (int i = STEPS_COUNT - 1; i >= 0 ; i--) {
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
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
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
