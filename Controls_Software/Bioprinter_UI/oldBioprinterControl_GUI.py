#!/usr/bin/python3

import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from matplotlib import pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
from matplotlib.backend_bases import key_press_handler

import tkinter as tk
from tkinter import ttk

import serial
import numpy as np

# Start Serial
#ser = serial.Serial('/dev/cu.usbmodemfa131', 9600)

counter = 0
def counter_label(label):
  def count():
    global counter
    counter += 1
    label.config(text=str(counter))
    label.after(1000, count)
  count()

root = tk.Tk()
root.title("Bioprinter Controls")

f = plt.figure(figsize=(5,5), dpi=100)
#f = 1
subplotTop    = 211
subplotBottom = 212

'''
class PlotFrame():
  def __init__(self):
    plt.subplot(211)
    self.lineSyringe, = self.axes.plot(self.dataSyringe, 'rs', label='Syringe Temp')
    self.lineTube,    = self.axes.plot(self.dataTube,    'bs', label='Tube Temp')
    self.lineNeedle,  = self.axes.plot(self.dataNeedle,  'gs', label='Needle Temp')
    self.plt.ylim([20,45])
    self.plt.legend(loc='upper left')
'''

class PlotData():

  def __init__(self,plot_fig, subplot_axes):
    self.fig = plot_fig
    self.subplot = subplot_axes
    self.data = [0,0,0]
    self.dataSyringe = [0] * 60 * 3
    self.dataTube    = [0] * 60 * 3
    self.dataNeedle  = [0] * 60 * 3

    plt.subplot(self.subplot)
    self.lineSyringe, = plt.plot(self.dataSyringe, 'rs', label='Syringe Temp')
    self.lineTube,    = plt.plot(self.dataTube,    'bs', label='Tube Temp')
    self.lineNeedle,  = plt.plot(self.dataNeedle,  'gs', label='Needle Temp')
    plt.ylim([0,45])
    plt.legend(loc='upper left')
 
  def updatePlotVals(self):
    # Update tempData values
    plt.subplot(self.subplot)
    self.dataSyringe.append(float(self.data[0]))
    del self.dataSyringe[0]
    self.dataTube.append(float(self.data[1]))
    del self.dataTube[0]
    self.dataNeedle.append(float(self.data[2]))
    del self.dataNeedle[0]
    ymin = float(min(self.dataSyringe+self.dataTube+self.dataNeedle))-10
    ymax = float(max(self.dataSyringe+self.dataTube+self.dataNeedle))+10
    #plt.ylim([ymin,ymax])
    plt.ylim([0,45])
    self.lineSyringe.set_xdata(np.arange(len(self.dataSyringe)))
    self.lineSyringe.set_ydata(self.dataSyringe)  # update the data
    self.lineTube.set_xdata(np.arange(len(self.dataTube)))
    self.lineTube.set_ydata(self.dataTube)  # update the data
    self.lineNeedle.set_xdata(np.arange(len(self.dataNeedle)))
    self.lineNeedle.set_ydata(self.dataNeedle)  # update the data
    #print 'The value of self.dataSyringe[end] is: ' + repr(self.dataSyringe[-1])
    #print 'The value of self.dataTube[end] is: ' + repr(self.dataTube[-1])
    #print 'The value of self.dataNeedle[end] is: ' + repr(self.dataNeedle[-1])


canvas = FigureCanvasTkAgg(f, master=root)
canvas.show()
canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

toolbar = NavigationToolbar2TkAgg(canvas, root)
toolbar.update()
canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)


# Code to draw the plot
#def animatePlot(i, ser):
def animatePlot(frameNum):

  # If the serial line is recognized as valid, update plot values and redraw plot
  #serData = ser.readline().rstrip() # read data from serial 
                                 # format should be [tempSyringe,pidSyringe,tempTube,pidTube,tempNeedle,pidNeedle]
                                 # port and strip line endings
  serData="10,12,14,20,22,24"                               
  if len(serData.split(",")) == 6:
    temperaturePlot.data = serData.split(",")[::2] # even vals
    temperaturePlot.updatePlotVals()
    
    pidPlot.data = serData.split(",")[1::2] # odd vals
    pidPlot.updatePlotVals()

    plt.draw() # update the plot

# Instantiate plot objects
temperaturePlot = PlotData(f, subplotTop)
pidPlot = PlotData(f, subplotBottom)

#ani = animation.FuncAnimation(f, animatePlot(ser), interval=1000)
ani = animation.FuncAnimation(f, animatePlot, interval=1000)

button = tk.Button(root, text="Stop", width=25, command=root.destroy)
button.pack()
root.mainloop()
