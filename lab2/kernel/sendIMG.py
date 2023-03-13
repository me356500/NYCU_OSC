import serial
import os,sys
import time



tty = serial.Serial(s, 115200, timeout=0.5) 
file_stats = os.stat(f)

# issue request and tell the size of img to rec
tty.write(str(file_stats.st_size).encode('utf-8'))

# send img byte-by-byte
# delay to ensure no loss
with open(f, "rb") as fp:
    byte = fp.read(1)
    while byte:
        tty.write(byte)
        byte = fp.read(1)
        time.sleep(1)