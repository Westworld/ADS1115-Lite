/**************************************************************************/
/*!
Adapted from adafruit ADS1015/ADS1115 library

    v1.0 - First release
*/
/**************************************************************************/

#include "Arduino.h"
#include "ADS1115_lite.h"

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1115 class w/appropriate properties
*/
/**************************************************************************/
ADS1115_lite::ADS1115_lite(uint8_t i2cAddress) {
	//_useWire->begin();
	_i2cAddress = i2cAddress;
	_gain = ADS1115_REG_CONFIG_PGA_2_048V; /* +/- 6.144V range (limited to VDD +0.3V max!) */
	_mux = ADS1115_REG_CONFIG_MUX_DIFF_0_1; /* to default */
	_rate = ADS1115_REG_CONFIG_DR_128SPS; /* to default */
}

ADS1115_lite::begin(TwoWire &useWire) {
	_useWire = &useWire;
	_useWire->begin();
}

/**************************************************************************/
/*!
    @brief  Sets up the HW (reads coefficients values, etc.)
*/
/**************************************************************************/
bool ADS1115_lite::testConnection() {
	_useWire->beginTransmission(_i2cAddress);
	_useWire->write(ADS1115_REG_POINTER_CONVERT);
	_useWire->endTransmission();
	_useWire->requestFrom(_i2cAddress, (uint8_t)2);
	if (_useWire->available()) {return 1;}
	return 0;
}

/**************************************************************************/
/*!
    @brief  Sets the gain and input voltage range.  Valid numbers:
	 ADS1115_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
	 ADS1115_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
	 ADS1115_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
	 ADS1115_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
	 ADS1115_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
	 ADS1115_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16
*/
/**************************************************************************/
void ADS1115_lite::setGain(uint16_t gain) {
  _gain = gain;
}

/**************************************************************************/
/*!
    @brief  Sets the mux.  Valid numbers:
	ADS1115_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
	ADS1115_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
	ADS1115_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
	ADS1115_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
	ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3
*/
/**************************************************************************/
void ADS1115_lite::setMux(uint16_t mux) {
	_mux = mux;
}
/**************************************************************************/
/*!
    @brief  Sets the mux.  Valid numbers:
	ADS1115_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
	ADS1115_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
	ADS1115_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
	ADS1115_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
	ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3
*/
/**************************************************************************/
void ADS1115_lite::setSampleRate(uint8_t rate) {
	_rate = rate;
}
/**************************************************************************/
/*! 
		Trigger Conversion to Read later
*/
/**************************************************************************/
void ADS1115_lite::triggerConversion() {
	
	//Bits 0 through 4 deal with the comparator function.  The combined default value for these bits is 0x0003
		uint16_t config = 0x0003; // Disable the comparator (default val)

	// OR in the Data rate or Sample Per Seconds bits 5 through 7
		config |= _rate;

	// OR in the mode bit 8
		config |= ADS1115_REG_CONFIG_MODE_SINGLE; // Single-shot mode (default) 

	// OR in the PGA/voltage range bits 9 through 11
		config |= _gain;

	// OR in the mux channel, bits 12 through 14
		config |= _mux;

	// OR in the start conversion bit bit 15
		config |= ADS1115_REG_CONFIG_OS_SINGLE;

	// Write config register to the ADC
		_useWire->beginTransmission(_i2cAddress);
		_useWire->write(ADS1115_REG_POINTER_CONFIG);
		_useWire->write((uint8_t)(config>>8));
		_useWire->write((uint8_t)(config & 0xFF));
		_useWire->endTransmission();
}

/**************************************************************************/
/*!
    @brief  Waits and returns the  last conversion results
*/
/**************************************************************************/
int16_t ADS1115_lite::getConversion() {  // Wait for the conversion to complete
   do {
      } while(!isConversionDone());
	  
  // Read the conversion results
	_useWire->beginTransmission(_i2cAddress); //Sets the Address of the ADS1115.
	_useWire->write(ADS1115_REG_POINTER_CONVERT); //queue the data to be sent, in this case modify the pointer register so that the following RequestFrom reads the conversion register
	_useWire->endTransmission(); //Send the data
	
	_useWire->requestFrom(_i2cAddress, (uint8_t)2); //Request the 2 byte conversion register
return ((_useWire->read() << 8) | _useWire->read()); //Read each byte.  Shift the first byte read 8 bits to the left and OR it with the second byte.

}
/**************************************************************************/
/*!
    @brief  returns true if the conversion is done
*/
/**************************************************************************/
bool ADS1115_lite::isConversionDone() {
	
	_useWire->beginTransmission(_i2cAddress); //Sets the Address of the ADS1115.
	_useWire->write(ADS1115_REG_POINTER_CONFIG); //queue the data to be sent, in this case modify the pointer register so that the following RequestFrom reads the config register
	_useWire->endTransmission(); //Set the stop bit
	
	_useWire->requestFrom(_i2cAddress, (uint8_t)2); //Request 2 byte config register
  return ((_useWire->read() << 8) | _useWire->read())>>15 ; //Read 2 bytes.  Return the most signifagant bit
}
