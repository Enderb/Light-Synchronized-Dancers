# Light Synchronized Dancers

## Project Overview
The purpose of this project was to create a wearable technology that could be used in a dance show. The end product was two costumes that transform the dancers wearing them into a display and an actuator. Each costume has two LED strips and two accelerometers. Costume A has two LED strips on the arms, and two accelerometers on the legs, in order to track leg movements. Costume B has two LED strips on the legs, and two accelerometers on the arms, in order to track arm movements. The main idea is that when the dancer wearing costume A moves their legs, the lights on the other dancer lights up, and when the dancer wearing costume B moves their arms, the lights on the other dancer also light up. The LEDs light up differently depending on the motion the other dancer is doing. In this way, the dancers could have a way to communicate through the lights, and eventually synchronize using their movement and the lights. As you mentioned earlier, my final product ended up as more of a pre-prototype. However, I was able to use the product to show the idea to a choreographer and start the conversation on what a piece using this wearable could look like. These conversations will be used to iterate on the design. This pre-prototype has also taught me many things about making such a wearable, and I’ll be able to take these learnings to make the next iteration better and more durable.
## The Build
Included with this document are the two Arduino sketches used to power the costumes. 
Each costume had the same components, however they were organized differently. The costumes had:
* 2 x Adafruit Neopixel Digital RGB LED Strip – 60 LED (https://www.adafruit.com/product/1461) 
* 2 x Adafruit LIS3DH Triple-Axis Accelerometer (https://www.adafruit.com/product/2809) 
* 1 x Adafruit Feather HUZZAH with ESP8266 WiFi (https://www.adafruit.com/product/2821) 
* 2 x 4 x AA Battery Holder with On/Off Switch (https://www.adafruit.com/product/830)
* 1 x Lithium Ion Battery Pack - 3.7V 4400mAh (https://www.adafruit.com/product/354) 
* 4 x 330 Ohm resistors (https://www.radioshack.com/products/radioshack-330-ohm-1-2w-5-carbon-film-resistor-pk-5) 
* 4 x 1000 uF capacitors (https://www.radioshack.com/products/radioshack-1000uf-35v-20-radial-lead-electrolytic-capacitor) 
* Industrial Strength Velcro (https://www.quill.com/velcro-industrial-strength-tape-roll-black/cbs/143739.html?hidedisruptive=1&cm_mmc=CSE_MSN_NULL_143739&mcode=CSE_MSN_NULL_143739&gclid=CMGysv354dUCFUkAfwode1ABqw&gclsrc=ds) 

Costume A had the LED strips attached to the sleeves of the costume, and the accelerometers to the legs (one per leg). Costume B had the LED strips attached to the legs, and the accelerometers to the arms (one per arm). The batteries and ESP8266 were both placed in a back pocket in the pants of the costumes. Each LED strip was powered by one of the 4 x AA Battery Holders, and the ESP8266 and the accelerometers were powered by 1 x Lithium Ion Battery Pack (one per microcontroller). The 4 x AA Battery Holders were modified so they only needed 3 x AA batteries, since that would provide the correct amount of voltage for the LED strips; 4 x AA batteries could have blown the strips. 3 x AA Battery Holders are available and would be best to use for this (https://www.adafruit.com/product/771).
The resistors were connected to the data line of each LED strip, and the capacitors were connected to the batteries of each LED strip. These are to make sure the strips are not blown by the batteries or the microcontrollers. The Velcro was used to attach the components to the costumes
Each costume had its own sketch assigned, with mostly the same code except with things inverted. Costume A, with the accelerometers on the legs, used sketch “Dancer_Legs”, and Costume B used sketch “Dancer_Arms”. The costumes communicated by using MQTT. Each had a channel that it published to, which the other subscribed to. The communication looked like this:
 
The protocol used to communicate was very simple. It was based on JSON, and only two keys were used “r” and “l”, standing in for either “right LED” or “left LED”. The values ranged from 0 to 3 for the arm LEDs and from 0 to 2 for the leg LEDs (Due to less motion from the legs, since I figured it would be unlikely for someone to move their leg straight up. This assumption is not entirely right). Each value is mapped to a certain position from the accelerometers and what color that LED would turn:
* 0 – Arm/Leg straight down – Red 
* 1 – Arm/leg to the side – Green 
* 2 – Arm/leg straight forward – Blue 
* 3 – Arm straight up – White  (This could also be mapped to a leg straight back, however I had not thought of this previously)

The mappings were done by first setting up the costumes with the accelerometers, then putting them on and looking at the readings. These readings were used to calibrate when the microcontroller would publish each value.
The libraries used in the sketches were:
* ESP8266WiFi – to connect the microcontrollers to the WiFi
* PubSubClient – to connect, publish, and subscribe to the MQTT server
* ArduinoJson – to easily read the JSON from the MQTT server
* Adafruit_LIS3DH – to read data from the accelerometers
* Adafruit_Sensor – to make the readings from the accelerometer easier to read
* Adafruit_NeoPixel – to control the LED strips
