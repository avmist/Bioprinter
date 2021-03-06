// Configuration file to hold DS18B20 temperature IC's addresses

//------------ Specify Which Printer ------------------------------- //
#define PRINTER_MAIN			1	// Printer Lyna Uses
#define PRINTER_DEVELOPMENT		0	// Printer Alex Uses
// ----------------------------------------------------------------- //

#if PRINTER_MAIN
uint8_t tmpAddressTube[]  = {0x28,0xDF,0x1A,0x0A,0x08,0x00,0x00,0x01};
uint8_t tmpAddressSyringe[] = {0x28,0x86,0x1B,0x0B,0x08,0x00,0x00,0xFB};
uint8_t tmpAddressNeedle[] = {0x28, 0xD2, 0xF4, 0x0A, 0x08,0x00,0x00,0x33};

#elif PRINTER_DEVELOPMENT
uint8_t tmpAddressSyringe[]  = {0x28, 0xED, 0x71, 0xDE, 0x06, 0x00, 0x00, 0x18};
//uint8_t tmpAddressTube[] = {0x28, 0x11, 0xA2, 0x94, 0x06, 0x00, 0x00, 0xFC};
uint8_t tmpAddressNeedle[] = {0x28, 0x66, 0x83, 0xDD, 0x06, 0x00, 0x00, 0xA7};

#else
#error "Need to add Temp Sensor ID to Configuration.h in Peltier Folder!!!"
#endif

// This is what is on
/*
uint8_t tmpAddressSyringe[]  = {0x28, 0xAB, 0xF7, 0xDD, 0x06, 0x00, 0x00, 0x00};
uint8_t tmpAddressTube[] = {0x28, 0x11, 0xA2, 0x94, 0x06, 0x00, 0x00, 0xFC};
uint8_t tmpAddressNeedle[] = {0x28, 0x4A, 0xDD, 0xDD, 0x06, 0x00, 0x00, 0x5B};
 */


// OLD CODE BELOW
/*
// For the Syringe
//DeviceAddress tmpAddress = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};
//uint8_t tmpAddress[]  = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};
//SL uint8_t tmpAddress[]  = {0x28, 0xED, 0x71, 0xDE, 0x06, 0x00, 0x00, 0x18};
uint8_t tmpAddressSyringe[]  = {0x28, 0xAB, 0xF7, 0xDD, 0x06, 0x00, 0x00, 0x00};
//uint8_t tmpAddress[]  = {0x28, 0xB7, 0x74, 0xDD, 0x06, 0x00, 0x00, 0xA3};
//uint8_t tmpAddressSyringe[]  = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};

// For the Tube
//uint8_t tmpAddressTube[] = {0x28, 0xC2, 0x92, 0x20, 0x06, 0x00, 0x00, 0x7A};
//uint8_t tmpAddressTube[] = {0x28, 0x55, 0x67, 0x21, 0x06, 0x00, 0x00, 0xB5};
//uint8_t tmpAddressTube[] = {0x28, 0x6B, 0x7A, 0xD7, 0x06, 0x00, 0x00, 0xA3};
uint8_t tmpAddressTube[] = {0x28, 0x11, 0xA2, 0x94, 0x06, 0x00, 0x00, 0xFC};
//uint8_t tmpAddressTube[] = {0x28, 0x11, 0xA2, 0x94, 0x06, 0x00, 0x00, 0xFC};
//uint8_t tmpAddressTube[] = {0x28, 0x2B, 0xFA, 0xDD, 0x06, 0x00, 0x00, 0x06};
//uint8_t tmpAddressTube[] = {0x28, 0xFC, 0x2E, 0xD6, 0x06, 0x00, 0x00, 0x92};

// For the Needle
//uint8_t tmpAddressNeedle[] = {0x28, 0x68, 0x1C, 0x31, 0x02, 0x00, 0x00, 0xA9};
//uint8_t tmpAddressNeedle[] = {0x28, 0x69, 0x96, 0xDE, 0x06, 0x00, 0x00, 0xA5};
// SL uint8_t tmpAddressNeedle[] = {0x28, 0x66, 0x83, 0xDD, 0x06, 0x00, 0x00, 0xA7};
uint8_t tmpAddressNeedle[] = {0x28, 0x4A, 0xDD, 0xDD, 0x06, 0x00, 0x00, 0x5B}; 
//uint8_t tmpAddressNeedle[] = {0x28, 0x94, 0x96, 0x94, 0x06, 0x00, 0x00, 0x66};
//uint8_t tmpAddressNeedle[] = {0x28, 0xD6, 0xC4, 0x55, 0x07, 0x00, 0x00, 0x76};
*/
