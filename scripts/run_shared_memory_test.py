#!/bin/env python3
from test_functions import run_shared_memory_test
import threading

AMOUNT_OF_TEST_ITERATIONS = 1

def thread_function():
    run_shared_memory_test()


def main():
    for _ in range(AMOUNT_OF_TEST_ITERATIONS):
        thread = threading.Thread(target=thread_function)
        thread.start()

        run_shared_memory_test()

        thread.join()


if __name__ == '__main__':
    main()
