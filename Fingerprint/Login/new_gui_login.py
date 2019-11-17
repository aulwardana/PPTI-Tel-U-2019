import serial 
import tkinter as tk
from tkinter import messagebox
import time

port = '/dev/ttyUSB0'
ard = serial.Serial(port, 57600, timeout=1)

def command2():
    messagebox.showinfo('Finger system', 'Tekan OK, lalu letakkan jari anda!')
    inputs = 2
    ard.write(str(inputs).encode())
    while True:
        msg = ard.readline().decode('ascii')
        print(msg)
        time.sleep(5)
        messagebox.showinfo('finger system', 'Done')
        break
        

root = tk.Tk()
root.title("Finger System")
frame = tk.Frame(root)
frame.pack()

btn2 = tk.Button(frame,height=5, width=20,
                   text="Log in",
                   command=command2)
btn2.pack()


button = tk.Button(frame,height=5, width=20,  
                   text="QUIT", 
                   fg="red",
                   command=quit)
button.pack()
root.mainloop()

