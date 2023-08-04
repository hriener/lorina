#!/usr/bin/env python3

from pathlib import Path
import fileinput
import os
import re
import argparse

colorful = True

COL_RESET     = "\033[0m"
COL_WHITE_FG  = "\033[01;37m"
COL_YELLOW_FG = "\033[01;33m"
COL_RED_BG    = "\033[01;41m"
COL_YELLOW_BG = "\033[01;43m"

def report(line_old, line_new):
    if colorful:
        print(COL_YELLOW_FG + "[i] found a potential trailing whitespace:")
        print(COL_YELLOW_FG + line_old + COL_RED_BG + "$" + COL_RESET)
        print(COL_YELLOW_FG + line_new + COL_RED_BG + "$" + COL_RESET)

    else:
        print("[i] found a potential inconsistency:")
        print(line_old + "$")
        print(line_new + "$")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--extensions', nargs='+', default=['.h', '.hh', '.hpp', '.c' ,'.cc', '.cpp', '.inc', '.aag', '.rst', '.py', '.txt'],
                        help="specify a list of file extensions to parse (default='.h .hh .hpp .c .cc .cpp .inc .aag .rst .py .txt')")
    parser.add_argument('-u', '--update_files', action='store_true',
                        help='update files in inplace (default=false)')
    parser.add_argument('-c', '--colors',       action='store_false',
                        help='toggle output colors (default=true)')

    args = parser.parse_args()
    colorful = args.colors
    print(args.colors)
    
    for root, subdirs, files in os.walk(os.getcwd()):
        for filename in files:
            file_path = os.path.join(root, filename)

            if Path(file_path).suffix in args.extensions:
                print('process...', file_path)

                update = False
                with open(file_path, 'r') as f:
                    for line in f:
                        stripped = line.rstrip()
                        if line != line.rstrip() + '\n':
                            update = True
                            report(line, stripped)


                if args.update_files and update:
                    for line in fileinput.input(files=(file_path), inplace=True, backup='.bak'):
                        stripped = line.rstrip()
                        if line != line.rstrip() + '\n':
                            print(line.rstrip())
                        else:
                            print(line, end='')
