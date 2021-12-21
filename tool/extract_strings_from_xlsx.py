#!/usr/bin/python3
"""
Extract Strings from Excel (XLSX) file:
    1. Generate language identifiers and strings identifiers in
       ngux_language_ids.h and ngux_string_ids.h respectively.
    2. Generate string list for different languages in
       raw_strings_<language>.c zipped_strings_<language>.c
    3. Generate the map of language to raw strings or zipped strings in 
       raw_language_to_strings_map or zipped_language_to_bytes_map.c
       respectively.
"""
import os, sys
import time
import re
import traceback
import xlrd
import zlib
import codecs
import shutil
import argparse
import hashlib

FILENAME_LANGUAGEIDS = "ngux_language_ids.h"
FILENAME_STRINGIDS = "ngux_string_ids.h"
FILENAME_STRINGIDS_MINI = "ngux_string_ids_miniphone.h"

VARNAME_RAW_STRINGS = "__ngux_raw_strings_%s"
VARNAME_RAW_MAP = "__ngux_map_language_to_raw_strings"
FILENAME_RAW_STRINGS = "raw_strings_%s.c"
FILENAME_RAW_MAP = "raw_language_to_strings_map.c"

VARNAME_ZIPPED_STRINGS = "__ngux_zipped_bytes_%s"
VARNAME_ZIPPED_MAP = "__ngux_map_language_to_zipped_bytes"
FILENAME_ZIPPED_STRINGS = "zipped_bytes_%s.c"
FILENAME_ZIPPED_MAP = "zipped_language_to_bytes_map.c"

def get_file_md5(filename):
    fd = codecs.open(filename, 'rb')
    ftext = fd.read()
    fd.close()
    md5 = hashlib.md5()
    md5.update(ftext)
    return md5.hexdigest()


def trace_back():
    try:
        return traceback.print_exc()
    except:
        return ''

def usage():
    print ("Usage: extract_strings_from_xlsx <xlsx-file> <out-dir>")

fw_pattern = re.compile (r"^\s*(\w+)")
def get_first_word(str):
    m = fw_pattern.match(str)
    if m == None:
        return None
    return m.group(1)

def str2key(str):
    key = 0;
    l = len(str)
    if l <= 0: return 0
    l = (l)/2
    i = 0
    while i < l:
        w = (ord(str[i<<1])&0xFF) | ((ord(str[(i<<1)+1])<<8)&0xFF00)
        key ^= (w << (i&0xf))
        i += 1
    return key

def get_exten_name(name):
    m = re.match(".*\.(\w+)$", name)
    if m != None:
        return m.group(1)
    return ""

def bin2c(src, dest, dest_name, pkg_name):
    '''
    try:
        fin = open(src, "rb")
    except:
        print("bin2c: open input file %s failed\n"%src)
        return (0, "")
    try:
        fout = open(dest, "w")
    except:
        print("bin2c: open output file %s failed\n"%dest)
        fin.close()
        return (0, "")

    s = fin.read()

    fout.write("/*\n** $Id$\n**\n** " + "%s_%s.cpp"%(pkg_name, dest_name) + " : TODO\n**\n** Copyright (C) 2002 ~ ")
    fout.write(time.strftime('%Y',time.localtime(time.time()))+" Beijing FMSoft Technology Co., Ltd.\n**\n** All rights reserved by FMSoft.\n**\n** Current Maintainer : " + os.uname()[1] + "\n**\n")
    fout.write("** Create Date : "+time.strftime('%Y-%m-%d',time.localtime(time.time()))+"\n*/\n\n")

    fout.write("#include "apollo.h"\n\n")
    fout.write("#ifdef _APOLLO_INNER_RES\n\n")
    fout.write("#ifdef __cplusplus\nextern \"C\"\n{\n#endif /* __cplusplus */\n\n")
    fout.write("extern const NGByte _%s_%s_img_data[];\n\n"%(pkg_name, dest_name))
    fout.write("#ifdef __cplusplus\n}\n#endif /* __cplusplus */\n\n")

    fout.write("const NGByte _%s_%s_img_data[] = {\n  "%(pkg_name, dest_name))
    i = 0
    for c in s:
        fout.write(" 0x%02X,"%ord(c))
        i += 1
        if i == 8:
            fout.write("   ")
        elif i == 16:
            fout.write("\n  ")
            i = 0
    fout.write("\n};\n\n")
    fout.write("#endif   // _APOLLO_INNER_RES\n\n")
    fin.close()
    fout.close()
    return (len(s), "_%s_%s_img_data"%(pkg_name, dest_name))
    '''
    os.system("gen_incore_bitmap " + src + " " + dest + " " + dest_name + " " + pkg_name);
    return (1, "_%s_%s_bitmap"%(pkg_name, dest_name))

