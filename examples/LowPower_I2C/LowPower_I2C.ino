#include <IridiumSBD.h>

#include <Wire.h> //Needed for I2C communication

/*
 * LowPower_I2C
 * 
 * This sketch demonstrates the low power mode of the Qwiic Iridium 9603N.
 * 
 * Assumptions
 * 
 * The sketch assumes an Arduino Mega or other Arduino-like device with
 * a serial console and a hardware I2C (Wire) port. It assumes
 * the SparkFun Qwiic Iridium 9603N is connected via I2C.
 */

#define IridiumWire Wire
#define DIAGNOSTICS false // Change this to enable diagnostics

// Declare the IridiumSBD object using default I2C address
IridiumSBD modem(IridiumWire);

void setup()
{
  // Start the console serial port
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Iridium SBD LowPower I2C");

  // Start the I2C wire port connected to the satellite modem
  Wire.begin();
  Wire.setClock(400000); //Set I2C clock speed to 400kHz

  // Check that the Qwiic Iridium is attached
  if (!modem.isConnected())
  {
    Serial.println("I2C device is not connected!");
    while(1);
  }
}

void loop()
{
  int signalQuality = -1;
  int err;
  
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

  // Enable power for the 9603N
  Serial.println("Enabling 9603N power...");
  modem.enable9603Npower(true);

  // Begin satellite modem operation
  Serial.println("Starting modem...");
  modem.setPowerProfile(IridiumSBD::USB_POWER_PROFILE); // Assume 'USB' power (slow recharge)
  err = modem.begin();
  if (err != ISBD_SUCCESS)
  {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    return;
  }

  // Example: Test the signal quality.
  // This returns a number between 0 and 5.
  // 2 or better is preferred.
  err = modem.getSignalQuality(signalQuality);
  if (err != ISBD_SUCCESS)
  {
    Serial.print("SignalQuality failed: error ");
    Serial.println(err);
    return;
  }

  Serial.print("On a scale of 0 to 5, signal quality is currently ");
  Serial.print(signalQuality);
  Serial.println(".");

  // Power down the modem
  Serial.println("Putting the 9603N to sleep.");
  err = modem.sleep();
  if (err != ISBD_SUCCESS)
  {
    Serial.print("sleep failed: error ");
    Serial.println(err);
  }
  
  // Disable 9603N power
  Serial.println("Disabling 9603N power...");
  modem.enable9603Npower(false);

  // Disable the supercapacitor charger
  Serial.println("Disabling the supercapacitor charger...");
  modem.enableSuperCapCharger(false);

  // Enable the ATtiny841 low power mode
  Serial.println("Enabling ATtiny841 low power mode");
  modem.enable841lowPower(true); // Change this to false if you want to measure the current draw without enabling low power mode

  Serial.println("The current draw should reduce in one second from now...");
  Serial.println("Sleeping for 30 seconds...");

  delay(30000);
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
