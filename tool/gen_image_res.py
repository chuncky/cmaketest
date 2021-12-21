#!usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
import os
import re
import codecs


# DEF_CONFIG_RES_NAME="asr-128x160"
#DEF_CONFIG_RES_NAME="nokia-240x320"
#DEF_CONFIG_RES_NAME="custom01-240x320"


_img_file_text = '''\
#include "ngux.h"
#include "apolloconfig.h"

#ifdef _APOLLO_INNER_RES

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern const unsigned char %s[];

#ifdef __cplusplus
}
#endif // __cplusplus
'''

_img_end_text = "#endif   // _APOLLO_INNER_RES\n"


res_file_text = '''\
// Main CPP File For Res Desc File
#include "ngux.h"
#include "apolloconfig.h"
#ifdef _APOLLO_INNER_RES

USE_NGUX_NAMESPACE
'''

end_line = "};\n"

def get_img_files(res_file):
    with codecs.open(res_file,'r','utf-8') as _obj:
        _start = False
        _line_l = []
        for _line in _obj:
            _line = _line.strip()
            if _line.startswith("//") or _line.startswith("/*"):
                continue
            if "begin_image_res" in _line:
                _start = True
                continue
            if "end_image_res" in _line:
                break
            if _start:
                _line_l.append(_line)
        # print(_line_l)
        _text = "".join(_line_l)
        _text = _text.replace(" ","").replace("\t","")
    # print(_text)
    return re.findall('image\((.*?),"(.*?)"\)',_text)


def parser_res_file_image(app_name):
    res_file = os.path.join(res_desc_root_dir, app_name, "include", app_name+".res.c")
    print("Process Image Of Res Describe File: %s -------->"%res_file)
    _img_info_list = get_img_files(res_file)
    # if not _img_info_list:
        # print("Warning : no image_desc found in %s !"%res_file)
        # print("Done <---------")
        # return False
    _img_fname = "_img_%s_inner_res.h"%app_name
    dir_name = os.path.join(output_root_dir, app_name, "include")
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)
    inner_res_output_file = os.path.join(dir_name,_img_fname)
    inner_res_fout = codecs.open(inner_res_output_file,'w','utf-8')
    inner_res_fout.write(res_file_text)

    _inner_res_list = []
    for _name, _img_file_name in _img_info_list:
        _fname = "%s_%s.h"%(app_name,_name)
        _file_name = os.path.join(output_root_dir, app_name, "include", _fname)
        # print(_file_name)
        _img_file = os.path.join(res_file_root_dir, _img_file_name)
        # print(_img_file)
        inner_res_fout.write('#include "%s"\n'%_fname)
        fout = codecs.open(_file_name,'w','utf-8')
        arr_name = "_%s_%s_bitmap"%(app_name, _name)
        first_text = _img_file_text%(arr_name)
        # print(first_text)
        fout.write(first_text)
        first_line = "const unsigned char %s[] = {\n"%arr_name
        fout.write(first_line)
        if os.path.exists(_img_file):
            _img_file_cnt = os.path.getsize(_img_file)
            with open(_img_file,'rb') as fin:
                _ch = True
                _list = []
                while _ch:
                    _ch = fin.read(1)
                    hex_str = "0x%s"%_ch.hex()
                    # print(hex_str)
                    _list.append(hex_str)
                _list = _list[:-1]
            step = 8
            for i in range(0, _img_file_cnt, step):
                fout.write(", ".join(_list[i:i+step])+",\n")
                fout.flush()
        else:
            _img_file_cnt = 0
            fout.write("0x00,\n")
        fout.write(end_line)
        fout.write("\n\n")
        fout.write(_img_end_text)
        fout.flush()
        fout.close()
        _inner_res_text = '''{ "%s", (unsigned char*)%s, %d},//R_%s_img_%s'''%(_img_file_name, arr_name, _img_file_cnt, app_name, _name)
        _inner_res_list.append(_inner_res_text)
    inner_res_fout.write("\n\n")
    first_line = "static const INNER_RES_INFO _img_%s_inner_res[] = {\n"%app_name
    inner_res_fout.write(first_line)
    if _inner_res_list:
        for _line in _inner_res_list:
            inner_res_fout.write("    "+_line+"\n")
        inner_res_fout.write("    { NULL, NULL,    0},//NULL\n")
    else:
        print("Warning : no image_desc found in %s !"%res_file)
        inner_res_fout.write("    { NULL, NULL,    0},//NULL\n")
    inner_res_fout.write(end_line)
    inner_res_fout.write("\n\n")
    inner_res_fout.write(_img_end_text)
    inner_res_fout.flush()
    inner_res_fout.close()
    print("Create %s done"%inner_res_output_file)
    print("Done <---------")
    return _img_fname


