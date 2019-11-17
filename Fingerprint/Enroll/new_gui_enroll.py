import serial 
import tkinter as tk
from tkinter import messagebox
import time

port = '/dev/ttyUSB0'
ard = serial.Serial(port, 57600, timeout=1)

root = tk.Tk()
root.title("Enroll Finger")
frame = tk.Frame(root)
frame.pack()

def enroll():
    inputs = 1
    ard.write(str(inputs).encode())

enrollBtn = tk.Button(frame, height=5, width=20,
                    text="Start", command=enroll)
enrollBtn.pack()

large_font = ('Verdana',25)
medium_font = ('Verdana',15)

def validate_input(new_value):
    valid = len(new_value) <= 3
    return valid


validate = frame.register(validate_input)
for i in range(10):
    txt = tk.Entry(frame, font=large_font,width=15,validate="key", validatecommand=(validate, "%P"))

inputStr = txt
txt.pack()

def numInser(value):

    global pin

    if value == 'Del':
        pin = pin[:-1]
        txt.delete('0', 'end')
        txt.insert('end', pin)

    else:
        pin += value
        txt.insert('end', value)

    print("Current:", pin)

keys = [
    ['1', '2', '3'],    
    ['4', '5', '6'],    
    ['7', '8', '9'],    
    ['', '0','Del'],    
]

pin = ''

root = tk.Tk()
e = tk.Entry(root)
frame.grid(row=0, column=0, columnspan=3, ipady=5)

for y, row in enumerate(keys, 5):
    for x, key in enumerate(row):
        b = tk.Button(root, text=key, command=lambda val=key:numInser(val), width=10, font=medium_font)
        b.grid(row=y, column=x, ipadx=10, ipady=10)

def clicked():
    messagebox.showinfo('Finger system', 'Tekan OK, Lalu letakkan jari anda')
    res = txt.get()
    ard.write(str(res).encode())
    while True:
        msg = ard.readline().decode('ascii')
        print(msg)
        break
    time.sleep(8)
    messagebox.showinfo('Finger system', 'Done')
    

btn = tk.Button(frame, height=3, width=20,
                text="Enrollment", command=clicked)
btn.pack()
button = tk.Button(frame,height=5, width=20,  
                   text="QUIT", 
                   fg="red",
                   command=quit)
button.pack()
root.mainloop()
