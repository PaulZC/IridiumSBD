#include <IridiumSBD.h>

#include <Wire.h> //Needed for I2C communication

/*
 * Ring_I2C
 * 
 * This sketch demonstrates how to use the Iridium RING line to detect
 * when inbound messages are available and retrieve them.
 * 
 * Assumptions
 * 
 * The sketch assumes an Arduino Mega or other Arduino-like device with
 * a serial console and a hardware I2C (Wire) port. It assumes
 * the SparkFun Qwiic Iridium 9603N is connected via I2C.
 */
 
#define IridiumWire Wire
#define DIAGNOSTICS true // Change this to disable diagnostics

// Declare the IridiumSBD object using default I2C address
IridiumSBD modem(IridiumWire);

void setup()
{
  int signalQuality = -1;
  int err;

  // Start the console serial port
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Iridium SBD Ring I2C");
  
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

  // Enable 9603N power
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
    exit(0);
  }

  // Check signal quality for fun.
  err = modem.getSignalQuality(signalQuality);
  if (err != ISBD_SUCCESS)
  {
    Serial.print("SignalQuality failed: error ");
    Serial.println(err);
    return;
  }

  Serial.print("Signal quality is ");
  Serial.println(signalQuality);

  // Check network available for fun.
  Serial.print("Network is ");
  if (!modem.checkNetworkAvailable()) Serial.print("NOT ");
  Serial.println("available!");

  Serial.println("Begin waiting for RING...");
}


void loop()
{
  static int err = ISBD_SUCCESS;
  delay(100UL); // Go round the loop no more than once every 100 msec to avoid thrashing the I2C bus
  bool ring = modem.hasRingAsserted();
  if (ring || modem.getWaitingMessageCount() > 0 || err != ISBD_SUCCESS)
  {
    if (ring)
      Serial.println("RING asserted! Let's try to read the incoming message.");
    else if (modem.getWaitingMessageCount() > 0)
      Serial.println("Waiting messages available.  Let's try to read them.");
    else
      Serial.println("Let's try again.");

    uint8_t buffer[200];
    size_t bufferSize = sizeof(buffer);
    err = modem.sendReceiveSBDText(NULL, buffer, bufferSize);
    if (err != ISBD_SUCCESS)
    {
      Serial.print("sendReceiveSBDBinary failed: error ");
      Serial.println(err);
      return;
    }

    Serial.println("Message received!");
    Serial.print("Inbound message size is ");
    Serial.println(bufferSize);
    for (int i=0; i<(int)bufferSize; ++i)
    {
      Serial.print(buffer[i], HEX);
      if (isprint(buffer[i]))
      {
        Serial.print("(");
        Serial.write(buffer[i]);
        Serial.print(")");
      }
      Serial.print(" ");
    }
    Serial.println();
    Serial.print("Messages remaining to be retrieved: ");
    Serial.println(modem.getWaitingMessageCount());

    // Clean up by making sure the Ring Indicator flag is clear.
    // The RI pin pulses twice (slowly!) for each message _and_ SBDRING can arrive over the serial port,
    // which otherwise can cause the code to go around the loop multiple times for each message!
    Serial.println();
    Serial.println("Making sure the Ring Indicator flag is clear.");
    modem.clearRingIndicator(); // Clear the Ring Indicator flag
    Serial.println();
  }
}

#if DIAGNOSTICS
void ISBDConsoleCallback(IridiumSBD *device, char c)
{
  Serial.write(c);
}

void ISBDDiagsCallback(IridiumSBD *device, char c)
{
  Serial.write(c);
}
#endif
