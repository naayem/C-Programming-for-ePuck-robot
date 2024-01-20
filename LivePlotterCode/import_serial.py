import serial
import numpy as np
import matplotlib.pyplot as plt
import liveplotter as lp

with serial.Serial('/dev/cu.e-puck2_04020-UART', 19200, timeout=1) as ser:
    x = ser.read()          # read one byte
    s = ser.read(10)        # read up to ten bytes (timeout)

       # read a '\n' terminated line
    counter = 0
    all_values = []
    l=ser.readline().decode("utf-8")
    line1 = []
    line2 = []
    Ax = np.array([0 for _ in range(200)])
    Ay = np.array([0 for _ in range(200)])
    while(True):
        l=ser.readline().decode("utf-8")
        print(l)
        if counter%2:
            all_values.append([float(x[3:]) for x in l.split(' ')[:-1]])
            print(np.array(all_values).shape)
            Ax[-1] = np.array(all_values)[-1,0]
            Ay[-1] = np.array(all_values)[-1,1]
            print(counter%5)
            if not counter%5-1:
                line1 = lp.live_plotter(np.array(range(200)),Ax,line1)
                line2 = lp.live_plotter(np.array(range(200)),Ay,line2)
            Ax = np.append(Ax[1:],0.0)
            Ay = np.append(Ay[1:],0.0)

        counter+=1

        





