#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import sys
import json


def load_json(json_file):
    assert os.path.exists(json_file),"%s no exists" % json_files
    with open(json_file) as f:
        json_str = json.load(f)
    return json_str


if __name__ == "__main__":
    barod_version = sys.argv[1]
    json_file = os.path.join(barod_version,"config","template","CRANE_EVB.json")
    json_dict = load_json(json_file)
    # for _name in json_dict:
        # print( json_dict[_name])
    for _d in json_dict["images"]:
        if _d["name"] == "rd":
            # print (_d["rd"])
            image_name = _d["image"]
            path_dir = _d["use"]
    # print(path_dir)
    input_image_dir = os.path.join(barod_version, "reliabledata", path_dir)
    output_image = os.path.join(barod_version,"images",image_name)
    rd_l = [_dd for _d in json_dict["images"] if _d["name"] == "rd" for _dd in _d["rd"]]
    for _d in rd_l:
        if _d["path"] == path_dir:
            # print(_d["images"])
            images = [_v["image"] for _v in _d["images"]]
            print(images)
            images = [os.path.join(input_image_dir, _v) for _v in images]
            # print(images)
            break
    cmd = "RDGenerator_dos.exe -n %s -o %s -r"%(" -n ".join(images), output_image)
    # print(cmd)
    os.system(cmd)