import logging
import sys
import time
import codecs
import serial
import struct
import SocketServer 

from wheeledChassis import Chassis
from socket import *
SYNC_BYTE = b'\xA5'
SYNC_BYTE2 = b'\x5A'
HEADER_BYTE = b'\xFF'
MOVE_LEN =15
def send_move(v,w):
        '''Sends `cmd` command to the sensor'''
        Len=struct.pack('B', MOVE_LEN)
        x_speed =struct.pack('<f', v)
        y_speed =struct.pack('<f', 0.0)
        ang_speed =struct.pack('<f', w)
        #print('Command sent: %x' %x_speed)
        Xspeed = struct.unpack('<I', x_speed)
        print('Command sent: %s' %x_speed)
        #Yspeed = struct.pack('B', y_speed)
        #Aspeed = struct.pack('B', ang_speed)
        req =  Len +x_speed+y_speed+ang_speed
        checksum = 0
        for v in struct.unpack('B'*len(req), req):
           checksum ^= v
        req += struct.pack('B', checksum)
        payload = HEADER_BYTE+HEADER_BYTE+req
        #self._serial_port.write(payload)
        #self.logger.debug('Command sent: %s' % req)
        #print('Command sent: %c' % payload[0])
        for i in payload:
            print('%#x'%ord(i))
		
if __name__ == '__main__':
    chassis = Chassis('/dev/ttyUSB0')
    #res =(chassis._read_response(15))
    #send_move(0.1,0.0)
    chassis.send_move(0.2,0.0)