def output_strings (dst_file, varname, language, sorted_dict):
    return True

def gen_string_id (table):
    strname = table.col_values (0)
    modules = table.col_values (1)

    string_ids = []
    for i in range (5, table.nrows):
        string_id = "STRID_"
        string_id += make_identifier (modules[i])
        string_id += "_"
        string_id += make_identifier (strname[i])
        string_ids.append (string_id)

    string_ids.sort ()

def make_varname (string):
    name = string.strip ()
    name = name.replace (' ', '')
    name = name.replace ('(', '')
    name = name.replace (')', '')
    name = name.replace ('-', '_')
    name = name.lower ()
    return name;

def gen_strings (table, col):
    strname = table.col_values (0)
    modules = table.col_values (1)
    strings = table.col_values (col)

    string_dict = {}
    for i in range (5, table.nrows):
        string_id = "STRID_"
        string_id += make_identifier (modules[i])
        string_id += "_"
        string_id += make_identifier (strname[i])

        string_dict [string_ids] = strings[i]

    sorted_key_list = sorted (string_dict)
    sorted_dict = map (lambda x:{x:string_dict[x]}, sorted_key_list)

    langid = make_varname (strings [0])
    filename = FILENAME_RAW_STRINGS % langid
    varname = VARNAME_RAW_STRINGS % langid
    output_strings (filename, varname, strings [0], sorted_dict)

def gen_string_dict_for_language (table, col, string_dict = None):
    strname = table.col_values (0)
    modules = table.col_values (1)
    strings = table.col_values (col)

    if string_dict is None:
        string_dict = {}

    for i in range (5, table.nrows):
        string_id = make_string_id (modules[i], strname[i])
        string_dict [string_id] = strings[i]

    return string_dict 

def make_identifier (string):
    strid = string.strip().upper()
    strid = strid.replace (' ', '')
    strid = strid.replace ('-', '')
    return strid;

module_pattern = re.compile(r"^STRID_([A-Z0-9]+)_")
def extract_module_name (string_id):
    m = module_pattern.match (string_id)
    if m == None:
        return "EMPTY"
    return m.group(1)

def make_string_id (module_name, string_name):
    if module_name == 'EMPTY':
        string_id = string_name
    else:
        string_id = "STRID_"
        string_id += make_identifier (module_name)
        string_id += "_"
        string_id += make_identifier (string_name)

    return string_id

invalid_string_pattern = re.compile(r"^[a-z]{2}-[A-Z]{2}#")
def normolize_c_string (a_string):
    if type (a_string) != type (""):
        a_string = str (int (a_string))

    m = invalid_string_pattern.match (a_string)
    if m is not None:
        return ""

    a_string = a_string.replace ('"', '\\"')
    a_string = a_string.replace ('\t', '\\t')
    a_string = a_string.replace ('\n', '\\n')
    a_string = a_string.replace ('\r', '\\n')
    return a_string;

def load_language_info (table):
    ids = table.row_values (0)
    ime_lang_ids = table.row_values (1)
    self_names = table.row_values (3)
    locale_names = table.row_values (4)

    info_list = []
    for i in range (3, table.ncols):
        lang_info = {}
        lang_info ['lang_id'] = ids [i]
        lang_info ['ime_lang_id'] = ime_lang_ids [i]
        lang_info ['locale_name'] = locale_names [i]
        lang_info ['self_name'] = self_names [i]
        info_list.append (lang_info)

    return info_list

