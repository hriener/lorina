#!/usr/bin/env python3
import os
import fileinput
import re

header = """/\* lorina: C\+\+ parsing library
 \* Copyright \(C\) ([\d-]+)  EPFL
 \*
 \* Permission is hereby granted, free of charge, to any person
 \* obtaining a copy of this software and associated documentation
 \* files \(the \"Software\"\), to deal in the Software without
 \* restriction, including without limitation the rights to use,
 \* copy, modify, merge, publish, distribute, sublicense, and/or sell
 \* copies of the Software, and to permit persons to whom the
 \* Software is furnished to do so, subject to the following
 \* conditions:
 \*
 \* The above copyright notice and this permission notice shall be
 \* included in all copies or substantial portions of the Software.
 \*
 \* THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,
 \* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 \* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 \* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 \* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 \* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 \* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 \* OTHER DEALINGS IN THE SOFTWARE.
 \*/
"""

header_replace = """/* lorina: C++ parsing library
 * Copyright (C) 2018-2021  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
"""

def find_files( path, ext ):
    items = []
    for root, dirs, files in os.walk( path ):
        for name in files:
            if name.endswith( ext ):
                items.append( os.path.join( root, name ) )
    return items

def read_file( filename ):
    with open( filename, "r" ) as f:
        content = f.read().splitlines()
    return content

def write_file( filename, content ):
    with open( filename, "w" ) as f:
        f.write( '\n'.join( content ) )

def match_replace( content, pattern, replace ):
    found = False
    for match in pattern.finditer( content ):
        found = True
        content = ''.join( [content[:match.start()], replace, content[match.end():]] )
    return found, content

# find all `.hpp` files
hpp_files = find_files( '.', '.hpp' )

header_pattern = re.compile( header )
for file in hpp_files:
    content = read_file( file )
    content = '\n'.join( content )
    new_content = content

    # update file header if necessary
    header_found, new_content = match_replace( new_content, header_pattern, header_replace )

    # add file header if not found
    if not header_found:
        new_content = ''.join( [ header_replace, new_content ] )

    # update the file if it exists
    if ( content != new_content ):
        print( "create backup ", file + "~" )
        write_file( file + "~", [ content ] )

        print( "update file ", file )
        write_file( file, [ new_content ] )
