#!usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('input_bin')
    parser.add_argument('output_file')
    parser.add_argument('arr_name')
    args = parser.parse_args()
    fout = open(args.output_file,'w')
    first_line = 'extern "C" const unsigned char %s[] = {\n'%args.arr_name
    end_line = "};"
    fout.write(first_line)
    with open(args.input_bin,'rb')as fin:
        _ch = True
        _list = []
        while _ch:
            _ch = fin.read(1)
            hex_str = "0x%s"%_ch.hex()
            # print(hex_str)
            _list.append(hex_str)
            if len(_list) == 16:
                # print(_list)
                fout.write(", ".join(_list)+",\n")
                fout.flush()
                _list = []
        if _list:
            # print(_list[:-1])
            fout.write(", ".join(_list[:-1])+"\n")
            fout.flush()
    fout.write(end_line)

    fout.write('\n\nextern "C" const unsigned int %s_len = %d;\n\n'%(args.arr_name, os.path.getsize(args.input_bin)))
    fout.close()

