#!/bin/env python3
import random

CHUNK_LOW_BOUNDARY = 1000
CHUNK_UPPER_BOUNDARY = 2000

FILE_ITERATIONS_LOW_BOUNDARY = 100
FILE_ITERATIONS_UPPER_BOUNDARY = 300

MAX_UNICODE_UTF8 = 1112064


def generate_chunk():
    size = random.randrange(CHUNK_LOW_BOUNDARY, CHUNK_UPPER_BOUNDARY)
    numbers = list(bytes(str(2 ** size), 'utf-8'))

    random.shuffle(numbers)
    multiplied_numbers = [(x * y % MAX_UNICODE_UTF8) for (x, y) in zip(numbers, range(size))]
    random.shuffle(multiplied_numbers)

    array_symbols = [chr(array_byte) for array_byte in multiplied_numbers]
    return bytes(''.join(array_symbols), 'utf-8')


def generate_file():
    with open('generated.txt', 'wb') as file:
        for _ in range(random.randrange(FILE_ITERATIONS_LOW_BOUNDARY, FILE_ITERATIONS_UPPER_BOUNDARY)):
            generated_chunk = generate_chunk()
            file.write(generated_chunk)


def main():
    generate_file()


if __name__ == '__main__':
    main()
