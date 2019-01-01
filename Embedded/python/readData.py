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

    if line != "end\n":
        time = line.split(", ")[0]
        voltage = line.split(", ")[1]
        radian = line.split(", ")[2]
        new = str(time)+ ", " + str(voltage)+ ", " + str( radian)
        data += (str(new) + "\n")
    else:
        f = open("logged_data.csv", "w")
        f.write(data)
        f.close()
        data = ''
        print("File saved")
        break

ser.close()
