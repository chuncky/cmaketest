#!/usr/bin/env python

from __future__ import print_function
from sys import argv, exit, stderr
from argparse import ArgumentParser, FileType, Action
from os import fstat
from struct import pack
from hashlib import sha1
import sys, os
import re

first_offset = 0
second_offset = 0
third_offset = 0
fourth_offset = 0
file_list_auto_number = 0

def filesize(f):
    if f is None:
        return 0
    try:
        return fstat(f.fileno()).st_size
    except OSError:
        return 0


def fileofst(args, f):
    if f is None:
        return 0
    else:
        return (int((filesize(f) + args.pagesize - 1) / args.pagesize) * args.pagesize)


def update_sha(sha, f):
    if f:
        sha.update(f.read())
        f.seek(0)
        sha.update(pack('I', filesize(f)))
    else:
        sha.update(pack('I', 0))


def pad_file(f, padding):
    pad = (padding - (f.tell() & (padding - 1))) & (padding - 1)
    f.write(pack(str(pad) + 'x'))

def auto_list_num(args):
    global file_list_auto_number
    list_dir=args.autolist_dir
    if os.path.exists(list_dir):
        file_list_auto_number = len(os.listdir(list_dir))
        # print('list dir files number:', file_list_auto_number)


def write_header_auto_list_info(args):
    list_dir = args.autolist_dir
    offset = fourth_offset + fileofst(args, args.fourth)
    for __name in os.listdir(list_dir):
        # print(__name)
        with open(os.path.join(list_dir, __name), 'rb') as f:
            args.output.write(pack('2I',filesize(f), offset))
            args.output.write(pack('248s', __name.encode()))
            offset += fileofst(args, f)


def write_header_file_info(args, f, offset):
    if f is not None:
        # print(f.name)
        args.output.write(pack('2I',filesize(f),offset))    # size in bytes and offset
        args.output.write(pack('248s', f.name.encode()))
    else:
        args.output.write(pack('256s', ''.encode()))


def write_header_file_list(args):
    args.output.write(pack('16s', 'fileslist..:'.encode()))

    write_header_file_info(args, args.first,  first_offset)
    write_header_file_info(args, args.second, second_offset)
    write_header_file_info(args, args.third,  third_offset)
    write_header_file_info(args, args.fourth, fourth_offset)

    args.output.write(pack('16s', 'fileslistauto'.encode()))

    write_header_auto_list_info(args)

    args.output.write(pack('12s', 'fileslistend'.encode()))
    args.output.write(pack('I', args.output.tell() + 4))       # size of respkg header


def write_header(args):
    BOOT_MAGIC = 'iRESPKG!'.encode()
    args.output.write(pack('8s', BOOT_MAGIC))

    global first_offset
    global second_offset
    global third_offset
    global fourth_offset

    base_offset = 112   # 8+10*4+16+32+16
    auto_list_num(args)
    # print('list dir files number:', file_list_auto_number)
    if file_list_auto_number:
        base_offset += 16*3+256*(file_list_auto_number+4)
        # print('base_offset:', base_offset)

    first_offset = (int((base_offset + args.pagesize - 1) / args.pagesize) * args.pagesize)
    second_offset= (fileofst(args, args.first) + first_offset )
    third_offset = (fileofst(args, args.second)+ second_offset)
    fourth_offset= (fileofst(args, args.third) + third_offset )

    args.output.write(pack('10I',
        filesize(args.first),                          # size in bytes
        first_offset,                                  # offset in binary
        filesize(args.second),                         # size in bytes
        second_offset,
        filesize(args.third),                          # size in bytes
        third_offset,
        filesize(args.fourth),                         # size in bytes
        fourth_offset,
        args.pagesize,                                 # flash page size we assume
        args.header_version))                          # version of respkg header
    args.output.write(pack('16s', args.board.encode()))# asciiz product name

    sha = sha1()
    update_sha(sha, args.first)
    update_sha(sha, args.second)
    update_sha(sha, args.third)
    update_sha(sha, args.fourth)

    img_id = pack('32s', sha.digest())
    args.output.write(img_id)

    args.output.write(pack('4I',
        file_list_auto_number,                         # file list auto total number
        0,0,                                           # unused 8 bytes
        args.output.tell() + 16))                      # size of respkg header

    if file_list_auto_number:
        write_header_file_list(args)
    pad_file(args.output, args.pagesize)
    return img_id


class ValidateStrLenAction(Action):
    def __init__(self, option_strings, dest, nargs=None, **kwargs):
        if 'maxlen' not in kwargs:
            raise ValueError('maxlen must be set')
        self.maxlen = int(kwargs['maxlen'])
        del kwargs['maxlen']
        super(ValidateStrLenAction, self).__init__(option_strings, dest, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        if len(values) > self.maxlen:
            raise ValueError('String argument too long: max {0:d}, got {1:d}'.
                format(self.maxlen, len(values)))
        setattr(namespace, self.dest, values)


def write_auto_list_data(args):
    list_dir = args.autolist_dir
    for __name in os.listdir(list_dir):
        with open(os.path.join(list_dir, __name), 'rb') as f:
            write_padded_file(args.output, f, args.pagesize)


def write_padded_file(f_out, f_in, padding):
    if f_in is None:
        return
    f_out.write(f_in.read())
    pad_file(f_out, padding)


def parse_int(x):
    return int(x, 0)

def parse_cmdline():
    parser = ArgumentParser()
    parser.add_argument('--first',  help='path to the first file',  type=FileType('rb'), required=True)
    parser.add_argument('--second', help='path to the second file', type=FileType('rb'))
    parser.add_argument('--third',  help='path to the third file',  type=FileType('rb'))
    parser.add_argument('--fourth', help='path to the fourth file', type=FileType('rb'))

    parser.add_argument('--board', help='board name', default='uirespkg', action=ValidateStrLenAction, maxlen=16)
    parser.add_argument('--pagesize', help='page size', type=parse_int, choices=[2**i for i in range(10,15)], default=2048)
    parser.add_argument('--id', help='print the respkg ID on standard output', action='store_true')
    parser.add_argument('--header_version', help='respkg header version', type=parse_int, default=0)
    parser.add_argument('--autolist_dir', help='auto package files dir', default='autolist', action=ValidateStrLenAction, maxlen=16)
    parser.add_argument('-o', '--output', help='output file name', type=FileType('wb'), required=True)
    return parser.parse_args()


def write_data(args):
    write_padded_file(args.output, args.first,  args.pagesize)
    write_padded_file(args.output, args.second, args.pagesize)
    write_padded_file(args.output, args.third,  args.pagesize)
    write_padded_file(args.output, args.fourth, args.pagesize)
    if file_list_auto_number:
        write_auto_list_data(args)

def main():
    args = parse_cmdline()
    img_id = write_header(args)
    write_data(args)
    if args.id:
        if isinstance(img_id, str):
            # Python 2's struct.pack returns a string, but py3 returns bytes.
            img_id = [ord(x) for x in img_id]
        print('0x' + ''.join('{:02x}'.format(c) for c in img_id))

if __name__ == '__main__':
    main()
