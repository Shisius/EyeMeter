import serial
import sys
import time

if __name__ == "__main__":
	s = serial.Serial('/dev/ttyACM0')
	written = True
	if len(sys.argv) > 1:
		if sys.argv[1] == 'on':
			s.write(b'\x0f')
		elif sys.argv[1] == 'off':
			s.write(b'\x00')
		elif sys.argv[1] == 'disp':
			s.write(b'\x40')
			time.sleep(0.5)
		else:
			print("Unknown cmd")
			written = False
		print(s.read(1))
	else:
		print('usage: on|off ')
	s.close()