def write_language_ids (dst_file, lang_info_list):
    try:
        fout = codecs.open(os.path.join(output_dir,dst_file),'w','utf-8')
    except:
        print("write_language_ids: failed to open output file %s" % dst_file)
        return False

    try:
        fout.write ("/*\n")
        fout.write ("** %s: This file defines the language identifiers. \n" % (dst_file, ))
        fout.write ("**     This file is auto-generated by using `extract_strings_from_xlsx.py`.\n")
        fout.write ("**     Please take care when you modify this file mannually.\n")
        fout.write ("**\n")
        fout.write ("** Copyright (C) 2018 FMSoft.\n")
        fout.write ("**\n")
        #fout.write ("** Create date: %s\n" % (time.strftime ('%Y-%m-%d',time.localtime (time.time()))))
        fout.write ("** Create date: \n")
        fout.write ("*/\n")
        fout.write ("\n")

        fout.write ("#ifndef _NGUX_LANGUAGE_IDS_H_\n")
        fout.write ("#define _NGUX_LANGUAGE_IDS_H_\n")
        fout.write ("\n")
        fout.write ("typedef enum {\n")

        fout.write ("    /* 000 */ LANGID_AUTOMATIC, /* Automatic */\n")
        for i in range (0, len (lang_info_list)):
            fout.write ("    /* %03d */ LANGID_%s, /* %s */\n" % (i+1, lang_info_list[i]['lang_id'], lang_info_list[i]['locale_name'], ))

        fout.write ("    MAX_LANGID, /* Not a real LANGID */\n")
        fout.write ("}MMI_LANG_TYPE_E;\n")
        fout.write ("\n")
        fout.write ("typedef struct _LANGUAGE_RAW_STRINGS {\n")
        fout.write ("    int            ime_lang_id;\n")
        fout.write ("    const char*    locale_name;\n")
        fout.write ("    const char*    self_name;\n")
        fout.write ("    const char**   raw_strings;\n")
        fout.write ("} LANGUAGE_RAW_STRINGS;\n")
        fout.write ("\n")
        fout.write ("typedef struct _LANGUAGE_ZIPPED_STRINGS {\n")
        fout.write ("    int             ime_lang_id;\n")
        fout.write ("    const char*     locale_name;\n")
        fout.write ("    const char*     self_name;\n")
        fout.write ("    const char*     zipped_bytes;\n")
        fout.write ("    unsigned int    zipped_size;\n")
        fout.write ("    unsigned int    origin_size;\n")
        fout.write ("} LANGUAGE_ZIPPED_STRINGS;\n")
        fout.write ("\n")
        fout.write ("#ifdef __cplusplus\n")
        fout.write ("extern \"C\" {\n")
        fout.write ("#endif\n")
        fout.write ("\n")
        fout.write ("extern const LANGUAGE_RAW_STRINGS __ngux_map_language_to_raw_strings [];\n")
        fout.write ("extern const LANGUAGE_ZIPPED_STRINGS __ngux_map_language_to_zipped_bytes [];\n")
        fout.write ("\n")
        fout.write ("#ifdef __cplusplus\n")
        fout.write ("}\n")
        fout.write ("#endif\n")
        fout.write ("\n")
        fout.write("#endif /* _NGUX_LANGUAGE_IDS_H_ */\n")
        fout.write ("\n")
    except:
        fout.close ()
        trace_back ()
        print("write_language_ids: failed to write code to %s" % dst_file)
        return False

    fout.close ()
    return True

def write_string_ids (dst_file, string_dict, sorted_string_ids):
    try:
        fout = codecs.open(os.path.join(output_dir,dst_file),'w','utf-8')
    except:
        print("write_string_ids: failed to open output file %s" % dst_file)
        return False

    try:
        fout.write ("/*\n")
        fout.write ("** %s: This file defines the string identifiers. \n" % (dst_file, ))
        fout.write ("**     This file is auto-generated by using `extract_strings_from_xlsx.py`.\n")
        fout.write ("**     Please take care when you modify this file mannually.\n")
        fout.write ("**\n")
        fout.write ("** Copyright (C) 2018 FMSoft.\n")
        fout.write ("**\n")
        #fout.write ("** Create date: %s\n" % (time.strftime ('%Y-%m-%d',time.localtime (time.time()))))
        fout.write ("** Create date: \n" )
        fout.write ("*/\n")
        fout.write ("\n")

        fout.write ("#ifndef _NGUX_STRING_IDS_H_\n")
        fout.write ("#define _NGUX_STRING_IDS_H_\n")
        fout.write ("\n")
        fout.write ("enum {\n")

        old_module_name = None
        for i in range (0, len (sorted_string_ids)):
            module_name = extract_module_name (sorted_string_ids[i])
            my_string = string_dict[sorted_string_ids[i]]
            if my_string is None:
                my_string = ""
            else:
                my_string = normolize_c_string (my_string)
            fout.write ("    /* %05d */ %s, // %s; %s\n" % (i, sorted_string_ids[i], module_name, my_string))
        fout.write ("    MAX_STRID, // Not a real STRID\n")    
        fout.write ("    STRID_TONEVOICE_TYPE_LOCAL_ECL_QTN_BEGIN,\n")
        fout.write ("    STRID_TONEVOICE_TYPE_LOCAL_ECL_QTN_END = STRID_TONEVOICE_TYPE_LOCAL_ECL_QTN_BEGIN+18,\n")
        fout.write ("};\n")
        fout.write ("\n")
        fout.write("#endif /* _NGUX_STRING_IDS_H_ */\n")
        fout.write ("\n")
    except:
        fout.close ()
        trace_back ()
        print("write_string_ids: failed to write code to %s" % dst_file)
        return False

    fout.close ()
    return True

