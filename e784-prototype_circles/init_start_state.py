#!/usr/bin/env python
# $Id: init_start_state.py 1613 2009-03-25 14:38:18Z lsc $
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Nov 2009
#
# Description:
#    Quick hack to create start states (0.xml) needed by the ABM_EXP model
#
import re
import os
import sys
from math import pi
from random import random

outfile = "0.xml"
radius  = 2.0

# We expect 4 input arguments
if len(sys.argv) != 4:
    print >> sys.stderr, "\nUsage: %s <width> <height> <agent_count>" % sys.argv[0]
    print >> sys.stderr, """More info:
    <width> and <height> will determine the size of simulation space in which
    agents will be randomly placed in.

    <agent_count> specifies the number of agents to initialise.
    """ 
    sys.exit(1)


width, height, acount  = map(int, sys.argv[1:])


if acount < 4:
    print >> sys.stderr, "Error: There must be at least 5 agents"
    sys.exit(1)


# Open file and write preamble
print "Writing to file %s ... " % outfile,
f = open(outfile, "w")
f.write("<states>\n   <itno>0</itno>\n")

# write out agent data
for id in xrange(0,acount):
    
    x = width/4 * (random() - 0.5)
    y = height/4 * (random() - 0.5)

    # write agent values to file
    f.write("""
    <xagent>
        <name>Circle</name>
        <id>%d</id>
        <x>%.2f</x>
        <y>%.2f</y>
        <fx>0.00</fx>
        <fy>0.00</fy>
        <radius>%.2f</radius>
    </xagent>
""" % (id, x, y, radius,))

# End XML file and close
f.write("</states>\n")
f.close();

print "DONE"

