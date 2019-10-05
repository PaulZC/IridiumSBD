# IridiumSBD Arduino Library

The Iridium 9602 and 9603N are fascinating communications modules that give devices like Arduino or Raspberry Pi&trade; access to the Iridium satellite network.
This is a big deal, because it means that your application can now easily and inexpensively communicate from any point on the surface of the globe,
from the heart of the Amazon to the Siberian tundra.
This library, IridiumSBD, uses Iridium's SBD ("Short Burst Data") protocol to send and receive short messages to/from the Iridium hub.
SBD is a "text message"-like technology that supports the transmission of text or binary messages up to a certain maximum size (270 bytes received, 340 bytes transmitted).

Breakout boards for the 9602 and 9603N are available in different formats. [Rock 7 Mobile](http://rock7mobile.com) provide the original 9602-based
[RockBLOCK MK2](http://www.rock7mobile.com/products-rockblock) and the newer, more compact, 9603N-based [RockBLOCK 9603](http://www.rock7mobile.com/products-rockblock-9603). 
The breakouts are also available from SparkFun: [RockBLOCK MK2](https://www.sparkfun.com/products/13745) and [RockBLOCK 9603N](https://www.sparkfun.com/products/14498).
Both of these breakouts use serial (UART) interfacing. The [Qwiic Iridium 9603N](https://github.com/PaulZC/Qwiic_Iridium_9603N) provides I2C support instead of serial,
allowing you to add Iridium SBD communication to any Qwiic or I2C project.

A full set of [examples](https://github.com/PaulZC/IridiumSBD/tree/master/examples) demonstrate how to use both interfaces.

## Documentation

You can documentation for the library in the [documentation folder](https://github.com/PaulZC/IridiumSBD/tree/master/documentation)

## History

[Versions 1 and 2](https://github.com/mikalhart/IridiumSBD) of this library were written by Mikal Hart with generous support from [Rock 7 Mobile](http://rock7mobile.com).

This version is based on Version 2.0 of IridiumSBD but has been modified to provide I2C support for the [Qwiic Iridium 9603N](https://github.com/PaulZC/Qwiic_Iridium_9603N).
Serial will still work too of course. New [examples](https://github.com/PaulZC/IridiumSBD/tree/master/examples) demonstrate how to use the I2C interface. The serial examples
have also been restructured and enhanced.

This version of the library contains a new function called _clearBuffers_ which can be used to clear the Mobile Originated or Mobile Terminated message buffers.
This allows you to overcome the 'feature' (bug?) where the 9603N will automatically re-transmit the last MO message when checking for new MT messages.
This closes Issues [#10](https://github.com/mikalhart/IridiumSBD/issues/10) and [#11](https://github.com/mikalhart/IridiumSBD/issues/11) in Mikal's repo.

- _clearBuffers(ISBD_CLEAR_MO)_ will clear the MO buffer (default)
- _clearBuffers(ISBD_CLEAR_MT)_ will clear the MT buffer
- _clearBuffers(ISBD_CLEAR_BOTH)_ will clear both buffers

The library also includes Pull Requests [#14](https://github.com/mikalhart/IridiumSBD/pull/14) and [#21](https://github.com/mikalhart/IridiumSBD/pull/21).
There are new serial and I2C examples for _getIMEI_.

Also included is the correction for Issue [#12](https://github.com/mikalhart/IridiumSBD/issues/12).

## License

Like versions 1 and 2 of Mikal's library, this version is also distributed under a
[GNU Lesser General Public Licence v2.1](https://github.com/PaulZC/IridiumSBD/blob/master/LICENSE.md).

Enjoy!

**_Paul_**
