# python code that copies PID_esp.ino from C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Bronbestanden\PID\PID_esp
# to C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Teamwinnaar VS code\src and convert the file name of PID_esp.ino to PID_esp.cpp

import shutil
import os

def copyfile():
    shutil.copy2(
        r"C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Bronbestanden\PID\PID_esp\PID_esp.ino",
        r"C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Teamwinnaar VS code\src\main.cpp")
    print("File copied and renamed")


def deletefile():
    os.remove(r"C:\Users\eigen\OneDrive - Quadraam\PWS\Teamwinnaar-git\Teamwinnaar\Teamwinnaar VS code\src\main.cpp")
    print("File deleted")

deletefile()
copyfile()

