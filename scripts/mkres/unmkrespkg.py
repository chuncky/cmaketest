#!/usr/bin/env python

"""unpacks the bootimage.

Extracts the kernel, ramdisk, second bootloader and recovery dtbo images.
"""

from __future__ import print_function
from argparse import ArgumentParser, FileType
from struct import unpack
import os


def create_out_dir(dir_path):
    """creates a directory 'dir_path' if it does not exist"""
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)


def extract_image(offset, size, bootimage, extracted_image_name):
    """extracts an image from the bootimage"""
    bootimage.seek(offset)
    with open(extracted_image_name, 'wb') as file_out:
        file_out.write(bootimage.read(size))


def get_number_of_pages(image_size, page_size):
    """calculates the number of pages required for the image"""
    return (image_size + page_size - 1) / page_size


def unpack_uirespkg(args):
    """extracts first, second, third, fourth and more image"""
    boot_magic = unpack('8s', args.respkgimg.read(8))
    print('image_magic: %s' % boot_magic)
    image_info = unpack('10I', args.respkgimg.read(10 * 4))
    print('first_size   : %s' % image_info[0])
    print('first_offset : %s' % image_info[1])
    print('second_size  : %s' % image_info[2])
    print('second_offset: %s' % image_info[3])
    print('third_size   : %s' % image_info[4])
    print('third_offset : %s' % image_info[5])
    print('fourth_size  : %s' % image_info[6])
    print('fourth_offset: %s' % image_info[7])
    print('page size    : %s' % image_info[8])
    print('image header version: %s' % image_info[9])

    product_name = args.respkgimg.read(16).decode().strip('\0')
    print('product name: %s' % product_name)

    args.respkgimg.read(32)  # ignore SHA
    file_info = unpack('4I', args.respkgimg.read(4 * 4))
    file_list_auto_number = file_info[0]
    image_header_size = file_info[3]
    print('auto file list number : %s' % file_list_auto_number)
    print('image tiny header size: %s' % image_header_size)

    first_size   = image_info[0]
    first_offset = image_info[1]
    second_size  = image_info[2]
    second_offset= image_info[3]
    third_size   = image_info[4]
    third_offset = image_info[5]
    fourth_size  = image_info[6]
    fourth_offset= image_info[7]
    page_size    = image_info[8]
    version      = image_info[9]

    image_info_list = [(first_offset, first_size, 'first')]
    image_info_list.append((second_offset, second_size, 'second'))
    image_info_list.append((third_offset, third_size, 'third'))
    image_info_list.append((fourth_offset, fourth_size, 'fourth'))

    if file_list_auto_number:
        args.respkgimg.read(16)  # ignore 'fileslist..:'

        args.respkgimg.read(8)   # ignore first size and offset
        first_image_name = args.respkgimg.read(248).decode().strip('\0')
        print('1st image name: ', first_image_name)

        args.respkgimg.read(8)   # ignore second size and offset
        second_image_name = args.respkgimg.read(248).decode().strip('\0')
        print('2nd image name: ', second_image_name)

        args.respkgimg.read(8)   # ignore third size and offset
        third_image_name = args.respkgimg.read(248).decode().strip('\0')
        print('3rd image name: ', third_image_name)

        args.respkgimg.read(8)   # ignore fourth size and offset
        fourth_image_name = args.respkgimg.read(248).decode().strip('\0')
        print('4th image name: ', fourth_image_name)

        args.respkgimg.read(16)  # ignore 'fileslistauto'
        if file_list_auto_number:
            loop = file_list_auto_number
            while loop :
                loop -= 1
                fauto_info=unpack('2I', args.respkgimg.read(8))
                fauto_name = args.respkgimg.read(248).decode().strip('\0')
                print('... image name: ', fauto_name,',size:',fauto_info[0],',offset:',fauto_info[1])
                image_info_list.append((fauto_info[1], fauto_info[0], fauto_name))

        args.respkgimg.read(12)  # ignore 'fileslistend'
        boot_header_size = unpack('I', args.respkgimg.read(4))[0]
        print('image full header size: %s' % boot_header_size)

    for image_info in image_info_list:
        extract_image(image_info[0], image_info[1], args.respkgimg,
            os.path.join(args.out, image_info[2]))


def parse_cmdline():
    """parse command line arguments"""
    parser = ArgumentParser(
        description='Unpacks uirespkg.bin, extracts the first, '
        'second, third, fourth and more images')
    parser.add_argument('--respkgimg',
        help='path to respkg image', type=FileType('rb'), required=True)
    parser.add_argument('--out', help='path to out binaries', default='out')
    return parser.parse_args()


def main():
    """parse arguments and unpack uirespkg image"""
    args = parse_cmdline()
    create_out_dir(args.out)
    unpack_uirespkg(args)


if __name__ == '__main__':
    main()
