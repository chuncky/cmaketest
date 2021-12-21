#!usr/bin/env python
# -*- coding: utf-8 -*-
import os
import re
import sys
import fileinput
import traceback
import shutil

def search(root_dir,replace_dict,except_t = ()):
	os.chdir(root_dir)
	for root,dir,files in os.walk('.'):
		root = root.replace('.\\','')
		if root.split('\\')[0] in except_t or root in except_t:
			continue
		for f in files:
			replace_dict.update({f:os.path.join(root,f)})
	# print(replace_dict)


def replace(root_dir,replace_d,except_t = ()):
	os.chdir(root_dir)
	file_src_l = []
	for root,dir,files in os.walk(root_dir):
		if root.split('\\')[0] in except_t or root in except_t:
			continue
		for f in files:
			file_src_l.append(os.path.join(root,f)) if f.endswith('.cpp') or f.endswith('.h') or f.endswith('.c') else None
	# file_src_l = [os.path.join(root,f) for root,dir,files in os.walk(root_dir)
										 # for f in files
										  # if f.endswith('.cpp') or f.endswith('.h') or f.endswith('.c') ]
	for f in file_src_l:
		print('*'*18)
		print(f)
		with open(f,mode = 'rb') as f_obj:
			src_l = []
			try:
				for line in f_obj:
					if '#include' in str(line):
						line = bytes.decode(line)
						tgt_l = re.findall(r'#include "(.*?)"',line)
						if tgt_l:
							tgt = tgt_l[0]
						if tgt in replace_d:
							line = line.replace(tgt,replace_d[tgt])
					if type(line) is bytes:
						src_l.append(line)
					else:
						src_l.append(str.encode(line))
			except KeyboardInterrupt:
				sys.exit()
			except:
				traceback.print_exc()
				input()
		with open(f,mode = 'wb') as f_obj:
			f_obj.write(b''.join(src_l))

			
def find_include(root_dir,except_t = ()):
	include_l = []
	for root,dir,__ in os.walk('.'):
		root = root.replace('.\\','')
		if root.split('\\')[0] in except_t or root in except_t or 'include' in [root,root.split('\\')[0]]:
			continue
		for d in dir:
			include_l.append((root,os.path.join(root,d))) if d.endswith('include') else None
	return include_l

	
def copy_dir(root_dir,except_t = ()):
	os.chdir(root_dir)
	if not os.path.exists('include'):
		os.mkdir('include')
	include_l = find_include(root_dir,except_t)
	# print(include_l)
	for d,inc in include_l:
		for root,__,files in os.walk(inc):
			if root in '.\\include':
				continue
			print('src:',root)
			dir_ = root.replace('\\include','')
			dir_ = os.path.join('include',dir_)
			if not os.path.exists(dir_):
				os.makedirs(dir_)
			print('dist:',dir_)
			for f in files:
				os.system('copy %s %s'%(os.path.join(root,f),dir_))

				
def rename(root_dir,except_t = []):
	os.chdir(root_dir)
	include_l = find_include(root_dir,except_t)
	for __,inc in include_l:
		for root,__,__ in os.walk(inc):
			if '.\\include' in root:
				continue
			# print(root)
			os.rename(root,root+'_remove')


if __name__ == "__main__":
	'''(dir,(except list))'''
	dir_list = [ ('.\\gui\\mgapollo\\apps',('bluetooth')),
				 ('.\\gui\\mgapollo\\inner-res',('')),
				 ('.\\gui\\mgapollo\\resdesc\\nokia-240x320',(''))]
	root_dir = os.getcwd()
	tgt_file_dir = sys.argv[-1]
	tgt_file_dir = os.path.dirname(tgt_file_dir)
	root_dir = os.path.join(root_dir,tgt_file_dir)
	print(root_dir)
	root_dir,__ =root_dir.split('scripts')
	replace_dict = {}
	for dir, except_t in dir_list:
		root_dir_ = os.path.join(root_dir,dir)
		search(root_dir_, replace_dict, except_t)
	replace_dict = {file:dir.replace('src\\','').replace('include\\','').replace('.\\','').replace('\\','/') for file,dir in replace_dict.items()}
	# print(replace_dict)
	# input()
	for root_dir_ in [os.path.join(root_dir,'gui'),os.path.join(root_dir,'inc')]:
		replace(root_dir_,replace_dict)
	for dir, except_t in dir_list:
		root_dir_ = os.path.join(root_dir,dir)
		copy_dir(root_dir_, except_t)
	for dir, except_t in dir_list:
		root_dir_ = os.path.join(root_dir,dir)
		rename(root_dir_, except_t)