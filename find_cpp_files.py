#!/usr/bin/env python3
import argparse
import os

extensions = ['.c', '.cpp', '.cxx', '.cc', '.C', '.CPP', '.c++', '.cp']


def check_cxx(file):
    for extension in extensions:
        if file.endswith(extension):
            return True
    return False


def go(cur, fr):
    for dr in os.listdir(cur):
        abs_path = os.path.join(cur, dr)
        if os.path.isdir(abs_path):
            go(abs_path, fr)
        elif check_cxx(dr):
            fr.write(abs_path + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dir', required=True, help="name of dir")
    args = parser.parse_args()
    rp = os.path.abspath(args.dir)
    print(rp)
    with open("files_for_ESCA.txt", "w") as fr:
        go(rp, fr)


if __name__ == '__main__':
    main()
