import sys
import os

sys.path.append(os.getcwd() + '/build/lib.win32-2.5')

import pyhelpers

d = pyhelpers.ToolHelp()

for i in d:
    print i