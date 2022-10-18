
import serial
import sys
import crc

flipflop = serial.Serial('/dev/ttyUSB1', 9600, timeout=5)

flipflop.write(b'UU')
flipflop.read(2)
resp = flipflop.read(1)
if resp != b'K':
    flipflop.close()
    print('Failed to flipflop!')
    sys.exit(1)
print('Connected to flipflop')


#### Write Test ####
write_addr = 0x500
write_words = 999
write_bytes = write_words * 2
print(f"Writing { write_bytes } bytes to address: { hex(write_addr) }")
write_data = bytes(bytes.fromhex("12 da") * (write_bytes//2))
print(write_data.hex(" ", 2))
write_crc = crc.crc16(write_data)
print(f"CRC: {hex(write_crc)}")
flipflop.write(b'W') # Write command
flipflop.write(write_addr.to_bytes(2, 'big'))
flipflop.write(write_bytes.to_bytes(2, 'big'))
flipflop.read(5)

flipflop.write(write_data) # Write data
flipflop.read(write_bytes)

flipflop.write(write_crc.to_bytes(2, 'big')) # Write CRC
flipflop.read(2)

if flipflop.read() != b'K': # Should be K
    print("Write Failed!")
    flipflop.write(b'U')
    flipflop.close()
    sys.exit(1)
print(f"Successfully wrote {write_bytes} bytes\n")

#### Read Test ####
print(f"Reading { write_bytes } bytes from { hex(write_addr) }")
flipflop.write(b'R')
flipflop.write(write_addr.to_bytes(2, 'big'))
flipflop.write(write_bytes.to_bytes(2, 'big'))
flipflop.read(5)

resp = flipflop.read(write_bytes)
if resp == b'E':
    print("Read failed!\n")
    flipflop.write(b'U')
    flipflop.close()
    sys.exit(1)
print(f"Received { len(list(resp)) } bytes:\n{resp.hex(' ', -2)}")

crcread = flipflop.read(2)
print(f"CRC: 0x{crcread.hex()}\n")


msg = bytearray(resp)
msg.extend(crcread)
crccheck = crc.crc16(msg)
if (crccheck == 0):
    print(f"CRC OK: {hex(crccheck)}\n")
else:
    print(f"CRC ERROR: {hex(crccheck)}\n")

# Reset bootloader
flipflop.write(b'U')
flipflop.close()