def write_raw_strings (dst_file, language_id, varname, string_dict, sorted_string_ids):
    try:
        fout = codecs.open(os.path.join(output_dir,dst_file),'w','utf-8')
    except:
        print("write_raw_strings: failed to open output file %s" % dst_file)
        return False

    try:
        fout.write ("/*\n")
        fout.write ("** %s: This file defines the raw string array for specific language (%s). \n" % (dst_file, language_id))
        fout.write ("**     This file is auto-generated by using `extract_strings_from_xlsx.py`.\n")
        fout.write ("**     Please take care when you modify this file mannually.\n")
        fout.write ("**\n")
        fout.write ("** Copyright (C) 2018 FMSoft.\n")
        fout.write ("**\n")
        #fout.write ("** Create date: %s\n" % (time.strftime ('%Y-%m-%d',time.localtime (time.time()))))
        fout.write ("** Create date: \n" )
        fout.write ("*/\n")
        fout.write ("\n")

        fout.write ("#ifdef __MMI_LANGUAGE_%s__\n" % (language_id, ))
        fout.write ("const char* %s [] = {\n" % (varname, ))

        for i in range (0, len (sorted_string_ids)):
            module_name = extract_module_name (sorted_string_ids[i])
            my_string = string_dict[sorted_string_ids[i]]
            if my_string is None:
                my_string = ""
            else:
                my_string = normolize_c_string (my_string)
            fout.write ("    // %05d %s %s\n" % (i, module_name, sorted_string_ids[i], ))
            fout.write ("    \"%s\",\n" % (my_string, ))

        fout.write ("};\n")
        fout.write ("\n")
        fout.write("#endif /* __MMI_LANGUAGE_%s__ */\n" % (language_id, ))
        fout.write ("\n")

    except:
        fout.close ()
        trace_back ()
        print("write_raw_strings: failed to write code to %s" % dst_file)
        return False

    fout.close ()
    return True

