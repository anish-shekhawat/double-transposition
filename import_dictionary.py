#!/usr/bin/env python

"""import_dictionary.py: Imports key-size n-gram words from a collection of English n-gram words"""

import sys
import os

if not os.path.exists('.\dictionary'):
    os.makedirs('.\dictionary')

for i in range(2,6):
    infile = 'w' + str(i) + '_.txt'
    outfile = '.\dictionary\w' + str(i) + '.txt'

    if os.path.exists(outfile):
        os.remove(outfile)

    fin = open(infile, "r")
    fout = open(outfile, "w")

    for x in fin.readlines():
        line = x.split('\t')
        if (len(line) == 3):
            line = [w.replace('-', '') for w in line[1:3]]
            length = sum(len(s) for s in line[:2])
            if (length == 19):
                fout.write(''.join(line[:3]))
        elif (len(line) == 4):
            line = [w.replace('-', '') for w in line[1:4]]
            length = sum(len(s) for s in line[:4])
            if (length == 19):
                fout.write(''.join(line[:4]))
        elif (len(line) == 5):
            line = [w.replace('-', '') for w in line[1:5]]
            length = sum(len(s) for s in line[:5])
            if (length == 19):
                fout.write(''.join(line[:5]))
        elif (len(line) == 6):
            line = [w.replace('-', '') for w in line[1:6]]
            length = sum(len(s) for s in line[:6])
            if (length == 19):
                fout.write(''.join(line[:6]))

    print str(i) + '-gram data extracted.'

    fin.close()
    fout.close()

