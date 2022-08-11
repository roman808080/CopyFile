#!/bin/env python3
import hashlib
import sys

BLOCK_SIZE = 1024


def calculate_for_stream(stream):
    while True:
        data = stream.read(BLOCK_SIZE)
        if len(data) == 0:
            break

        md5_returned = hashlib.md5(data).hexdigest()

        print(md5_returned)


def calculate_hashes(file_name):
    with open(file_name, 'rb') as file_to_check:
        calculate_for_stream(stream=file_to_check)


if __name__ == '__main__':
    file_name = sys.argv[1]
    calculate_hashes(file_name)
