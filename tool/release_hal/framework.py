import os
import subprocess
import time
from subprocess import Popen, PIPE,STDOUT
import shutil
import stat
import sys

#arg_crane_dir = 'D:\\yazhou\\svn\\R2029_test\\crane'
#arg_crane_dir = 'D:\\yazhou\\svn\\R1479_test\\crane'
#arg_libhal_dir = 'D:\yazhou\svn\R2029_test\crane\build\rel\build\crane_evb_z2'
#arg_output_dir = "output"

arg_crane_dir = sys.argv[1]
arg_libhal_dir = sys.argv[2]
arg_output_dir = sys.argv[3]

print("hello world");
cur_dir = os.getcwd()
#here set shell dir....must be set
# cur_dir = 'D:/yazhou/svn/R2029_test/framework_tool'
os.chdir(cur_dir)
cur_dir = os.getcwd()
print(cur_dir)
time.sleep(1)
print('step1 -----------------------------------------------------------')
print('copy framework to output dir')
def delete_file(filePath):
 if os.path.exists(filePath):
  for fileList in os.walk(filePath):
   for name in fileList[2]:
    os.chmod(os.path.join(fileList[0],name), stat.S_IWRITE)
    os.remove(os.path.join(fileList[0],name))
  shutil.rmtree(filePath)
  return "delete ok"
 else:
  return "no filepath"
dst_dir = arg_output_dir+'/framework'
if os.path.exists(dst_dir):
    delete_file(dst_dir)
cmd = 'mkdir '+arg_output_dir+'\\framework'
os.system(cmd)
cmd = 'xcopy /E framework '+ arg_output_dir+'\\framework'
print(cmd)
os.system(cmd)

print('step2 -----------------------------------------------------------')
print('copy framework to framework')
cmd = 'xcopy /U/E/Y '+ arg_crane_dir+'\\hal '+ arg_output_dir+'\\framework'
print(cmd)
os.system(cmd)
cmd = 'xcopy /U/E/Y '+ arg_crane_dir+'\\inc '+ arg_output_dir+'\\framework\\inc'
print(cmd)
os.system(cmd)
cmd = 'xcopy /U/E/Y '+ arg_crane_dir+'\\cus '+ arg_output_dir+'\\framework\\cus'
print(cmd)
os.system(cmd)
print('step3 -----------------------------------------------------------')
filename = 'set_uuid.txt'
filename2 = arg_output_dir+'\\framework\\os\\ui_os_entry.c'
f2 = open(filename2, 'a')
with open(filename) as file_object:
    for line in file_object:
        f2.write(line)
print('step4 -----------------------------------------------------------')
print('lcd_test.c do not change')
cmd = 'xcopy /E/y framework\\test\\lcd\\lcd_test.c '+ arg_output_dir+'\\framework\\test\\lcd\\lcd_test.c'
print(cmd)
os.system(cmd)
# cmd = 'xcopy /E/y '+arg_libhal_dir+' '+ arg_output_dir+'\\framework\\lib'
# print(cmd)
# os.system(cmd)
libhal = os.path.join(arg_output_dir,"framework","lib","libhal.a")
print("copy %s to %s"%(arg_libhal_dir,libhal))
shutil.copy2(arg_libhal_dir,libhal)
#shutil.copy('autoconf.h', arg_output_dir+'\\framework\\inc')
print('end end----------------------------------------------------------')
sys.exit()