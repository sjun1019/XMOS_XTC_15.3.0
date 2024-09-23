#!/usr/bin/env python3

# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
CSV generator for OTP board info programming
"""

def xburn_encode(serial_number, mac_address, board_id, board_string):
	if mac_address is not None:
		assert len(mac_address) <= (1 << 3) - 1, 'Too many MAC addresses.'

		import re
		for i in range(len(mac_address)):
			mac_address[i] = re.split(':|-', mac_address[i])
			assert len(mac_address[i]) == 6, 'Invalid MAC address.'
			for j in range(len(mac_address[i])):
				try: mac_address[i][j] = int(mac_address[i][j], 16)
				except: assert False, 'Invalid MAC address.'
			mac_address[i].reverse()
			mac_address[i] = bytearray(mac_address[i])

	if board_string is not None:
		board_string = bytearray(board_string.encode('ascii'))
		board_string += b'\x00'

	words = 1
	if   mac_address is not None: words += 2 * len(mac_address)
	if serial_number is not None: words += 1
	if      board_id is not None: words += 1
	if  board_string is not None: words += (len(board_string) + 3) // 4

	if words & 1: words += 1
	assert words <= (1 << 5) - 1, 'Structure exceeds maximum size.'

	header = [0xFFFFFFFF]
	def xburn_set_header(value, shift, size):
		mask = (1 << size) - 1
		mask <<= shift
		value <<= shift
		header[0] ^= (value ^ header[0]) & mask

	xburn_set_header(0, 31, 1)
	xburn_set_header(words, 25, 5)
	xburn_set_header(len(mac_address) if mac_address   is not None else 0, 22, 3)
	xburn_set_header(               1 if serial_number is not None else 0, 21, 1)
	xburn_set_header(               1 if board_id      is not None else 0, 20, 1)
	xburn_set_header(               1 if board_string  is not None else 0, 19, 1)

	buffer = []
	for i in range(words):
		buffer.append(0 if i % 2 else 0xFFFFFFFF)
	buffer.reverse()
	buffer[-1] = header[0]

	import struct
	buffer = struct.pack('<{}I'.format(words), *buffer)

	pos = len(buffer) - 4

	if mac_address is not None:
		for i in range(len(mac_address)):
			pos -= 8
			buffer = buffer[:pos] + mac_address[i] + buffer[pos + 6:]

	if serial_number is not None:
		pos -= 4
		buffer = buffer[:pos] + struct.pack('<I', serial_number) + buffer[pos + 4:]

	if board_id is not None:
		pos -= 4
		buffer = buffer[:pos] + struct.pack('<I', board_id) + buffer[pos + 4:]

	if board_string is not None:
		for i in range(len(board_string)):
			pos -= 1
			buffer = buffer[:pos] + bytes(chr(board_string[i]).encode('ascii')) + buffer[pos + 1:]

	return buffer

def xburn_decode(buffer):
	array = []
	for i in range(0x800):
		array.append(0 if i % 2 else 0xFFFFFFFF)
	array.reverse()

	import struct
	array = struct.pack('<2048I', *array)
	pos = len(array) - len(buffer)
	array = array[:pos] + buffer
	array = struct.unpack('<2048I', array)

	for i in range(len(array)):
		if array[i] == (0xFFFFFFFF if i % 2 else 0): continue
		print('*,0x%03X,0x%08X' % (i, array[i]))

def main():
	import argparse

	parser = argparse.ArgumentParser(prog='XBurn_BoardInfo',
		description='CSV generator for OTP board info programming',
		formatter_class=argparse.RawTextHelpFormatter,
		epilog='For bug reporting instructions, please see:\nhttps://www.xmos.com/support')

	parser.add_argument('--serial-number', action='store',  metavar='arg', type=lambda x: int(x, 0), help='32-bit serial number')
	parser.add_argument('--mac-address',   action='append', metavar='arg', type=str,                 help='MAC address(es) (specified in the form 12:34:56:78:9A:BC)')
	parser.add_argument('--board-id',      action='store',  metavar='arg', type=lambda x: int(x, 0), help='32-bit board identifier')
	parser.add_argument('--board-string',  action='store',  metavar='arg', type=str,                 help='Board string')

	args = parser.parse_args()

	buffer = xburn_encode(**vars(args))
	xburn_decode(buffer)

	return 0

if __name__ == '__main__':
	import sys
	sys.exit(main())
