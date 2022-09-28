#!/bin/env python3
from test_functions import run_shared_memory_test

AMOUNT_OF_TEST_ITERATIONS = 1


def main():
    for _ in range(AMOUNT_OF_TEST_ITERATIONS):
        run_shared_memory_test()


if __name__ == '__main__':
    main()
