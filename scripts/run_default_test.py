#!/bin/env python3
from test_functions import run_default_test

AMOUNT_OF_TEST_ITERATIONS = 5


def main():
    for _ in range(AMOUNT_OF_TEST_ITERATIONS):
        run_default_test()


if __name__ == '__main__':
    main()
