#!/usr/bin/env python3
import argparse
import colorama
import importlib
import os
import re
import subprocess
import utils

### parse arguments
parser = argparse.ArgumentParser()
parser.add_argument( 'files', nargs='*', help="a list of C++ files" )
parser.add_argument( '--dry-run', action='store_true', help="don't actually update files" )
args = parser.parse_args()

### load config file if existing
git_toplevel = subprocess.check_output( [ 'git', 'rev-parse', '--show-toplevel' ] )
config_filename = git_toplevel.decode('utf-8').strip() + '/.c++clean'
if os.path.exists( config_filename ):
    with open( config_filename ) as infile:
        exec( infile.read() )

### initialize colors
colorama.init( autoreset = True )

for path in args.files:
    print( "process {}'{}'".format( colorama.Style.BRIGHT + colorama.Fore.BLUE, os.path.basename( path ) ) )
    if not os.path.isfile( path ):
        print( "File does not exist" )
    else:
        ### read file
        content = utils.read_file( path )
        old_content = '\n'.join( content )

        blocks = utils.find_metadata_blocks( content )
        print("metadata blocks:", blocks)

        for index in range(0, len(blocks)):
            block = blocks[index]

            ### copyright header
            m = utils.match_block( content, block, re.compile( copyright_header_mask.format( *copyright_header_regex ) ) )
            if ( m ):
                print( "matched copyright header:", m.groups(), end=' ' )

                assert len(m.groups()) == len(copyright_header_replace) 
                if list(m.groups()) != copyright_header_replace:
                    utils.replace_block( content, blocks, index, utils.string_remove_escape( copyright_header_mask ).format( *copyright_header_replace ).split( '\n' ) )
                    print( '[', colorama.Style.BRIGHT + colorama.Fore.BLUE + "UPDATE", ']' )
                else:
                    print( '[', colorama.Style.BRIGHT + colorama.Fore.GREEN + "OK", ']' )
                
            ### file and author information
            m = utils.match_block( content, block, re.compile( file_header_match_mask ) )
            if ( m ):
                print( "matched file and author information:", m.groups(), end=' ' )

                filename = m.groups()[0]
                description = m.groups()[1].strip()
                authors = re.findall( re.compile( author_match_mask ), m.groups()[2] )

                expected_filename = os.path.basename( path )
                expected_authors = utils.git_authors( path, author_pseudonyms )

                new_file_header = file_header_replace_mask.format( expected_filename, description,
                                                                   ''.join( [author_replace_mask.format( author ) for author in expected_authors] ) )
                new_file_header = utils.string_remove_escape( new_file_header )

                if ( '\n'.join( content[block[0]:block[1]] ) != new_file_header ):
                    utils.replace_block( content, blocks, index, new_file_header.split('\n') )
                    print( '[', colorama.Style.BRIGHT + colorama.Fore.BLUE + "UPDATE", ']' )
                else:
                    print( '[', colorama.Style.BRIGHT + colorama.Fore.GREEN + "OK", ']' )
            # else:
            #     expected_filename = os.path.basename( path )
            #     expected_authors = utils.git_authors( path, author_pseudonyms )
            # 
            #     new_file_header = file_header_replace_mask.format( expected_filename, "No description.",
            #                                                        ''.join( [author_replace_mask.format( author ) for author in expected_authors] ) )

        content = '\n'.join(content)

        ### rewrite the file
        if not args.dry_run and old_content != content:
            print( "create backup {}'{}~'".format( colorama.Style.BRIGHT + colorama.Fore.WHITE, os.path.basename( path ) ) )
            utils.write_file( path + "~", [ old_content ] )
        
            print( "update {}'{}'".format( colorama.Style.BRIGHT + colorama.Fore.WHITE, os.path.basename( path ) ) )
            utils.write_file( path, [ content ] )
