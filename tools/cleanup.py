#!/usr/bin/env python3
import os
import fileinput
import re

def find_files( path, ext ):
    items = []
    for root, dirs, files in os.walk( path ):
        for name in files:
            if name.endswith( ext ):
                items.append( os.path.join( root, name ) )
    return items

# find all `.hpp` files
hpp_files = find_files( '.', '.hpp' )

# update copyright string
for file in hpp_files:
    print("processing ", file)
    with fileinput.FileInput(file, inplace=True, backup='~') as file:
        for line in file:
            print( re.sub( r'\* Copyright \(C\) [\d-]+', '* Copyright (C) 2017-2021', line ), end='' )
