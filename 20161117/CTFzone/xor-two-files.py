import sys

src_data = bytearray(open(sys.argv[1], 'rb').read())
xor_data = bytearray(open(sys.argv[2], 'rb').read())

for i in range(len(src_data)):
	src_data[i] ^= xor_data[i % len(xor_data)]

sys.stdout.buffer.write(src_data)