import os
import ycm_core

flags = ["-std=c11",
         "-lpng",
         "-lm"]

def FlagsForFile(filename, **kwargs):
    return {'flags': flags, 'do_cache': False}

