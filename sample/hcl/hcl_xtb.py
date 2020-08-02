#!/usr/bin/env python3
"""
A tool to generate potential energy curve for HCl, 
using an installed [xtb](https://github.com/grimme-lab/xtb/) packaged 
developed in Dr. Grimme's group.

A user of Quick 1D DVR typically does not need to use this script,
it is for testing (to give a good PEC very quickly, I am just tired of 
find an analytical PEC online).

For diatomic system, the user typically need a more accurate PEC,
definitely not from xtb, which is designed as a method for much larger systems.
I suggest a coupled cluster one for electronic ground state.

"""
import numpy as np
import os

with open(f"sample_hcl.inp","w") as pec:
    pec.write('''\
mass 0.972                  # <reduced mass in amu>   
nlevels 7                   # <number of vibrational states wanted>

pec                         # <r in Angstrom>   <potential in cm-1>
''')
    for r in np.arange(0.6, 2.5, 0.01):
        with open(f"{r:4.2f}.xyz","w") as f:
            f.write("2\n\n")
            f.write(f"""\
            Cl 0.00 0.00 0.00
            H  0.00 0.00 {r:4.2f} """)
        out = (float(os.popen(\
                f'xtb {r:4.2f}.xyz 2>/dev/null | grep "TOTAL ENERGY"'\
                ).read().split()[3])\
                +5.052813509006)*219474.6313702
        pec.write(f"{r:4.2f}  {out:20.5f}\n")
    pec.write('endpec\n')
    
os.popen("rm -f *.xyz charges wbo xtbrestart xtbtopo.mol")
        
