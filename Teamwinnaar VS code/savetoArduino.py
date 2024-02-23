# python code that replaces PID_esp.ino from C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Teamwinnaar VS code\src and convert the file name of main.cpp to PID_esp.ino
  
# to C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Bronbestanden\PID\PID_esp

import shutil
import os

def copyfile():
    shutil.copy2(
        r"C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Teamwinnaar VS code\src\main.cpp",
        r"C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Bronbestanden\PID\PID_esp\PID_esp.ino")
    print("File copied and renamed")

copyfile()