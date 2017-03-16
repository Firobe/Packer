#!/usr/bin/env python2 
import inkex
import cubicsuperpath, simplestyle, copy, math, re, bezmisc
from subprocess import call
import sys

class Packer(inkex.Effect):
    def __init__(self):
        inkex.Effect.__init__(self)
        self.OptionParser.add_option("--width",
                        action="store", type="int", 
                        dest="width", default=0,
                        help="Le caca partout")
        self.OptionParser.add_option("--height",
                        action="store", type="int", 
                        dest="height", default=0,
                        help="Le caca partout")
        self.OptionParser.add_option("--dup",
                        action="store", type="inkbool", 
                        dest="dup", default=0,
                        help="Le caca partout")
        self.OptionParser.add_option("--buffer",
                        action="store", type="int", 
                        dest="buffer", default=0,
                        help="Le caca partout")
        self.OptionParser.add_option("--display",
                        action="store", type="inkbool", 
                        dest="display", default=0,
                        help="Le caca partout")
        self.OptionParser.add_option("--conf",
                        action="store", type="string", 
                        dest="conf", default="default.ce",
                        help="Le caca partout")
        self.OptionParser.add_option("--tab",
                        action="store", type="string", 
                        dest="tab", default="def",
                        help="Le caca partout")
        self.OptionParser.add_option("--defWidth",
                        action="store", type="inkbool", 
                        dest="defWidth", default=1,
                        help="Use default width")
        self.OptionParser.add_option("--defHeight",
                        action="store", type="inkbool", 
                        dest="defHeight", default=1,
                        help="Use default height")
    def effect(self):
        t = []
        for a in sys.argv[1:]:
            if(not ("--width" in a and self.options.defWidth or "--height" in a and self.options.defHeight)):
                t += [a];
            else:
                sys.stderr.write("Utilisation de la valeur par defaut");
        call(["./packer"] + t)

if __name__ == '__main__':
    e = Packer()
    e.affect()