def write_raw_map (dst_file, lang_info_list):
    try:
        fout = codecs.open(os.path.join(output_dir,dst_file),'w','utf-8')
    except:
        print ("write_raw_map: failed to open output file %s" % dst_file)
        return False

    try:
        fout.write ("/*\n")
        fout.write ("** %s: This file defines the map from language to raw strings. \n" % (dst_file, ))
        fout.write ("**     This file is auto-generated by using `extract_strings_from_xlsx.py`.\n")
        fout.write ("**     Please take care when you modify this file mannually.\n")
        fout.write ("**\n")
        fout.write ("** Copyright (C) 2018 FMSoft.\n")
        fout.write ("**\n")
        #fout.write ("** Create date: %s\n" % (time.strftime ('%Y-%m-%d',time.localtime (time.time()))))
        fout.write ("** Create date: \n" )
        fout.write ("*/\n")
        fout.write ("\n")

        fout.write ("#include \"ime_language_ids.h\"\n")
        fout.write ("#include \"ngux_language_ids.h\"\n")
        fout.write ("\n")

        fout.write ("#ifndef NULL\n")
        fout.write ("#define NULL ((void*)0)\n")
        fout.write ("#endif\n")
        fout.write ("\n")

        for i in range (0, len (lang_info_list)):
            lang_id = lang_info_list [i]['lang_id']
            varname = VARNAME_RAW_STRINGS % lang_id.lower ()
            fout.write ("#ifdef __MMI_LANGUAGE_%s__\n" % (lang_id, ))
            fout.write ("extern const char* %s [];\n" % (varname))
            fout.write ("#endif /* __MMI_LANGUAGE_%s__*/\n" % (lang_id, ))

        fout.write ("\n")

        fout.write ("extern const LANGUAGE_RAW_STRINGS %s [] = {\n" % (VARNAME_RAW_MAP))
        fout.write ("    {/*LANGID_AUTOMATIC, */IMELANGIDNone, \"Automatic\", \"Automatic\", NULL},\n")
        for i in range (0, len (lang_info_list)):
            lang_id = lang_info_list [i]['lang_id']
            varname = VARNAME_RAW_STRINGS % lang_id.lower ()
            fout.write ("#ifdef __MMI_LANGUAGE_%s__\n" % (lang_id, ))
            fout.write ("    {/* LANGID_%s, */%s, \"%s\", \"%s\", %s},\n" % (lang_id, lang_info_list [i]['ime_lang_id'], \
                lang_info_list [i]['locale_name'], lang_info_list [i]['self_name'], varname, ))
            fout.write ("#else\n")
            fout.write ("    {/*LANGID_%s, */%s, NULL, NULL, NULL},\n" % (lang_id, lang_info_list [i]['ime_lang_id'], ))
            fout.write ("#endif /* __MMI_LANGUAGE_%s__*/\n" % (lang_id, ))
        fout.write ("};\n")
        fout.write ("\n")

    except:
        fout.close ()
        trace_back ()
        print ("write_raw_map: failed to write code to %s" % dst_file)
        return False

    fout.close ()
    return True


def gen_string_dict (table, col, string_dict = None):
    strname = table.col_values (0)
    modules = table.col_values (1)
    strings = table.col_values (col)

    if string_dict is None:
        string_dict = {}

    for i in range (5, table.nrows):
        string_id = make_string_id (modules[i], strname[i])
        string_dict [string_id] = strings[i]

    return string_dict

def zip_strings (string_dict, sorted_string_ids):
    origin_data = bytearray ()
    origin_size = 0
    for i in range (0, len (sorted_string_ids)):
        module_name = extract_module_name (sorted_string_ids[i])
        my_string = string_dict [sorted_string_ids[i]]
        if my_string is None:
            my_string = ""
        # else:
            # my_string = normolize_c_string (my_string)

        my_string = my_string.encode (encoding="utf-8")
        origin_data.extend (my_string)
        origin_data.append (0x00)
        origin_size += len (my_string)
        origin_size += 1

    zipped_bytes = zlib.compress (origin_data, zlib.Z_BEST_COMPRESSION)
    return zipped_bytes, origin_size

def write_zipped_bytes (dst_file, language_id, varname, zipped_bytes, zipped_size):
    try:
        fout = codecs.open(os.path.join(output_dir,dst_file),'w','utf-8')
    except:
        print("write_zipped_bytes: failed to open output file %s" % dst_file)
        return False

    try:
        fout.write ("/*\n")
        fout.write ("** %s: This file defines the zipped string bytes for specific language (%s). \n" % (dst_file, language_id))
        fout.write ("**     This file is auto-generated by using `extract_strings_from_xlsx.py`.\n")
        fout.write ("**     Please take care when you modify this file mannually.\n")
        fout.write ("**\n")
        fout.write ("** Copyright (C) 2018 FMSoft.\n")
        fout.write ("**\n")
        #fout.write ("** Create date: %s\n" % (time.strftime ('%Y-%m-%d',time.localtime (time.time()))))
        fout.write ("** Create date: \n")
        fout.write ("*/\n")
        fout.write ("\n")

        fout.write ("#ifdef __MMI_LANGUAGE_%s__\n" % (language_id, ))
        fout.write ("const char %s [] = {\n" % (varname, ))

        for i in range (0, zipped_size):
            if i % 16 == 0:
                fout.write ("    ")
            fout.write ("0x%02x, " % (zipped_bytes[i], ))
            if (i + 1) % 16 == 0:
                fout.write ("\n")

        fout.write ("\n")
        fout.write ("};\n")
        fout.write ("\n")
        fout.write("#endif /* __MMI_LANGUAGE_%s__ */\n" % (language_id, ))
        fout.write ("\n")

    except:
        fout.close ()
        trace_back ()
        print("write_zipped_bytes: failed to write code to %s" % dst_file)
        return False

    fout.close ()
    return True

