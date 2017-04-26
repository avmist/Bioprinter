/* Temp_DS18B20
 * 2015 by Steven Lammers
 * Adapted from: Dallas Temperature Library: Example: WaitForConversion2.pde 
 *
 * This file is part of the Arduino SyringPump Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SyringePump Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef Temp_DS18B20_H
#define Temp_DS18B20_H
                                                               /* DEFINITIONS */
#define TEMP_DS18B20_VERSION "1.0"
#define ONE_WIRE_BUS 7//9                           /* Can be any digital pin */

                                                                  /* INCLUDES */
#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>
                                                          /* CLASS DEFINITION */
class Temp_DS18B20
{
  // pointer to Dallas Temperature object passed to constructor
  DallasTemperature * p_sensors;

  DeviceAddress tempAddress = {0, 0, 0, 0, 0, 0, 0, 0};
  //DeviceAddress syringeTempAddress = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};

  int  resolution;                 /* Resolution can be between 9 to 12 */
  unsigned long lastTempRequest;
  int  delayInMillis;
  float temperature;
  int  idle;
  int a;

  public:
  /* Constructors */
  Temp_DS18B20();
  Temp_DS18B20(DallasTemperature *);

  /* for Debugging */
  void set_a(int);
  int get_a(void);

  /* Set the address to the specific device */
  //void setTempAddress(DeviceAddress);
  void setTempAddress(uint8_t []);

  /* Assign a pointer to a DallasTemperature Object, required for functionality */
  void setDallasTemperature(DallasTemperature *);

  /* Send a request to the devices on the One-Wire bus to begin acquiring a new temperature value */
  void requestTemperatures(void);
  
  // Set resolution and calculate new delayInMillis
  void setResolution(int tmp_resolution);
  
  // Set device address
  // Physical addresses for one-wire temperature sensors
  // DeviceAddress tempDeviceAddress;
  void setDeviceAddress(DeviceAddress tempDeviceAddress);

  /* Return pointer to temperature array */
  float getTemp(void);

  /* Check if enought time has passed so that the temperature conversion has
     had a chance to complete in the external sensor */
  bool checkReadyTemp(void);
  
  void printAddress(void);
  

};
#endif                                       /* Close out define TempDS18B20.h */
