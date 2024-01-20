from threading import Thread
import cv2

import socket
import numpy as np
import pickle
import struct
import requests


###### Classes for video streaming ##############################################################################

class DataStream:
    def __init__(self, ser):
        """initialize the ePuck mapping stream and read the first frame form the stream"""
        self.ser = ser
        self.line = ''
        self.x, self.y, self.angle = 0.0, 0.0, 0.0
        # initialize the variable used to indicate if the thread should
		# be stopped
        self.stopped = False
    
    def start(self):
        """start the thread to read frames from the video stream"""
        print("we are starting the thread")
        Thread(target=self.update, args=()).start()
        return self
        
    def update(self):
        """keep looping infinitely until the thread is stopped"""
        while True:
            # if the thread indicator variable is set, stop the thread
            if self.stopped:
                return
            # otherwise, read the next frame from the stream
            
            self.line = self.ser.readline().decode("utf-8")
            self.process()
    
    def read(self):
        """return the frame most recently read"""
        return self.x, self.y, self.angle

    def process(self):
        l = self.line
        
        if l[:6] == 'update':
            try:
                self.x = float(l[l.find('x')+2:l.find('x')+8])
                self.y = float(l[l.find('y')+2:l.find('y')+8])
                self.angle = float(l[l.find('l')+3:l.find('l')+9])
            except:
                print('corrupted data')
    
    def stop(self):
        """indicate that the thread should be stopped"""
        self.stopped = True