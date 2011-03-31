import sys
import os

sys.path.append(os.getcwd() + '/build/lib.win32-2.5')

import pyhelpers

d = pyhelpers.ToolHelp()

for i in d:
    i
    if i[1].lower().find('scite') != -1:
        print i[0]
        pyhelpers.killProcess(i[0])
        #for l in i[2]:
            #print l
            #if l[0].lower().find('lexer') != -1:
            #    print i[0], l[1]
            #    pyhelpers.unloadModule(i[0], l[1])
            #    break
        break
