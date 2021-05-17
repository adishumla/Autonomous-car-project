#include <Servo.h>
#include <Wire.h>
#include <VL53L1X.h>


#define MUX_ADDRESS 0x70     // i2c-adressen till multiplexern
#define NUMBER_OF_SENSORS 2
#define SERVO_PIN 9
#define ESC_PIN 5
#define VALUE 2



Servo steering;
Servo ESC;
VL53L1X lidar;

uint16_t throttle;        // 0 - 89: back. 90 stillastående. 91 - 180: fram.
uint16_t steering_angle;  // 90 = rakt fram; 120 = max höger; 60 = max vänster;
bool error = false;
uint16_t distances[NUMBER_OF_SENSORS];  // array för att lagra sensorvärden
uint16_t value[VALUE];
//float speed;      // Faktor för att reglera hastigheten;


// Multiplexern har 8 i2c-anslutningar, för att välja vilken som skall användas skrivs en etta till motsvarande bit 
// Exempelvis: 0b00010000 innebär att anslutning 4 används
void i2cSelect(uint8_t bus) {
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(1 << bus);
  Wire.endTransmission();
  
 }

// Funktionen lidarSetup initerar alla ToF-sensorer
// Argumentet sensors anger hur många sensorer som är anslutna
// I for-loopen väljs först aktuell i2c-bus, därefter initeras sensorn.
 void lidarSetup(uint8_t sensors) {
  Wire.setClock(400000);  // Sätter i2c-klockan till 400kHz
  
  for (uint8_t i=0; i < sensors; i++) {
    i2cSelect((i+2));
    
    if (!lidar.init()) {
      error = true;
    }
    lidar.setDistanceMode(VL53L1X::Long);
    lidar.setMeasurementTimingBudget(20000);
    lidar.startContinuous(20);
  }
  
}

// funktionen getDistance() lagrar sensorvärden i den globala arrayen distances;
void getDistance() {
  for (uint8_t i=0; i < NUMBER_OF_SENSORS; i++) {
    i2cSelect((i+2));
    distances[i] = lidar.read();
  }
}

void valueDistance() {
  value[1] = distances[1];
  delay(50);
  value[2] = distances[1];
}

void setup() {
  steering.attach(SERVO_PIN);     // Initierar servo
  ESC.attach(ESC_PIN);            // Initierar ESC
  ESC.write(90);
  delay(1000);
  lidarSetup(NUMBER_OF_SENSORS);  // Initierar lidar-sensorer
}
void getDistance() {
  for (uint8_t i=0; i < NUMBER_OF_SENSORS; i++) {
    i2cSelect(i);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(":\t");
    Serial.print(sensor.read());
    Serial.print("\t\t");
  }
  Serial.println("\n");
}

void loop() {
  getDistance();
  steering.write(90);
  ESC.write(120);
                                 // ((distance[0] - distance[2])/10 + 90)
  /*if (distances[0] < 0) {        // distance[0] är vänster sensor, distance[2] är höger
    steering.write(distances[0] - distances[2] * 2 + 90); // Max höger Ex. 30 - 23 * 3 + 90 = 111
  }
  if (distances[0] > 0) {
    steering.write(distances[0] - distances[2] * 2 + 90); // Max vänster Ex. 23 - 30 * 3 + 90 = -111
  }*/
  if (value[1] = value[2]){
    ESC.write(90);
    delay(50);
    ESC.write(80);
    delay(50);
  }
  /*if (distances[1] <= 50 && distances[2] = <=50){
    steering.write(110);
    delay(200);
    ESC.write(70);
  }*/
}
