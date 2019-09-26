#include <IridiumSBD.h>

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS

#include <Wire.h> //Needed for I2C communication

/*
 * Beacon
 * 
 * This sketch shows how you might use a GPS with the satellite modem
 * to create a beacon device that periodically transmits a location
 * message to the configured endpoints.
 * 
 * Assumptions
 * 
 * The sketch assumes an Arduino Mega or other Arduino-like device with
 * a serial console and a hardware I2C (Wire) port. It assumes
 * the SparkFun Qwiic Iridium 9603N and (e.g.) SAM-M8Q or ZOE-M8Q Breakout are connected via I2C.
 */

#define IridiumWire Wire
#define DIAGNOSTICS false // Change this to enable diagnostics

// Time between transmissions (seconds)
#define BEACON_INTERVAL 600

// Declare the IridiumSBD object using default I2C address
IridiumSBD modem(IridiumWire);

// Declare the GPS object
SFE_UBLOX_GPS myGPS;

static const int ledPin = LED_BUILTIN;

void setup()
{
  pinMode(ledPin, OUTPUT);

  // Start the console serial port
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Iridium SBD Beacon I2C");
  
  // Start the I2C wire port connected to the satellite modem
  Wire.begin();
  Wire.setClock(400000); //Set I2C clock speed to 400kHz

  // Check that the Qwiic Iridium is attached
  if !modem.isConnected()
  {
    Serial.println("I2C device is not connected!")
    return;
  }

  Serial.println("Connecting to the GPS receiver...");
  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR
  
  // Enable the supercapacitor charger
  Serial.println("Enabling the supercapacitor charger...");
  modem.enableSuperCapCharger(true);

  // Wait for the supercapacitor charger PGOOD signal to go high
  while (!modem.checkSuperCapCharger())
  {
    Serial.println("Waiting for supercapacitors to charge...");
    delay(1000);
  }
  Serial.println("Supercapacitors charged!");

  // Enable 9603N power
  Serial.println("Enabling 9603N power...");
  modem.enable9603Npower(true);

  // Begin satellite modem operation
  Serial.println("Starting modem...");
  modem.setPowerProfile(IridiumSBD::USB_POWER_PROFILE); // Assume 'USB' power (slow recharge)
  int err = modem.begin();
  if (err != ISBD_SUCCESS)
  {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    exit(0);
  }
}

void loop()
{
  unsigned long loopStartTime = millis();

  // Begin listening to the GPS
  Serial.println("Beginning to listen for GPS traffic...");

  // Look for GPS signal for up to 5 minutes
  while ((myGPS.getFixType() == 0) && millis() - loopStartTime < 5UL * 60UL * 1000UL)
  {
    blinkLED(1000); // Flash the LED
    delay(100UL); // getFixType every 100msec to avoid thrashing the I2C bus
  }

  // Did we get a GPS fix?
  if (myGPS.getFixType() == 0)
  {
    Serial.println(F("Could not get GPS fix."));
    return;
  }

  Serial.println(F("A GPS fix was found!"));

  float latitude = ((float)(myGPS.getLatitude())) / 10000000;
  Serial.print(F("Lat: "));
  Serial.print(latitude, 6);

  float longitude = ((float)(myGPS.getLongitude())) / 10000000;
  Serial.print(F(" Lon: "));
  Serial.print(longitude, 6);
  Serial.print(F(" (degrees)"));

  float altitude = ((float)(myGPS.getAltitude())) / 1000;
  Serial.print(F(" Alt: "));
  Serial.print(altitude, 2);
  Serial.println(F(" (m)"));

  // Start talking to the Qwiic Iridium 9603N and power it up
  Serial.println("Beginning to talk to the Qwiic Iridium 9603N...");

  // Construct the message in the format: lat,lon,alt
  char lat_str[15];
  dtostrf(latitude,8,6,lat_str);
  char lon_str[15];
  dtostrf(longitude,8,6,lon_str);
  char alt_str[15];
  dtostrf(altitude,4,2,alt_str);
  char outBuffer[60]; // Always try to keep message short
  sprintf(outBuffer, "%s,%s,%s", lat_str, lon_str, alt_str);

  Serial.print("Transmitting message '");
  Serial.print(outBuffer);
  Serial.println("'");

  int err = modem.sendSBDText(outBuffer);
  if (err != ISBD_SUCCESS)
  {
    Serial.print("Transmission failed with error code ");
    Serial.println(err);
  }
  else
  {
    Serial.println("Message sent!");
  }

  // Sleep
  digitalWrite(ledPin, LOW);
  int elapsedSeconds = (int)((millis() - loopStartTime) / 1000);
  if (elapsedSeconds < BEACON_INTERVAL)
  {
    int delaySeconds = BEACON_INTERVAL - elapsedSeconds;
    Serial.print(F("Waiting for "));
    Serial.println(delaySeconds);
    Serial.println(F(" seconds"));
    delay(1000UL * delaySeconds);
  }

  // Wake
  Serial.println("Wake up!");
}

void blinkLED(unsigned long interval)
{
  digitalWrite(ledPin, (millis() / interval) % 2 == 1 ? HIGH : LOW);
}

bool ISBDCallback()
{
  blinkLED(500);
  return true;
}

void ISBDConsoleCallback(IridiumSBD *device, char c)
{
#if DIAGNOSTICS
  Serial.write(c);
#endif
}

void ISBDDiagsCallback(IridiumSBD *device, char c)
{
#if DIAGNOSTICS
  Serial.write(c);
#endif
}
