#!/bin/env python3
import random
import tempfile
import hashlib

CHUNK_LOW_BOUNDARY = 1000
CHUNK_UPPER_BOUNDARY = 2000

FILE_ITERATIONS_LOW_BOUNDARY = 100
FILE_ITERATIONS_UPPER_BOUNDARY = 300

MAX_UNICODE_UTF8 = 1112064
READ_CHUNK_SIZE = 4096


def _generate_chunk():
    size = random.randrange(CHUNK_LOW_BOUNDARY, CHUNK_UPPER_BOUNDARY)
    numbers = list(bytes(str(2 ** size), 'utf-8'))

    random.shuffle(numbers)
    multiplied_numbers = [(x * y % MAX_UNICODE_UTF8) for (x, y) in zip(numbers, range(size))]
    random.shuffle(multiplied_numbers)

    array_symbols = [chr(array_byte) for array_byte in multiplied_numbers]
    return bytes(''.join(array_symbols), 'utf-8')


def generate_file_with_file_object(file_object):
    for _ in range(random.randrange(FILE_ITERATIONS_LOW_BOUNDARY, FILE_ITERATIONS_UPPER_BOUNDARY)):
        generated_chunk = _generate_chunk()
        file_object.write(generated_chunk)


def generate_file(file_name):
    with open(file_name, 'wb') as file:
        generate_file_with_file_object(file)


def calculate_hash_for_file(file_object):
    sha256_hash = hashlib.sha256()
    file_object.seek(0)

    # Read and update hash string value in blocks of 4K
    for byte_block in iter(lambda: file_object.read(READ_CHUNK_SIZE), b''):
        sha256_hash.update(byte_block)

    return sha256_hash.hexdigest()


def main():
    with tempfile.NamedTemporaryFile() as temp_file:
        generate_file_with_file_object(temp_file)
        hash_sha256 = calculate_hash_for_file(temp_file)
        print(hash_sha256)


if __name__ == '__main__':
    main()
