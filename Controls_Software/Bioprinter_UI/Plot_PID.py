import serial
import numpy as np
from matplotlib import pyplot as plt
#ser = serial.Serial('/dev/ttyACM0', 9600)
ser = serial.Serial('/dev/cu.usbmodemfa131', 9600)
 
plt.ion() # set plot to animated
 
tempData = [0] * 60 * 3
pidData = [0] * 60 * 3
ax1=plt.axes()  
 
# make plot
plt.figure(1)
plt.subplot(211)
lineTemp, = plt.plot(tempData, 'bs')
plt.ylim([10,40])

plt.subplot(212)
linePID, =plt.plot(pidData, 'r--')
plt.ylim([0,255])
 
# start data collection
while True:  
    data = ser.readline().rstrip() # read data from serial 
                                   # port and strip line endings
    if len(data.split(",")) == 2:

        # Update tempData values
        plt.subplot(211)
        tempData.append(float(data.split(",")[0]))
        del tempData[0]
        ymin = float(min(tempData))-10
        ymax = float(max(tempData))+10
        plt.ylim([ymin,ymax])
        lineTemp.set_xdata(np.arange(len(tempData)))
        lineTemp.set_ydata(tempData)  # update the data
        print 'The value of tempData[end] is: ' + repr(tempData[-1])

        # Update pidData values
        plt.subplot(212)
        #ymin = float(min(pidData))-10
        pidData.append(float(data.split(",")[1]))
        del pidData[0]
        ymin = 0
        ymax = float(max(pidData))+10
        plt.ylim([ymin,ymax])
        linePID.set_xdata(np.arange(len(pidData)))
        linePID.set_ydata(pidData)  # update the data
        print 'The value of pidData[end] is: ' + repr(pidData[-1]) + ' ymin: ' + repr(ymin) + ' ymax: ' + repr(ymax)

        plt.draw() # update the plot
