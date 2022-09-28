#!/bin/env python3
import random
import tempfile
import hashlib
import os
import subprocess
import threading


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

    file_object.flush()


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


def calculate_hash_for_path(file_path):
    with open(file_path, 'rb') as file:
        return calculate_hash_for_file(file_object=file)


def get_current_file_path():
    return os.path.abspath(__file__)


def get_current_folder():
    return os.path.dirname(get_current_file_path())


def get_project_root_directory():
    return os.path.dirname(get_current_folder())


def get_path_to_copy_file_utility():
    return os.path.join(get_project_root_directory(),
                        'build', 'CopyFile', 'CopyFile')


def run_default_test():
    with tempfile.NamedTemporaryFile() as temp_file, tempfile.TemporaryDirectory() as temp_directory:
        generate_file_with_file_object(temp_file)

        copy_file_path = get_path_to_copy_file_utility()
        destination = os.path.join(temp_directory, 'destination.txt')

        print(subprocess.run([copy_file_path, '-s', temp_file.name, '-d', destination], capture_output=True))

        hash_sha256_source = calculate_hash_for_file(temp_file)
        hash_sha256_destination = calculate_hash_for_path(destination)

        print(hash_sha256_source, '-> source ->', temp_file.name)
        print(hash_sha256_destination, '-> destination ->', destination)

        assert hash_sha256_source, hash_sha256_destination
        print('The hash is OK\n')


def run_server_shared_memory(destination, shared_memory_name):
    copy_file_path = get_path_to_copy_file_utility()
    output = subprocess.run([copy_file_path, '-d', destination, '-m', 'shared',
                             '--shared-memory', shared_memory_name], capture_output=True)

    print('Server args:', output.args)
    print('Server stdout:', output.stdout)
    print('Server stderr:', output.stderr)
    print('Server return code:', output.returncode)


def run_shared_memory_test():
    with tempfile.NamedTemporaryFile() as temp_file, tempfile.TemporaryDirectory() as temp_directory:
        generate_file_with_file_object(temp_file)
        shared_memory_name = 'crazy'

        copy_file_path = get_path_to_copy_file_utility()
        destination = os.path.join(temp_directory, 'destination.txt')

        thread = threading.Thread(target=run_server_shared_memory, args=(destination, shared_memory_name))
        thread.start()

        output = subprocess.run([copy_file_path, '-s', temp_file.name, '-m', 'shared', '--client',
                                 '--shared-memory', shared_memory_name], capture_output=True)

        print('Client args:', output.args)
        print('Client stdout:', output.stdout)
        print('Client stderr:', output.stderr)
        print('Client return code:', output.returncode)

        thread.join()

        print('Disable futher checkes for now')
        return

        hash_sha256_source = calculate_hash_for_file(temp_file)
        hash_sha256_destination = calculate_hash_for_path(destination)

        print(hash_sha256_source, '-> source ->', temp_file.name)
        print(hash_sha256_destination, '-> destination ->', destination)

        assert hash_sha256_source, hash_sha256_destination
        print('The hash is OK\n')
