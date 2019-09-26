#include <IridiumSBD.h>

#include <Wire.h> //Needed for I2C communication

/*
 * SendReceive
 * 
 * This sketch demonstrates a basic bidirectional transmission.  Before
 * loading this sketch, send a message to your Iridium modem from your
 * control panel or via email.  This sketch will send a binary buffer
 * of 11 bytes, then attempt to read the next incoming messages (if any).
 * It stops when there are no more messages to read.
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

uint8_t buffer[200] = 
{ 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89 };

void setup()
{
  int signalQuality = -1;

  // Start the console serial port
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Iridium SBD SendReceive I2C");

  // Start the I2C wire port connected to the satellite modem
  Wire.begin();
  Wire.setClock(400000); //Set I2C clock speed to 400kHz

  // Check that the Qwiic Iridium is attached
  if (!modem.isConnected())
  {
    Serial.println("I2C device is not connected!");
    while(1);
  }

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
  int err = modem.begin();
  if (err != ISBD_SUCCESS)
  {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    return;
  }

  // Check the signal quality (optional)
  err = modem.getSignalQuality(signalQuality);
  if (err != 0)
  {
    Serial.print("SignalQuality failed: error ");
    Serial.println(err);
    exit(1);
  }

  Serial.print("Signal quality is ");
  Serial.println(signalQuality);
}

static bool messageSent = false;

void loop()
{
  int err;
  
  // Read/Write the first time or if there are any remaining messages
  if (!messageSent || modem.getWaitingMessageCount() > 0)
  {
    size_t bufferSize = sizeof(buffer);

    // First time through send+receive; subsequent loops receive only
    if (!messageSent)
      err = modem.sendReceiveSBDBinary(buffer, 11, buffer, bufferSize);
    else
      err = modem.sendReceiveSBDText(NULL, buffer, bufferSize);
      
    if (err != ISBD_SUCCESS)
    {
      Serial.print("sendReceiveSBD* failed: error ");
      Serial.println(err);
    }
    else // success!
    {
      messageSent = true;
      Serial.print("Inbound buffer size is ");
      Serial.println(bufferSize);
      for (int i=0; i<bufferSize; ++i)
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
    }
    
    // Clear the Mobile Originated message buffer to avoid re-sending the message during subsequent loops
    Serial.println("Clearing the MO buffer.");
    err = modem.clearBuffers(ISBD_CLEAR_MO); // Clear MO buffer
    if (err != ISBD_SUCCESS)
    {
      Serial.print("clearBuffers failed: error ");
      Serial.println(err);
    }
  }
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
