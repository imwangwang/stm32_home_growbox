/**
 * Resolutions available
 *
 * Parameters:
 *	- TM_DS18B20_Resolution_9bits:
 *		DS18B20 9 bits
 *	- TM_DS18B20_Resolution_10bits:
 *		DS18B20 10 bits
 *	- TM_DS18B20_Resolution_11bits:
 *		DS18B20 11 bits
 *	- TM_DS18B20_Resolution_12bits:
 *		DS18B20 12 bits
 */
typedef enum {
	TM_DS18B20_Resolution_9bits = 9,
	TM_DS18B20_Resolution_10bits = 10,
	TM_DS18B20_Resolution_11bits = 11,
	TM_DS18B20_Resolution_12bits = 12
} TM_DS18B20_Resolution_t;

/**
 * Start temperature conversion for specific DS18B20 on specific onewire channel
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t *ROM:
 *		Pointer to first byte of ROM address
 * 	
 * Returns 1 if device is DS18B20 or 0 if not
 */
extern uint8_t TM_DS18B20_Start(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * Start temperature conversion for all DS18B20 devices on specific onewire channel
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 *	
 * No return
 */
extern void TM_DS18B20_StartAll(TM_OneWire_t* OneWireStruct);

/**
 * Read temperature from DS18B20
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t* ROM:
 *		Pointer to first byte of ROM address
 * 	- float* destination:
 *		Pointer to float variable to store temperature
 * 	
 * Returns 1 if temperature is read OK or 0 if device is not DS18B20 or conversion is not done yet or CRC failed
 */
extern uint8_t TM_DS18B20_Read(TM_OneWire_t* OneWireStruct, uint8_t* ROM, float* destination);

/**
 * Get resolution for temperature conversion
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t* ROM: 
 *		Pointer to first byte of ROM address
 * 
 * Returns 0 if device is not DS18B20 or bits number (9 - 12)
 */
extern uint8_t TM_DS18B20_GetResolution(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * Set resolution for specific DS18B20 device
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t* ROM:
 *		Pointer to first byte of ROM address
 * 	- TM_DS18B20_Resolution_t resolution:
 *		Set resolution for DS18B20 device:
 * 			- TM_DS18B20_Resolution_9bit: 9bits resolution
 * 			- TM_DS18B20_Resolution_10bit: 10bits resolution
 * 			- TM_DS18B20_Resolution_11bit: 11bits resolution
 * 			- TM_DS18B20_Resolution_12bit: 12bits resolution
 * 		
 * Returns 0 if device is not DS18B20 or 1 if resolution is set
 */
extern uint8_t TM_DS18B20_SetResolution(TM_OneWire_t* OneWireStruct, uint8_t* ROM, TM_DS18B20_Resolution_t resolution);

/**
 * Check if device is DS18B20 with specific ROM number
 * 
 * Parameters:
 * 	- uint8_t* ROM:
 *		Pointer to first byte of ROM address
 * 	
 * Returns 1 if device is DS18B20, otherwise 0
 */
extern uint8_t TM_DS18B20_Is(uint8_t* ROM);

/**
 * Set high alarm temperature to specific DS18B20 sensor
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t* ROM:
 *		Pointer to first byte of ROM address
 * 	- int8_t temp:
 *		Temperature between -55 and 125°„C
 * 
 * Returns 0 if device is not DS18B20, otherwise 1
 */
extern uint8_t TM_DS18B20_SetAlarmHighTemperature(TM_OneWire_t* OneWireStruct, uint8_t* ROM, int8_t temp);

/**
 * Set low alarm temperature to specific DS18B20 sensor
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t* ROM:
 *		Pointer to first byte of ROM address
 * 	- int8_t temp:
 *		Temperature between -55 and 125°„C
 * 
 * Returns 0 if device is not DS18B20, otherwise 1
 */
extern uint8_t TM_DS18B20_SetAlarmLowTemperature(TM_OneWire_t* OneWireStruct, uint8_t* ROM, int8_t temp);

/**
 * Disable alarm temperature on specific DS18B20 sensor
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 * 	- uint8_t* ROM:
 *		Pointer to first byte of ROM address
 * 
 * Returns 0 if device is not DS18B20, otherwise 1
 */
extern uint8_t TM_DS18B20_DisableAlarmTemperature(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * Search for devices with alarm flag set
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 *
 * Returns 1 if any device has flag, otherwise 0
 */
extern uint8_t TM_DS18B20_AlarmSearch(TM_OneWire_t* OneWireStruct);

/**
 * Check if all DS18B20 sensors are done with temperature conversion
 * 
 * Parameters:
 *	- TM_OneWire_t* OneWireStruct:
 *		Pointer to OneWire working struct (channel)
 *
 * Return 1 if they are, otherwise 0
 */
extern uint8_t TM_DS18B20_AllDone(TM_OneWire_t* OneWireStruct);