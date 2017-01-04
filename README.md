# MySORP
MySensors Firmware for Atlas scentific ORP sensor (EZO/ORP) and i2c protocol

## Hardware
* [Wemos D1](https://www.wemos.cc/product/d1.html) (not mini) or [Iteaduino UNO](https://www.itead.cc/iteaduino-uno.html) (be carefull standard UNO R3 doesn't work out of the box with RFM69 radio because input are not 5V compliant). Can work with any 3.3V UNO board (Seedstudio ?)
* Atlas Scientific [ORP Probe ](http://www.atlas-scientific.com/product_pages/probes/orp_probe.html)
* Atlas scientific [EZO/ORP](http://www.atlas-scientific.com/product_pages/circuits/ezo_orp.html) configured in I2C mode (see [datasheet](http://www.atlas-scientific.com/_files/_datasheets/_circuit/ORP_EZO_datasheet.pdf))
* Atlas scientfic [Tentacle Shield Mini](http://www.atlas-scientific.com/product_pages/components/tentacle-shield-mini.html) or Tentacle (untested)
* [RFM69](http://www.hoperf.com/rf_transceiver/modules/RFM69W.html) Radio or NRF24L01 (not tested)
* [MySensors](https://www.mysensors.org/) Gateway
* Water ;)

## Software
* [platformio](http://platformio.org/) (not tested with arduino IDE)
* MySensors Controller (Personnaly I use [OpenHab2](http://www.openhab.org/) with [Mysensors plugin](https://github.com/tobof/openhab2-addons/wiki))

## Status
Early Stage

## TODO
lot of things !
