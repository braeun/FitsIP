# This Python file uses the following encoding: utf-8

# This Pyhton files scans the script directory, loads all images with the
# extension CR2 and saves them as FITS images

import fits
import os

path = fits.scriptpath
fits.set_cwd(path)

file_list = []
obj = os.scandir(path)
for entry in obj:
    if entry.is_file():
        filename, extension = os.path.splitext(entry.name)
        if extension.lower() == '.cr2':
          img = fits.load(entry.name)
          newname = filename + ".fts"
          ret = fits.save(img,newname)
          if ret:
            print("saved ",entry.name," as ",newname)
          else:
            print("FAILED to save ",entry.name," as ",newname)