def write_zipped_map (dst_file, lang_info_list):
    try:
        # fout = open (os.path.join(output_dir,dst_file), "w")
        fout = codecs.open(os.path.join(output_dir,dst_file), 'w', 'utf_8_sig')
    except:
        print ("write_zipped_map: failed to open output file %s" % dst_file)
        return False

    try:
        fout.write ("/*\n")
        fout.write ("** %s: This file defines the map from language to zipped strings. \n" % (dst_file, ))
        fout.write ("**     This file is auto-generated by using `extract_strings_from_xlsx.py`.\n")
        fout.write ("**     Please take care when you modify this file mannually.\n")
        fout.write ("**\n")
        fout.write ("** Copyright (C) 2018 FMSoft.\n")
        fout.write ("**\n")
        #fout.write ("** Create date: %s\n" % (time.strftime ('%Y-%m-%d',time.localtime (time.time()))))
        fout.write ("** Create date: \n" )
        fout.write ("*/\n")
        fout.write ("\n")

        fout.write ("#include \"ime_language_ids.h\"\n")
        fout.write ("#include \"ngux_language_ids.h\"\n")
        fout.write ("\n")

        fout.write ("#ifndef NULL\n")
        fout.write ("#define NULL ((void*)0)\n")
        fout.write ("#endif\n")
        fout.write ("\n")

        for i in range (0, len (lang_info_list)):
            lang_info = lang_info_list[i]
            fout.write ("#ifdef __MMI_LANGUAGE_%s__\n" % (lang_info ['lang_id'], ))
            fout.write ("extern const char %s [];\n" % (lang_info ['varname']))
            fout.write ("#endif /* __MMI_LANGUAGE_%s__*/\n" % (lang_info ['lang_id'], ))
        fout.write ("\n")

        fout.write ("extern const LANGUAGE_ZIPPED_STRINGS %s [] = {\n" % (VARNAME_ZIPPED_MAP))
        fout.write ("    {/*LANGID_AUTOMATIC, */IMELANGIDNone, \"Automatic\", \"Automatic\", NULL, 0, 0},\n")
        for i in range (0, len (lang_info_list)):
            lang_info = lang_info_list[i]
            fout.write ("#ifdef __MMI_LANGUAGE_%s__\n" % (lang_info ['lang_id'], ))
            fout.write ("    {/* LANGID_%s, */%s, \"%s\", \"%s\", %s, %d, %d},\n" % (lang_info ['lang_id'], \
                    lang_info ['ime_lang_id'], lang_info ['locale_name'], lang_info ['self_name'], \
                    lang_info ['varname'], lang_info['zipped_size'], lang_info ['origin_size']))
            fout.write ("#else\n")
            fout.write ("    {/*LANGID_%s, */%s, NULL, NULL, NULL, 0, 0},\n" % (lang_info ['lang_id'], \
                    lang_info ['ime_lang_id'], ))
            fout.write ("#endif /* __MMI_LANGUAGE_%s__*/\n" % (lang_info ['lang_id'], ))
        fout.write ("};\n")
        fout.write ("\n")

    except:
        fout.close ()
        trace_back ()
        print ("write_zipped_map: failed to write code to %s" % dst_file)
        return False

    fout.close ()
    return True

