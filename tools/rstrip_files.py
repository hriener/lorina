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
COL_GREEN_BG  = "\033[01;42m"
COL_YELLOW_BG = "\033[01;43m"

#===========================================================================

def format_char(color, prefix, c):
    if colorful:
        return color + '{}{{{}}}'.format(prefix, c.encode('unicode_escape').decode("utf-8")) + COL_RESET
    else:
        return '{}{{{}}}'.format(prefix, c.encode('unicode_escape').decode("utf-8"))


def lcs(u, v):
    if len(u) > 0 and len(v) > 0:
        if u[0] == v[0]:
            return u[0] + lcs(u[1:], v[1:])

        a = lcs(u, v[1:])
        b = lcs(u[1:], v)

        return a if len(a) > len(b) else b
    else:
        return ''

# Returns an explanation how the string u can by modified to become
# identical to v, (e.g. for 'Hello' and 'Hellwo', the procedure
# returns 'Hell+{w}o').
def str_diff(u, v):
    result = ''

    r = lcs(u, v)
    i = 0
    j = 0
    for a in r:
        for b in v[i:]:
            i = i + 1
            if a == b:
                break
            result = result + format_char(COL_GREEN_BG, '+', b)

        for b in u[j:]:
            j = j + 1
            if a == b:
                break
            result = result + format_char(COL_RED_BG, '-', b)

        result = result + a

    for b in v[i:]:
        i = i + 1
        result = result + format_char(COL_GREEN_BG, '+', b)

    for b in u[j:]:
        j = j + 1
        result = result + format_char(COL_RED_BG, '-', b)

    return result

# ===========================================================================

def report(counter, line_old, line_new):
    if colorful:
        print(COL_YELLOW_FG + "[i] found a potential inconsistency in line {:d}:".format(counter) + COL_RESET)
        print('A:', line_old + COL_RED_BG + "$" + COL_RESET)
        print('B:', line_new + COL_RED_BG + "$" + COL_RESET)
        print('D:', str_diff(line_old, line_new))
    else:
        print("[i] found a potential inconsistency {:d}:".format(counter))
        print('A:', line_old + "$")
        print('B:', line_new + "$")
        print('D:', str_diff(line_old, line_new))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--extensions', nargs='+', default=['.h', '.hh', '.hpp', '.c' ,'.cc', '.cpp', '.inc', '.aag', '.rst', '.py', '.txt', '.md', '.yml'],
                        help="specify a list of file extensions to parse (default='.h' '.hh' '.hpp' '.c' '.cc' '.cpp' '.inc' '.aag' '.rst' '.py' '.txt' '.md' '.yml')")
    parser.add_argument('-u', '--update_files', action='store_true',
                        help='update files in inplace (default=false)')
    parser.add_argument('-c', '--colors',       action='store_false',
                        help='toggle output colors (default=true)')

    args = parser.parse_args()
    colorful = args.colors

    for root, subdirs, files in os.walk(os.getcwd()):
        for filename in files:
            file_path = os.path.join(root, filename)

            if Path(file_path).suffix in args.extensions:
                print('[i] process...', file_path)

                update = False
                line_counter = 1 # use line numbers started from 1 as in EMACS
                with open(file_path, 'r') as f:
                    for line in f:
                        stripped = line.rstrip()
                        if line != line.rstrip() + '\n':
                            update = True
                            report(line_counter, line, stripped)
                        line_counter = line_counter + 1

                if args.update_files and update:
                    for line in fileinput.input(files=(file_path), inplace=True, backup='.bak'):
                        stripped = line.rstrip()
                        if line != line.rstrip() + '\n':
                            print(line.rstrip())
                        else:
                            print(line, end='')
