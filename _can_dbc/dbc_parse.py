#!/usr/bin/python

import sys, getopt
import re

"""
ALPHA VERSION!  NOT TESTED!
This parses the Vector DBC file to generate code to marshal and unmarshal DBC defined messages
"""

class Signal(object):
    def __init__(self, name, bit_start, bit_size, is_unsigned, scale, offset, min_val, max_val, recipients):
        self.name = name
        self.bit_start = int(bit_start)
        self.bit_size = int(bit_size)
        self.is_unsigned = is_unsigned

        self.offset = float(offset)
        self.offset_str = offset
        self.scale = float(scale)
        self.scale_str = scale
        self.min_val = float(min_val)
        self.min_val_str = min_val
        self.max_val = float(max_val)
        self.max_val_str = max_val

        self.recipients = recipients

    def get_code_var_type(self):
        if '.' in self.scale_str:
            return "float"
        else:
            _max = (2 ** self.bit_size) * self.scale
            t = "uint32_t"
            if _max <= 256:
                t = "uint8_t"
            elif _max <= 65536:
                t = "uint16_t"

            if not self.is_unsigned:
                t = t[1:]

            return t


class Message(object):
    """
    Message Object that contains the list of signals inside
    """

    def __init__(self, mid, name, dlc, sender):
        self.mid = mid
        self.name = name
        self.dlc = dlc
        self.sender = sender
        self.signals = []

    def add_signal(self, s):
        self.signals.append(s)

    def get_struct_name(self):
        return "%s_TX_%s_t" % (self.sender, self.name)


def main(argv):
    dbcfile = ''
    outputfile = ''

    try:
        opts, args = getopt.getopt(argv, "hi:o:", ["ifile=", "ofile="])
    except getopt.GetoptError:
        print 'dbc_parse.py -i <dbcfile> -o <outputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'test.py -i <dbcfile> -o <outputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            dbcfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg

    print "/// DBC file: %s    Generated file: %s" % (dbcfile, outputfile)

    messages = []
    f = open(dbcfile, "r")
    while 1:
        line = f.readline()
        if not line:
            break

        # Start of a message
        if line.startswith("BO_ "):
            tokens = line.split(' ')
            msg = Message(tokens[1], tokens[2].strip(":"), tokens[3], tokens[4].strip("\n"))
            messages.append(msg)

        # Signals
        if line.startswith(" SG_ "):
            t = line.split(' ')

            # Split the bit start and the bit size
            s = re.split('[|@]', t[4])
            bit_start = s[0]
            bit_size = s[1]
            is_unsigned = '+' in s[2]

            # Split (0.1,1) to two tokens by removing the ( and the )
            s = t[5][1:-1].split(',')
            scale = s[0]
            offset = s[1]

            # Split the [0|0] to min and max
            s = t[6][1:-1].split('|')
            min_val = s[0]
            max_val = s[1]

            recipients = t[8].strip('\n').split(',')

            # Add the signal the last message object
            sig = Signal(t[2], bit_start, bit_size, is_unsigned, scale, offset, min_val, max_val, recipients)
            messages[-1].add_signal(sig)

    # Generate converted struct types for each message
    for m in messages:
        print "\n/// From '" + m.sender + "', DLC: " + m.dlc + " byte(s), MID: " + m.mid
        print "typedef struct {"
        for s in m.signals:
            print get_signal_code(s)
        print "} " + m.get_struct_name() + ";"

    # Generate marshal methods
    for m in messages:
        print "\nstatic inline void marshal_" + m.get_struct_name()[:-2] + "(uint64_t *to, " + m.get_struct_name() + " *from)"
        print "{"
        print "    uint64_t tmp = 0;"
        print "    *to = 0; ///< Default the entire destination data with zeroes"
        for s in m.signals:
            print "\n    // Set: " + s.name
            # Min/Max check
            if s.min_val != 0 or s.max_val != 0:
                print "    if(from->" + s.name + " < " + s.min_val_str + ") { " + "from->" + s.name + " = " + s.min_val_str + "; }"
                print "    if(from->" + s.name + " > " + s.max_val_str + ") { " + "from->" + s.name + " = " + s.max_val_str + "; }"

            # Compute binary value
            print "    tmp = (uint64_t) ((from->" + s.name + " - (" + s.offset_str + ")) / " + s.scale_str + " + 0.5);"
            print "    *to |= (tmp << " + str(s.bit_start) + ");"
        print "}"

    # Generate unmarshal methods
    for m in messages:
        print "\nstatic inline void unmarshal_" + m.get_struct_name()[:-2] + "(" + m.get_struct_name() + " *to, const uint64_t *from)"
        print "{"
        for s in m.signals:
            print "    to->" + s.name + " =",
            print " (((*from >> " + str(s.bit_start) + ") & 0x" + format(2 ** s.bit_size - 1, '02x') + ")",
            print " * " + str(s.scale) + ") + (" + s.offset_str + ");"
        print "}"


def get_signal_code(s):
    code = ""
    # code += "    " + str(s.__dict__)

    code += "    " + s.get_code_var_type() + " " + s.name + ";"

    # Align the start of the comments
    for i in range(len(code), 40):
        code += " "

    # Comment with Min/Max
    code += " ///< B" + str(s.bit_start + s.bit_size - 1) + ":" + str(s.bit_start)
    if s.min_val != 0 or s.max_val != 0:
        code += "  Min: " + s.min_val_str + " Max: " + s.max_val_str

    # Comment with destination nodes:
    code += "   Destination: "
    for r in s.recipients:
        if len(s.recipients) == 1:
            code += s.recipients[0]
        else:
            code += "," + r

    return code


if __name__ == "__main__":
    main(sys.argv[1:])