def extract_strings_from_xlsx(excel_file, output_dir):
    assert os.path.exists(excel_file)
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)
    try:
        data = xlrd.open_workbook (excel_file)
        table = data.sheets()[0] 
        nrows = table.nrows
        ncols = table.ncols
    except:
        print ("Failed to open Excel file: %s" % (sys.argv[1]))
        sys.exit (2)

    print ("Loading language info from %s (total %d)..." % (sys.argv[1], ncols - 3))
    lang_info_list = load_language_info (table)
    print ("DONE")

    print ("Generating language identifiers from %s (total %d)..." % (sys.argv[1], ncols - 3))
    write_language_ids (FILENAME_LANGUAGEIDS, lang_info_list)
    print ("DONE")

    print ("Generating string identifiers from %s (total %d)..." % (sys.argv[1], nrows - 5))
    string_dict = gen_string_dict (table, 2, None)
    sorted_string_ids = list (string_dict.keys())
    sorted_string_ids.sort()
    if args.dist_dir == "mini":
        write_string_ids (FILENAME_STRINGIDS_MINI, string_dict, sorted_string_ids)
    else:
        write_string_ids (FILENAME_STRINGIDS, string_dict, sorted_string_ids)
    print ("DONE")

    print ("Generating strings source files from %s for %d languages..." % (sys.argv[1], ncols - 3))
    total_origin_size = 0
    total_zipped_size = 0
    max_origin_size = 0
    max_zipped_size = 0

    for i in range (0, len (lang_info_list)):
        language_id = lang_info_list [i]['lang_id']

        print ("    Generating strings for language %s (%d)..." % (language_id, i, ))
        filename = FILENAME_RAW_STRINGS % language_id.lower ()
        varname = VARNAME_RAW_STRINGS % language_id.lower ()
        gen_string_dict (table, i + 3, string_dict)
        print ("    done")

        print ("    Writting raw strings to %s for language %s (%d/%d)..." % (filename, language_id, i, ncols - 3))
        write_raw_strings (filename, language_id, varname, string_dict, sorted_string_ids)
        print ("    done")

        print ("    Zipping strings for language %s (%d/%d)..." % (language_id, i, ncols - 3))
        filename = FILENAME_ZIPPED_STRINGS % language_id.lower ()
        varname = VARNAME_ZIPPED_STRINGS % language_id.lower ()
        zipped_bytes, origin_size = zip_strings (string_dict, sorted_string_ids)
        zipped_size = len (zipped_bytes)

        lang_info_list [i]['varname'] = varname
        lang_info_list [i]['origin_size'] = origin_size
        lang_info_list [i]['zipped_size'] = zipped_size

        total_origin_size += origin_size
        total_zipped_size += zipped_size
        if origin_size > max_origin_size:
            max_origin_size = origin_size
        if zipped_size > max_zipped_size:
            max_zipped_size = zipped_size
        print ("    done")

        print ("    Writting zipped strings to %s for language %s (%d/%d)..." % (filename, language_id, i, ncols - 3))
        write_zipped_bytes (filename, language_id, varname, zipped_bytes, zipped_size)
        print ("    done")
    print ("DONE")

    print ("Writting language to raw strings map to %s (total %d)..." % (FILENAME_RAW_MAP, ncols - 3))
    write_raw_map (FILENAME_RAW_MAP, lang_info_list)
    print ("DONE")

    print ("Writting language to zipped strings map to %s (total %d)..." % (FILENAME_ZIPPED_MAP, ncols - 3))
    write_zipped_map (FILENAME_ZIPPED_MAP, lang_info_list)
    print ("DONE: Total origin size: %d, total zipped size: %d, max origin size: %d, max zipped size: %d" % (total_origin_size, total_zipped_size, max_origin_size, max_zipped_size))


def copy_source_files_to_evb(src_dir, root_top_dir):
    ori_src_dir = src_dir
    print("    SOURCE ori_src_dir=%s"%ori_src_dir)
    evb_top_dir = root_top_dir
    print("    TARGET evb_tgt_dir=%s"%evb_top_dir)
    assert os.path.exists(ori_src_dir), ori_src_dir
    assert os.path.exists(evb_top_dir), evb_top_dir
    print("    start copying files...")
    tgt_header_dir=os.path.join(evb_top_dir,"gui","mgngux","resource","include")
    assert os.path.exists(tgt_header_dir), tgt_header_dir
    for _src_file in os.listdir(ori_src_dir):
        if _src_file.endswith(".h"):
            _src_file = os.path.join(ori_src_dir,_src_file)
            shutil.copy2(_src_file, tgt_header_dir)
    print("    target dir3:%s copied."%tgt_header_dir)
    tgt_source_dir=os.path.join(evb_top_dir,"gui","mgapollo","strings","src")
    assert os.path.exists(tgt_source_dir), tgt_source_dir
    for _src_file in os.listdir(ori_src_dir):
        if _src_file.endswith(".c") and (_src_file.startswith("raw_") or _src_file.startswith("zipped_")):
            _src_file = os.path.join(ori_src_dir,_src_file)
            shutil.copy2(_src_file, tgt_source_dir)
    print("    target dir4:%s copied."%tgt_source_dir)
    print( "    copy files to evb, done")

