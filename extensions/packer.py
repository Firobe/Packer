#!/usr/bin/env python2 
import inkex
import cubicsuperpath, simplestyle, copy, math, re, bezmisc
from subprocess import call
import sys

class Packer(inkex.Effect):
    def __init__(self):
        inkex.Effect.__init__(self)
        self.OptionParser.add_option("--width",
                        action="store", type="string", 
                        dest="width", default=0,
                        help="Une option")
        self.OptionParser.add_option("--height",
                        action="store", type="string", 
                        dest="height", default=0,
                        help="Une option")
        self.OptionParser.add_option("--dup",
                        action="store", type="inkbool", 
                        dest="dup", default=0,
                        help="Une option")
        self.OptionParser.add_option("--buffer",
                        action="store", type="int", 
                        dest="buffer", default=0,
                        help="Une option")
        self.OptionParser.add_option("--display",
                        action="store", type="inkbool", 
                        dest="display", default=0,
                        help="Une option")
        self.OptionParser.add_option("--conf",
                        action="store", type="string", 
                        dest="conf", default="default.ce",
                        help="Une option")
        self.OptionParser.add_option("--tab",
                        action="store", type="string", 
                        dest="tab", default="def",
                        help="Une option")
        self.OptionParser.add_option("--defWidth",
                        action="store", type="inkbool", 
                        dest="defWidth", default=1,
                        help="Use default width")
        self.OptionParser.add_option("--defHeight",
                        action="store", type="inkbool", 
                        dest="defHeight", default=1,
                        help="Use default height")
    def effect(self):
        #Raw width/height
        swidth = self.document.getroot().get('viewBox').split()[2] if self.options.defWidth else self.options.width
        sheight = self.document.getroot().get('viewBox').split()[3] if self.options.defHeight else self.options.height
        try:
            #If raw int, do not convert
            width = int(swidth)
        except ValueError:
            #If contains units, convert
            width = int(self.unittouu(swidth))
        try:
            height = int(sheight)
        except ValueError:
            height = int(self.unittouu(sheight))
        sys.stderr.write("Width (px) : " + str(width));
        sys.stderr.write("Height (px) : " + str(height));
        t = []
        #Replace inkscape parameters by computed ones
        for a in sys.argv[1:]:
            if("--width" in a):
                t.append("--width=" + str(width));
            elif("--height" in a):
                t.append("--height=" + str(height));
            else:
                t += [a];
        #Call packer with parameters
        call(["./packer"] + t)

if __name__ == '__main__':
    e = Packer()
    e.affect()
