#    ICRAR - International Centre for Radio Astronomy Research
#    (c) UWA - The University of Western Australia, 2017
#    Copyright by UWA (in the framework of the ICRAR)
#    All rights reserved
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#    MA 02111-1307  USA

import re

inp_regex = re.compile('%i\[(-[0-9]+)\]')
out_regex = re.compile('%o\[(-[0-9]+)\]')

class BashCommand(object):
    """
    An efficient implementation of the bash command with parameters
    """
    def __init__(self, cmds):
        """
        create the logical form of the bash command line

        cmds: a list such that ' '.join(cmds) looks something like:
                 'python /home/dfms/myclean.py -d %i[-21] -f %i[-3] %o[-2] -v'
        """
        self._input_map = dict() # key: logical drop id, value: a list of physical oids
        self._output_map = dict()
        self._cmds = cmds
        cmd = ' '.join(cmds)
        for m in inp_regex.finditer(cmd):
            self._input_map[int(m.group(1))] = set()
        for m in out_regex.finditer(cmd):
            self._output_map[int(m.group(1))] = set()

    def add_input_param(self, lgn_id, oid):
        lgn_id = int(lgn_id)
        if (lgn_id in self._input_map):
            self._input_map[lgn_id].add(oid)

    def add_output_param(self, lgn_id, oid):
        lgn_id = int(lgn_id)
        if (lgn_id in self._output_map):
            self._output_map[lgn_id].add(oid)

    def to_real_command(self):
        cmds = self._cmds
        for k in range(len(cmds)):
            d = cmds[k]
            if (d.startswith('%i[')):
                lgn_id = int(d[3:-1])
                cmds[k] = ' '.join(['%i[{0}]'.format(x) for x in self._input_map[lgn_id]])
            elif (d.startswith('%o[')):
                lgn_id = int(d[3:-1])
                cmds[k] = ' '.join(['%o[{0}]'.format(x) for x in self._output_map[lgn_id]])
        return ' '.join(cmds)
