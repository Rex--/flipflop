
import serial
import sys

flipflop = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)

flipflop.write(b'U')
resp = flipflop.read(1)
if resp != b'K':
    flipflop.close()
    print('Failed to flipflop!')
    sys.exit(1)
print('Connected to flipflop')

print('Flipping byte...', end=' ')
flipflop.write(b'S')
flipflop.write(bytes.fromhex('01 BE'))

resp = flipflop.read(1)
if resp != b'K':
    flipflop.close()
    print('\nFailed to flip word!')
    sys.exit(1)
print('success')

print('Reading back byte...', end=' ')
flipflop.write(b'G')
flipflop.write(bytes.fromhex('01'))
resp = flipflop.read()
print('0x' + resp.hex())

if resp != bytes.fromhex('BE'):
    flipflop.close()
    print('\nFailed to read word!')
    sys.exit(1)
print('Byte verified')

print('Reading application reset vector...', end=' ')
flipflop.write(b'R')
flipflop.write(bytes.fromhex('08 00'))
resp = flipflop.read(2)
print ('0x' + resp.hex())

print('Reading bootloader reset vector...', end=' ')
flipflop.write(b'R')
flipflop.write(bytes.fromhex('00 00'))
resp = flipflop.read(2)
print ('0x' + resp.hex())

print('Writing row of flash...', end=' ')
flipflop.write(b'P')
flipflop.write(bytes.fromhex('20 00'))
for i in range(64):
    flipflop.write(bytes.fromhex('0d bf'))
resp = flipflop.read(1)
print('success')

input('Press [Enter] to launch user app')
print('Starting user app')
flipflop.write(b'X')

flipflop.close()
