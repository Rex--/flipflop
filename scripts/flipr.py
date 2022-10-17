
import serial
import sys

flipflop = serial.Serial('/dev/ttyUSB1', 500000, timeout=2)

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
write_words = 1000
write_bytes = write_words * 2
print(f"Writing { write_bytes } bytes to address: { hex(write_addr) }")
write_data = bytes([17, 34] * (write_bytes//2))
print(write_data.hex(" ", 2))
flipflop.write(b'W') # Write command
flipflop.write(write_addr.to_bytes(2, 'big'))
flipflop.write(write_bytes.to_bytes(2, 'big'))
flipflop.read(5)

flipflop.write(write_data) # Write data
flipflop.read(write_bytes)

if flipflop.read() != b'K': # Should be K
    print("Write Failed!")
    flipflop.write(b'U')
    flipflop.close()
    sys.exit(1)
print(f"Successfully wrote {write_bytes} bytes\n")

#### Read Test ####
write_bytes +=2
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
print(f"Received { len(list(resp)) } bytes:\n{resp.hex(' ', -2)}\n")


# Reset bootloader
flipflop.write(b'U')
flipflop.close()