def copy_source_files_to_miniphone(src_dir, root_top_dir):
    ori_src_dir = src_dir
    print("    SOURCE ori_src_dir=%s"%ori_src_dir)
    evb_top_dir = root_top_dir
    print("    TARGET evb_tgt_dir=%s"%evb_top_dir)
    assert os.path.exists(ori_src_dir), ori_src_dir
    assert os.path.exists(evb_top_dir), evb_top_dir
    print("    start copying files...")
    tgt_header_dir=os.path.join(evb_top_dir,"gui","mgngux","resource","include")
    assert os.path.exists(tgt_header_dir), tgt_header_dir
    for _src_file in os.listdir(ori_src_dir):
        if _src_file.endswith(".h"):
            _src_file = os.path.join(ori_src_dir,_src_file)
            shutil.copy2(_src_file, tgt_header_dir)
    print("    target dir3:%s copied."%tgt_header_dir)
    tgt_source_dir=os.path.join(evb_top_dir,"gui","miniphone","strings","src")
    assert os.path.exists(tgt_source_dir), tgt_source_dir
    for _src_file in os.listdir(ori_src_dir):
        if _src_file.endswith(".c") and (_src_file.startswith("raw_") or _src_file.startswith("zipped_")):
            _src_file = os.path.join(ori_src_dir,_src_file)
            shutil.copy2(_src_file, tgt_source_dir)
    print("    target dir4:%s copied."%tgt_source_dir)
    print( "    copy files to evb, done")

def deploy_source_files(src_dir, root_top_dir):
    ori_src_dir = src_dir
    print("    SOURCE ori_src_dir=%s"%ori_src_dir)
    emu_top_dir = root_top_dir
    print("    TARGET evb_tgt_dir=%s"%emu_top_dir)
    assert os.path.exists(ori_src_dir),ori_src_dir
    assert os.path.exists(emu_top_dir), emu_top_dir
    print("    start copying files...")
    tgt_header_dir=os.path.join(emu_top_dir,"targets","include")
    assert os.path.exists(tgt_header_dir), tgt_header_dir
    for _src_file in os.listdir(ori_src_dir):
        if _src_file.endswith(".h"):
            _src_file = os.path.join(ori_src_dir,_src_file)
            shutil.copy2(_src_file, tgt_header_dir)
    print("    target dir3:%s copied."%tgt_header_dir)
    tgt_source_dir=os.path.join(emu_top_dir,"mgapollo","strings","src")
    assert os.path.exists(tgt_source_dir), tgt_source_dir
    for _src_file in os.listdir(ori_src_dir):
        if _src_file.endswith(".c") and (_src_file.startswith("raw_") or _src_file.startswith("zipped_")):
            _src_file = os.path.join(ori_src_dir,_src_file)
            shutil.copy2(_src_file, tgt_source_dir)
    print("    target dir4:%s copied."%tgt_source_dir)
    print( "    copy files to emulator, done")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('input_file')
    parser.add_argument('output_dir')
    parser.add_argument('root_top_dir')
    parser.add_argument("-t","--dist_dir", choices=["emulator","evb","mini"],default="emulator")

    args = parser.parse_args()
    excel_file = args.input_file
    output_dir = args.output_dir
    root_top_dir = args.root_top_dir
    md5_file = "md5.bin"
    md5_value_bak = ""
    if os.path.exists(md5_file):
        with open(md5_file,"r") as fob:
            md5_value_bak = fob.read()
    md5_value = get_file_md5(excel_file)
    print("md5_value_bak:",md5_value_bak)
    print("md5_value:",md5_value)
    with open(md5_file,"w") as fob:
        fob.write(md5_value)
    if md5_value_bak == md5_value:
        sys.exit()
    extract_strings_from_xlsx(excel_file,output_dir)
    if args.dist_dir == "evb":
        copy_source_files_to_evb(output_dir, root_top_dir)
    if args.dist_dir == "emulator":
        root_top_dir = os.path.join("..","..","..","..","mmi-1308")
        deploy_source_files(output_dir, root_top_dir)
    if args.dist_dir == "mini":
        copy_source_files_to_miniphone(output_dir,root_top_dir)
    shutil.rmtree(output_dir)



