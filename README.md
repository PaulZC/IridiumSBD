# IridiumSBD Arduino Library

The Rock 7 RockBLOCK is a fascinating communications module that gives TTL-level devices like Arduino access to the Iridium satellite network.
This is a big deal, because it means that your application can now easily and inexpensively communicate from any point on the surface of the globe,
from the heart of the Amazon to the Siberian tundra.
This library, IridiumSBD, uses Iridium's SBD ("Short Burst Data") protocol to send and receive short messages to/from the Iridium hub.
SBD is a "text message"-like technology that supports the transmission of text or binary messages up to a certain maximum size (270 bytes received, 340 bytes transmitted).
Written by Mikal Hart with generous support from Rock 7 Mobile. For more information, visit the Rock 7 http://rock7mobile.com.

This fork is based on Version 2.0 of IridiumSBD but has been modified to provide I2C support for the [Qwiic Iridium 9603N](https://github.com/PaulZC/Qwiic_Iridium_9603N).
Serial will still work too of course. New examples demonstrate how to use the I2C interface:

- **BasicSend_I2C** demonstrates how to: enable the supercapacitor charger; wait for the capacitors to charge; enable power for the 9603N; send a "Hello, world!"
text message; put the 9603N to sleep; and disable the supercapacitor charger
- **SendReceive_I2C** demonstrates bidirectional message transmission
- **Ring_I2C** demonstrates how to check if the 9603N has seen a Ring Indication and download the new Mobile Terminated message if it has
- **Beacon_I2C** demonstrates how to interface the Qwiic Iridium 9603N with (e.g.) a [SparkFun SAM-M8Q breakout](https://www.sparkfun.com/products/15210)
or [ZOE-M8Q breakout](https://www.sparkfun.com/products/15193) to form an I2C GPS + Iridium beacon or tracker
- **LowPower_I2C** demonstrates how to put the ATtiny841 into low power mode, substantially reducing the standby current

Also included is a new function called _clearBuffers_ which can be used to clear the Mobile Originated or Mobile Terminated message buffers. This allows you to
overcome the 'feature' (bug?) where the 9603N will automatically re-transmit the last MO message when checking (only) for new MT messages.
This closes Issues [#10](https://github.com/mikalhart/IridiumSBD/issues/10) and [#11](https://github.com/mikalhart/IridiumSBD/issues/11).

- _clearBuffers(ISBD_CLEAR_MO)_ will clear the MO buffer (default)
- _clearBuffers(ISBD_CLEAR_MT)_ will clear the MT buffer
- _clearBuffers(ISBD_CLEAR_BOTH)_ will clear both buffers

Also includes Pull Requests [#14](https://github.com/mikalhart/IridiumSBD/pull/14) and [#21](https://github.com/mikalhart/IridiumSBD/pull/21).
BasicSend_I2C now includes an example for getIMEI.

Also includes the correction for Issue [#12](https://github.com/mikalhart/IridiumSBD/issues/12).

Enjoy!

**_Paul_**