main_res_first_text = """\
// inner_resouce Main CPP File
#include "ngux.h"
#include "apolloconfig.h"
#include "apolloresdef.h"
#ifdef _APOLLO_INNER_RES

USE_NGUX_NAMESPACE

#pragma arm section rodata = "RESPKGIMAGE"

"""

main_res_end_text = """\
extern const int g_ResCount = sizeof(g_ResArray)/sizeof(INNER_RES_ARRAY);

#pragma arm section

#endif   // _APOLLO_INNER_RES
"""

main_res_line = "#define ADDInnerResource(RES_ID, R_TYPE, inner_res, num)  {RES_ID, R_TYPE, inner_res, num },"
main_res_line_1 = '#pragma arm section rodata = "RESPKGIMAGEHDR"'

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('gui_root_dir',help = "input gui relative path: gui\ ")
    parser.add_argument('-t','--type_name',choices = ["FP","mini"], default = "FP", help = "input type name: FP or mini")
    parser.add_argument('res_name',help = "input res name: asr-240x320")

    args = parser.parse_args()
    os.chdir(args.gui_root_dir)
    DEF_CONFIG_RES_NAME = args.res_name

    if args.type_name == "FP":
        _type_name = "mgapollo"
    else:
        _type_name = "miniphone"
    global_config_res_name = DEF_CONFIG_RES_NAME
    res_desc_root_dir = os.path.join(_type_name, "resdesc", global_config_res_name)
    res_file_root_dir = os.path.join(_type_name, "resfile", global_config_res_name)
    output_root_dir = os.path.join(_type_name, "inner-res", global_config_res_name)


    application_list = os.listdir(res_desc_root_dir)
    # print(application_list)
    application_list.sort()
    # print(application_list)
    main_res_cpp_file_name="inner_resouce.cpp"
    main_res_cpp_file_name = os.path.join(output_root_dir,"src",main_res_cpp_file_name)
    print("Res File Dir: ",res_file_root_dir)
    main_res_fout = codecs.open(main_res_cpp_file_name,'w','utf-8')
    main_res_fout.write(main_res_first_text)
    _img_h_file_list = []
    for _app in application_list:
        _img_h_file = parser_res_file_image(_app)
        if not _img_h_file:
            continue
        _img_h_file_list.append((_app,_img_h_file))
        main_res_fout.write('#include "%s"\n'%_img_h_file)
        # input(_app)
    main_res_fout.write("\n")
    main_res_fout.write(main_res_line)
    main_res_fout.write("\n\n")
    main_res_fout.write(main_res_line_1)
    main_res_fout.write("\n\n")
    main_res_fout.write("extern const INNER_RES_ARRAY g_ResArray[] = {\n")
    for _app,_name in _img_h_file_list:
        _first_str = "RES_PKG_%s_ID,"%_app.upper()
        _first_str = "%-25s"%_first_str
        _line_text = "    ADDInnerResource({0} R_TYPE_IMAGE, (INNER_RES_INFO *){1}, sizeof({1})/sizeof(INNER_RES_INFO))".format(_first_str, _name.replace(".h",""))
        main_res_fout.write(_line_text+"\n")

    main_res_fout.write(end_line)
    main_res_fout.write(main_res_end_text)
    print("Generate Image Res Done!")



