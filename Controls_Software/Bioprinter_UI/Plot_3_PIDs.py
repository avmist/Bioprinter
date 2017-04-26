import serial
import numpy as np
from matplotlib import pyplot as plt
#ser = serial.Serial('/dev/ttyACM0', 9600)
ser = serial.Serial('/dev/cu.usbmodemfa131', 9600)
 
plt.ion() # set plot to animated
 
tempDataSyringe = [0] * 60 * 3
tempDataTube    = [0] * 60 * 3
tempDataNeedle  = [0] * 60 * 3
pidDataSyringe  = [0] * 60 * 3
pidDataTube     = [0] * 60 * 3
pidDataNeedle   = [0] * 60 * 3
ax1=plt.axes()  
 
# make plot
plt.figure(1)
plt.subplot(211)
lineTempSyringe, = plt.plot(tempDataSyringe, 'rs', label='Syringe Temp')
lineTempTube   , = plt.plot(tempDataTube, 'bs', label='Tube Temp')
lineTempNeedle , = plt.plot(tempDataNeedle, 'gs', label='Needle Temp')
plt.ylim([20,45])
plt.legend(loc='upper left')

plt.subplot(212)
linePIDSyringe, =plt.plot(pidDataSyringe, 'r--', label='Syringe PID')
linePIDTube   , =plt.plot(pidDataTube, 'b--', label='Tube PID')
linePIDNeedle , =plt.plot(pidDataNeedle, 'g--', label='Needle PID')
plt.ylim([0,255])
plt.legend(loc='upper left')
 
# start data collection
while True:  
    data = ser.readline().rstrip() # read data from serial 
                                   # format should be [tempSyringe,pidSyringe,tempTube,pidTube,tempNeedle,pidNeedle]
                                   # port and strip line endings
    if len(data.split(",")) == 6:

        # Update tempData values
        plt.subplot(211)
        tempDataSyringe.append(float(data.split(",")[0]))
        del tempDataSyringe[0]
        tempDataTube.append(float(data.split(",")[2]))
        del tempDataTube[0]
        tempDataNeedle.append(float(data.split(",")[4]))
        del tempDataNeedle[0]
        ymin = float(min(tempDataSyringe+tempDataTube+tempDataNeedle))-10
        ymax = float(max(tempDataSyringe+tempDataTube+tempDataNeedle))+10
        #plt.ylim([ymin,ymax])
        plt.ylim([20,45])
        lineTempSyringe.set_xdata(np.arange(len(tempDataSyringe)))
        lineTempSyringe.set_ydata(tempDataSyringe)  # update the data
        lineTempTube.set_xdata(np.arange(len(tempDataTube)))
        lineTempTube.set_ydata(tempDataTube)  # update the data
        lineTempNeedle.set_xdata(np.arange(len(tempDataNeedle)))
        lineTempNeedle.set_ydata(tempDataNeedle)  # update the data
        print 'The value of tempDataSyringe[end] is: ' + repr(tempDataSyringe[-1])
        print 'The value of tempDataTube[end] is: ' + repr(tempDataTube[-1])
        print 'The value of tempDataNeedle[end] is: ' + repr(tempDataNeedle[-1])

        # Update pidData values
        plt.subplot(212)
        #ymin = float(min(pidData))-10
        pidDataSyringe.append(float(data.split(",")[1]))
        del pidDataSyringe[0]
        pidDataTube.append(float(data.split(",")[3]))
        del pidDataTube[0]
        pidDataNeedle.append(float(data.split(",")[5]))
        del pidDataNeedle[0]
        ymin = 0
        ymax = float(max(pidDataSyringe+pidDataTube+pidDataNeedle))+10
        plt.ylim([ymin,ymax])
        plt.ylim([0,260])
        linePIDSyringe.set_xdata(np.arange(len(pidDataSyringe)))
        linePIDSyringe.set_ydata(pidDataSyringe)  # update the data
        linePIDTube.set_xdata(np.arange(len(pidDataTube)))
        linePIDTube.set_ydata(pidDataTube)  # update the data
        linePIDNeedle.set_xdata(np.arange(len(pidDataNeedle)))
        linePIDNeedle.set_ydata(pidDataNeedle)  # update the data
        print 'The value of pidDataSyringe[end] is: ' + repr(pidDataSyringe[-1]) + ' ymin: ' + repr(ymin) + ' ymax: ' + repr(ymax)
        print 'The value of pidDataTube[end] is: ' + repr(pidDataTube[-1]) + ' ymin: ' + repr(ymin) + ' ymax: ' + repr(ymax)
        print 'The value of pidDataNeedle[end] is: ' + repr(pidDataNeedle[-1]) + ' ymin: ' + repr(ymin) + ' ymax: ' + repr(ymax)

        plt.draw() # update the plot
