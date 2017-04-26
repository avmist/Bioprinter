
#include "Temp_DS18B20.h"

/* Constructors for TempDS18B20 class */
Temp_DS18B20::Temp_DS18B20(){

  resolution = 10;                 /* Resolution can be between 9 to 12 */
  lastTempRequest = 0;
  delayInMillis = 0;
  temperature = 0.0;
  idle = 0;
  
  // Allow for main program to continue after requesting a temperature conversion
  // from the temperature sensors
}

Temp_DS18B20::Temp_DS18B20(DallasTemperature * p_tmp_sensors){

  // Assign pointer to p_sensors property
  p_sensors = p_tmp_sensors;

  resolution = 10;                 /* Resolution can be between 9 to 12 */
  lastTempRequest = 0;
  delayInMillis = 0;
  temperature = 0.0;
  idle = 0;
  a = 2;

  // Allow for main program to continue after requesting a temperature conversion
  // from the temperature sensors
  p_sensors->setWaitForConversion(false);
}

void Temp_DS18B20::set_a(int tmp_a)
{
  a = tmp_a;
}

int Temp_DS18B20::get_a(void)
{
  return a;
}

/* Set the address to the specific device */
//void Temp_DS18B20::setTempAddress(DeviceAddress tempAddress)
void Temp_DS18B20::setTempAddress(uint8_t _tempAddressIn[])
{
 for(int i=0; i<8; i++){
   tempAddress[i] = _tempAddressIn[i]; 

 } 
}

/* Assign a pointer to a DallasTemperature Object, required for functionality */
void Temp_DS18B20::setDallasTemperature(DallasTemperature * p_tmp_sensors)
{
  // Assign pointer to p_sensors property
  p_sensors = p_tmp_sensors;
  // Allow for main program to continue after requesting a temperature conversion
  // from the temperature sensors
  p_sensors->setWaitForConversion(false);
}

/* Send a request to the devices on the One-Wire bus to begin acquiring a new temperature value */
void Temp_DS18B20::requestTemperatures(void){
  p_sensors->requestTemperatures();
}

void Temp_DS18B20::setResolution(int tmp_resolution)
{
  resolution = tmp_resolution;
  delayInMillis = 750 / (1 << (12 - resolution));
  p_sensors->setResolution(tempAddress, resolution);
}

/* Return pointer to temperature array */
float Temp_DS18B20::getTemp(void){
  //float temperature = p_sensors->getTempCByIndex(0);
  float temperature = p_sensors->getTempC(tempAddress);
  return temperature;
}

/* Check if enought time has passed so that the temperature conversion has
   had a chance to complete in the external sensor */
bool Temp_DS18B20::checkReadyTemp(void){
  bool chkRdy = p_sensors->isConversionAvailable(tempAddress);
  return chkRdy;
}

// function to print a device address
void Temp_DS18B20::printAddress(void)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (tempAddress[i] < 16) Serial.print("0");
    Serial.print(tempAddress[i], HEX);
  }
}

