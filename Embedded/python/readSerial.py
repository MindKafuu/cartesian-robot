import serial

BAUDRATE = 115200
PORT = 'COM17'

ser = serial.Serial()
ser.baudrate = BAUDRATE
ser.port = PORT
ser.timeout = 1
ser.dtr = 0
ser.rts = 0
ser.open()
data = ""
print("connected to: " + ser.portstr)
while True:
    line = ser.readline().decode("utf-8")
    print(line)

ser.close()
