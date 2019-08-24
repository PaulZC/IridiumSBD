#include <IridiumSBD.h>

#include <Wire.h> //Needed for I2C communication

/*
 * BasicSend_I2C
 * 
 * This sketch sends a "Hello, world!" message from the satellite modem.
 * If you have activated your account and have credits, this message
 * should arrive at the endpoints you have configured (email address or
 * HTTP POST).
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
  int signalQuality = -1;
  int err;
  
  // Start the console serial port
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Iridium SBD BasicSend I2C");

  // Start the I2C wire port connected to the satellite modem
  Wire.begin();
  Wire.setClock(400000); //Set I2C clock speed to 400kHz

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

  // Example: Print the firmware revision
  char version[12];
  err = modem.getFirmwareVersion(version, sizeof(version));
  if (err != ISBD_SUCCESS)
  {
     Serial.print("FirmwareVersion failed: error ");
     Serial.println(err);
     return;
  }
  Serial.print("Firmware Version is ");
  Serial.print(version);
  Serial.println(".");

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

  // Send the message
  Serial.print("Trying to send the message.  This might take several minutes.\r\n");
  err = modem.sendSBDText("Hello, world!");
  if (err != ISBD_SUCCESS)
  {
    Serial.print("sendSBDText failed: error ");
    Serial.println(err);
    if (err == ISBD_SENDRECEIVE_TIMEOUT)
      Serial.println("Try again with a better view of the sky.");
  }

  else
  {
    Serial.println("Hey, it worked!");
  }

  // Clear the Mobile Originated message buffer
  Serial.println("Clearing the MO buffer.");
  err = modem.clearBuffers(ISBD_CLEAR_MO); // Clear MO buffer
  if (err != ISBD_SUCCESS)
  {
    Serial.print("clearBuffers failed: error ");
    Serial.println(err);
  }

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

  Serial.println("Bye!");
}

void loop()
{
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
