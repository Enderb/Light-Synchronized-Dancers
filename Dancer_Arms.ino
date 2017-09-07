// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

//Some immutable definitions/constants
//Wifi info
//#define ssid "University of Washington"
//#define pass ""

#define ssid "elephant"
#define pass "213Miller9421"

//LED Pins
#define PIN 13 //right
#define PIN2 14 //left

//Declare LEDs
Adafruit_NeoPixel strip_right = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_left = Adafruit_NeoPixel(60, PIN2, NEO_GRB + NEO_KHZ800);

//MQTT server info
#define mqtt_server "mediatedspaces.net"
#define mqtt_name "hcdeiot"
#define mqtt_pass "esp8266"
#define topic_name_pub "Dancer_Arms"
#define topic_name_sub "Dancer_Legs"
char espUUID[12] = "Dancer_Arms";

//Instantiate wifi client
WiFiClient espClient;
//Instantiate MQTT client
PubSubClient mqtt(espClient);

// I2C
Adafruit_LIS3DH lis_right = Adafruit_LIS3DH();
Adafruit_LIS3DH lis_left = Adafruit_LIS3DH();

//Variables to hold incoming data
int r;
int l;
char incoming[100];

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

//function to do something when a message arrives from mqtt server
//Print message out to serial when received
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    incoming[i] = (char)payload[i];
  }
  Serial.println(incoming);

  //Instantiate objects to turn stream into JSON object
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(incoming);

  //Check the JSON object is not empty
  if(!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  r = root["r"];
  l = root["l"];

  if (r == 0){
    colorWipe_right(strip_right.Color(255, 0, 0), 0); // Red
  }

  if (r == 1) {
    colorWipe_right(strip_right.Color(0, 255, 0), 0); // Green
  }

  if (r == 2) {
    colorWipe_right(strip_right.Color(0, 0, 255), 0); // Blue
  }

  if (l == 0){
    colorWipe_left(strip_left.Color(255, 0, 0), 0); // Red
  }

  if (l == 1) {
    colorWipe_left(strip_left.Color(0, 255, 0), 0); // Green
  }

  if (l == 2) {
    colorWipe_left(strip_left.Color(0, 0, 255), 0); // Blue
  }
  
}

//function to reconnect if we become disconnected from the server
void reconnect() {
  // Loop until we're reconnected
  while (!espClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    if (mqtt.connect(espUUID, mqtt_name, mqtt_pass)) { //the connction
      Serial.println("connected");
      // Once connected, publish an announcement...
      char announce[40];
      strcat(announce, espUUID);
      strcat(announce, "is connecting. <<<<<<<<<<<");
      mqtt.publish(topic_name_pub, announce);
      
      mqtt.subscribe(topic_name_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//////////////////////////////////////SETUP
void setup(void) {
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif

  Serial.begin(115200);
  Serial.println("LIS3DH test!");

  lis_left.begin(0x19);
  lis_right.begin(0x18);
/*  if (! lis_right.begin(0x19)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start right");
   while (1);
  }
  if (! lis_left.begin(0x19)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start left");
    while (1);
  }
 */ Serial.println("LIS3DH found!");
  
  lis_right.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  lis_left.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  //Set up Wifi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  Serial.print("Your ESP has been assigned the internal IP address ");
  Serial.println(WiFi.localIP());

  //finish setting up wifi connection

  //Start connection to MQTT server
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(callback);
  
  Serial.print("Range = "); Serial.print(2 << lis_right.getRange());  
  Serial.println("G");

  //Initialize pixels, and set them to off
  strip_right.begin();
  strip_left.begin();
  strip_right.show(); 
  strip_left.show();
}

void loop() {

  //Begin MQTT loop to publish events and subscribe to messages
  mqtt.loop();
  if (!mqtt.connected()) {
    reconnect();  //Recconnect to MQTT server if connection is lost
  }
  
  lis_right.read();      // get X Y and Z data at once
  lis_left.read();
  // Then print out the raw data
  Serial.print("Right X:  "); Serial.print(lis_right.x); 
  Serial.print("  \tY:  "); Serial.print(lis_right.y); 
  Serial.print("  \tZ:  "); Serial.print(lis_right.z); 

  Serial.println();

  Serial.print("Left X:  "); Serial.print(lis_left.x); 
  Serial.print("  \tY:  "); Serial.print(lis_left.y); 
  Serial.print("  \tZ:  "); Serial.print(lis_left.z); 
  
  char message_right[25];
  char message_left[25];

  if(lis_right.x > -500 && lis_right.x < 300){
    sprintf(message_right, "{\"r\": \"%s\"}", "0");
  }else if(lis_right.x > 1500){
    sprintf(message_right, "{\"r\": \"%s\"}", "1");
  }else if(lis_right.x < -500){
    sprintf(message_right, "{\"r\": \"%s\"}", "2");  
  }

  if(lis_left.x > -500 && lis_left.x < 300){
    sprintf(message_left, "{\"l\": \"%s\"}", "0");
  }else if(lis_left.x > 1500){
    sprintf(message_left, "{\"l\": \"%s\"}", "1");
  }else if(lis_left.x < -500){
    sprintf(message_left, "{\"l\": \"%s\"}", "2");  
  }

  mqtt.publish(topic_name_pub, message_right);
  mqtt.publish(topic_name_pub, message_left);

  /* Or....get a new sensor event, normalized */ 
  //sensors_event_t event; 
  //lis.getEvent(&event);
  
  /* Display the results (acceleration is measured in m/s^2) */
  //Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  //Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
  //Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
  //Serial.println(" m/s^2 ");

  Serial.println();
 
}

// Fill the dots one after the other with a color
void colorWipe_right(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_right.numPixels(); i++) {
    strip_right.setPixelColor(i, c);
    strip_right.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipe_left(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_left.numPixels(); i++) {
    strip_left.setPixelColor(i, c);
    strip_left.show();
    delay(wait);
  }
}
