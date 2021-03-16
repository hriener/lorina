import re
import subprocess

class match_replace_mask:
    def __init__( self, mask, matches, replaces ):
        assert( len( matches ) == len( replaces ) )
        self.mask = mask
        self.matches = matches
        self.replaces = replaces

        self.pattern = re.compile( mask.format( *matches ) )
        self.replacement = utils.string_remove_escape( mask.format( *replaces ) );

    def apply( self, content, args = [] ):
        if ( args == [] ):
            return utils.match_replace( content, self.pattern, self.replacement )
        else:
            return utils.match_replace( content, self.pattern, self.replacement.format( *args ) )

#---------------------------------------------------------------------------
def read_file( filename ):
    with open( filename, 'r' ) as f:
        content = f.read().splitlines()
    return content

#---------------------------------------------------------------------------
def write_file( filename, content ):
    with open( filename, "w" ) as f:
        f.write( '\n'.join( content ) )

#---------------------------------------------------------------------------
def string_remove_escape( string ):
    new_string = '';
    for i in range(0, len(string)):
        if string[i:i+2] in [ r'\*', r'\(', r'\)', r'\+', r'\!' ]:
            continue
        else:
            new_string = new_string + string[i]
    return new_string

#---------------------------------------------------------------------------
def git_authors( file, author_pseudonyms ):
    result = subprocess.check_output(['git', 'shortlog', '-s', '--', file])
    authors = []
    lines = result.decode('utf-8').split('\n')
    for l in lines:
        author_information = l.split( '\t' )
        if len( author_information ) > 1:
            name = author_information[1]
            if ( name in author_pseudonyms ):
                authors.append( author_pseudonyms[name] )
            else:
                authors.append( name )

    # remove duplicate names
    authors = list( dict.fromkeys( authors ) )
    return sorted( authors )

#---------------------------------------------------------------------------
def match_replace( content, pattern, replace ):
    found = False
    for match in pattern.finditer( content ):
        found = True
        content = ''.join( [content[:match.start()], replace, content[match.end():]] )
    return found, content

#---------------------------------------------------------------------------
def find_metadata_blocks( content ):
    blocks = []
    left = 0
    right = 0
    for i in range(0, len(content)):
        line = content[i].strip()
        if line.startswith( '/*' ):
            left = i
        elif line.startswith( '*/' ):
            right = i
            blocks = blocks + [ ( left, right+1 ) ]
    return blocks

#---------------------------------------------------------------------------
def print_block( content, block ):
    for i in range( block[0], block[1] ):
        print( content[i] )

#---------------------------------------------------------------------------
def match_block( content, block, pattern ):
    result = re.match( pattern, '\n'.join(content[block[0]:block[1]]) )
    return result

#---------------------------------------------------------------------------
def update_blocks_by_offset( blocks, index, offset ):
    for i in range(index, len(blocks)):
        block = blocks[i]
        blocks[i] = tuple( ( block[0] + offset, block[1] + offset) )

#---------------------------------------------------------------------------
def replace_block( content, blocks, index, replacement ):
    block = blocks[index]
    offset = len(replacement) - ( block[1] - block[0] )

    for i in range( block[0], min( block[1], block[0] + len(replacement) ) ):
        content[i] = replacement[i - block[0]]

    # replacement is longer than the original block
    if ( offset > 0 ):
        content[block[1]:block[1]] = replacement[block[1] - block[0]:]

    # replacement is shorter than the original block
    elif ( offset < 0 ):
        for i in range( -offset ):
            del content[block[1] - i - 1]

    update_blocks_by_offset( blocks, index, offset )
