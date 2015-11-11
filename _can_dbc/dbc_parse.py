#!/usr/bin/python

import sys, getopt
import re

"""
This parses the Vector DBC file to generate code to marshal and unmarshal DBC defined messages

Use Python (3.5 was tested to work)
python dbc_parse.py -i 243.dbc -s MOTOR > generated_code.c
Generate all code: dbc_parse.py -i 243.dbc -s MOTOR -a all
Generate all code with big endian: dbc_parse.py -i 243.dbc -s DRIVER -a all -b big > generated.h

TODO:
    - Handle muxed CAN messages
    - Handle "FieldType" as enumeration
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
            if not self.is_unsigned:
                _max *= 2

            t = "uint32_t"
            if _max <= 256:
                t = "uint8_t"
            elif _max <= 65536:
                t = "uint16_t"

            if not self.is_unsigned:
                t = t[1:]

            return t

    def get_signal_code(self):
        code = ""
        code += "    " + self.get_code_var_type() + " " + self.name + ";"

        # Align the start of the comments
        for i in range(len(code), 40):
            code += " "

        # Comment with Min/Max
        code += " ///< B" + str(self.bit_start + self.bit_size - 1) + ":" + str(self.bit_start)
        if self.min_val != 0 or self.max_val != 0:
            code += "  Min: " + self.min_val_str + " Max: " + self.max_val_str

        # Comment with destination nodes:
        code += "   Destination: "
        for r in self.recipients:
            if r == self.recipients[0]:
                code += r
            else:
                code += "," + r

        return code + "\n"

    def get_encode_code(self):
        code = ''

        # Min/Max check
        if self.min_val != 0 or self.max_val != 0:
            code += ("    if(from->" + self.name + " < " + self.min_val_str + ") { " + "from->" + self.name + " = " + self.min_val_str + "; }\n")
            code += ("    if(from->" + self.name + " > " + self.max_val_str + ") { " + "from->" + self.name + " = " + self.max_val_str + "; }\n")

        # Compute binary value (both little and big endian)
        # Encode should subtract offset then divide
        code += ("    raw_signal = ((uint64_t)(((from->" + self.name + " - (" + self.offset_str + ")) / " + str(self.scale) + ") + 0.5))")
        code += (" & 0x" + format(2 ** self.bit_size - 1, '02x') + ";\n")

        bit_pos = self.bit_start
        remaining = self.bit_size
        byte_num = int(self.bit_start / 8)
        while remaining > 0:
            if remaining > 8:
                bits_in_this_byte = 8 - (bit_pos % 8)
            else:
                bits_in_this_byte = remaining

            code += ("    bytes[" + str(byte_num) + "] |= (((uint8_t)(raw_signal >> " + str(bit_pos - self.bit_start) + ")")
            code += (" & 0x" + format(2 ** bits_in_this_byte - 1, '02x') + ") << " + str(bit_pos % 8) + ")")
            code += ("; ///< " + str(bits_in_this_byte) + " bit(s) to B" + str(bit_pos) + "\n")
            byte_num += 1

            bit_pos += bits_in_this_byte
            remaining -= bits_in_this_byte
        code += ("\n")
        return code

    def get_decode_code(self):
        # Little and Big Endian:
        bit_pos = self.bit_start
        remaining = self.bit_size
        byte_num = int(self.bit_start / 8)
        bit_count = 0
        code = ("\n")
        code += ("    raw_signal = 0;\n")

        while remaining > 0:
            if remaining > 8:
                bits_in_this_byte = 8 - (bit_pos % 8)
            else:
                bits_in_this_byte = remaining

            code += ("    bits_from_byte = ((bytes[" + str(byte_num) + "] >> " + str(bit_pos % 8) + ")")
            code += ("& 0x" + format(2 ** bits_in_this_byte - 1, '02x') + ")")
            code += ("; ///< " + str(bits_in_this_byte) + " bit(s) from B" + str(bit_pos) + "\n")
            code += ("    raw_signal    |= (bits_from_byte << " + str(bit_count) + ");\n")
            byte_num += 1

            bit_pos += bits_in_this_byte
            remaining -= bits_in_this_byte
            bit_count += bits_in_this_byte

        # Decode/get should multiply then add the offset
        code += ("    to->" + self.name + " = (raw_signal * " + str(self.scale) + ") + (" + self.offset_str + ");\n")

        return code


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

    def is_recipient_of_at_least_one_sig(self, node):
        for s in self.signals:
            if node in s.recipients:
                return True
        return False

    def gen_converted_struct(self, self_node):
        code = ''
        if False == self.is_recipient_of_at_least_one_sig(self_node) and self.sender != self_node:
            code = ("\n/// Not generating '" + self.get_struct_name() + "' since we are not the sender or a recipient of any of its signals")
            return code

        code += ("\n/// Message: " + self.name + " from '" + self.sender + "', DLC: " + self.dlc + " byte(s), MID: " + self.mid + "\n")
        code += ("typedef struct {\n")
        for s in self.signals:
            code += (s.get_signal_code())

        code += ("\n    mia_info_t mia_info;")
        code += ("\n} " + self.get_struct_name() + ";\n")
        return code

    def get_encode_code(self):
        code = ''
        code += ("\n/// Encode " + self.sender + "'s '" + self.name + "' message\n")
        code += ("/// @returns the message header of this message\n")
        code += ("static msg_hdr_t " + self.get_struct_name()[:-2] + "_encode(uint64_t *to, " + self.get_struct_name() + " *from)\n")
        code += ("{\n")
        code += ("    *to = 0; ///< Default the entire destination data with zeroes\n")
        code += ("    uint8_t *bytes = (uint8_t*) to;\n")
        code += ("    uint64_t raw_signal;\n")
        code += ("\n")

        for s in self.signals:
            code += s.get_encode_code()

        code += ("    return " + self.get_struct_name()[:-2] + "_HDR;\n")
        code += ("}\n")
        return code

    def get_decode_code(self):
        code = ''
        code += ("\n/// Decode " + self.sender + "'s '" + self.name + "' message\n")
        code += ("/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check\n")
        code += ("static inline bool " + self.get_struct_name()[:-2] + "_decode(" + self.get_struct_name() + " *to, const uint64_t *from, const msg_hdr_t *hdr)\n")
        code += ("{\n")
        code += ("    const bool success = true;\n")
        code += ("    if (NULL != hdr && (hdr->dlc != " + self.get_struct_name()[:-2] + "_HDR.dlc || hdr->mid != " + self.get_struct_name()[:-2] + "_HDR.mid)) {\n")
        code += ("        return !success;\n")
        code += ("    }\n")
        code += ("    uint64_t raw_signal;\n")
        code += ("    uint64_t bits_from_byte;\n")
        code += ("    const uint8_t *bytes = (const uint8_t*) from;\n")

        for s in self.signals:
            code += s.get_decode_code()

        code += ("\n")
        code += ("    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter\n")
        code += ("    return success;\n")
        code += ("}\n")
        return code


class DBC(object):
    def __init__(self, name):
        self.name = name
        self.messages = []
        self.nodes = []

    def gen_msg_hdr_struct(self):
        code = ("/// CAN message header structure\n")
        code += ("typedef struct { \n")
        code += ("    uint32_t mid; ///< Message ID of the message\n")
        code += ("    uint8_t  dlc; ///< Data length of the message\n")
        code += ("} msg_hdr_t; \n")
        return code

    def gen_msg_hdr_instances(self):
        code = ''
        for m in self.messages:
            code += ("static const msg_hdr_t " + (m.get_struct_name()[:-2] + "_HDR = ").ljust(32 + 7))
            code += ("{ " +  str(m.mid).rjust(4) + ", " + m.dlc + " };\n")
        return code

    def gen_mia_struct(self):
        code = ("/// Missing in Action structure\n")
        code += ("typedef struct {\n")
        code += ("    uint32_t is_mia : 1;          ///< Missing in action flag\n")
        code += ("    uint32_t mia_counter_ms : 31; ///< Missing in action counter\n")
        code += ("} mia_info_t;\n")
        return code

def main(argv):
    dbcfile = '243.dbc'
    self_node = 'DRIVER'
    gen_all = True
    big_endian = True

    try:
        opts, args = getopt.getopt(argv, "hi:s:a:b", ["ifile=", "self=", "all"])
    except getopt.GetoptError:
        print ('dbc_parse.py -i <dbcfile> -s <self_node> <-a> <-b>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('dbc_parse.py -i <dbcfile> -s <self_node> <-a> <-b>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            dbcfile = arg
        elif opt in ("-s", "--self"):
            self_node = arg
        elif opt in ("-a", "--all"):
            gen_all = True
        elif opt in ("-b", "--big"):
            big_endian = True

    # Parse the DBC file
    dbc = DBC(dbcfile)
    f = open(dbcfile, "r")
    while 1:
        line = f.readline()
        if not line:
            break

        # Nodes in the DBC file
        if line.startswith("BU_:"):
            nodes = line.strip("\n").split(' ')
            dbc.nodes = (nodes[1:])
            if self_node not in dbc.nodes:
                print ('/////////////////////////////// ERROR /////////////////////////////////////')
                print ('#error "Self node: ' + self_node + ' not found in _BU nodes in the DBC file"')
                print ('/////////////////////////////// ERROR /////////////////////////////////////')
                print ('')

        # Start of a message
        if line.startswith("BO_ "):
            tokens = line.split(' ')
            msg = Message(tokens[1], tokens[2].strip(":"), tokens[3], tokens[4].strip("\n"))
            dbc.messages.append(msg)

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
            dbc.messages[-1].add_signal(sig)

    print ("/// DBC file: %s    Self node: %s" % (dbcfile, self_node))
    print ("/// This file should be included by a source file, for example: #include \"generated.c\"")
    print ("#include <stdbool.h>")
    print ("#include <stdint.h>")
    print ("\n\n")

    # Generate header structs and MIA struct
    print (dbc.gen_mia_struct())
    print (dbc.gen_msg_hdr_struct())
    print (dbc.gen_msg_hdr_instances())

    # Generate converted struct types for each message
    for m in dbc.messages:
        print (m.gen_converted_struct(self_node))

    # Generate MIA handler "externs"
    print ("\n/// These 'externs' need to be defined in a source file of your project")
    for m in dbc.messages:
        if gen_all or m.is_recipient_of_at_least_one_sig(self_node):
            print ("extern const uint32_t " + m.name + "__MIA_MS;")
            print ("extern const " + m.get_struct_name() + " " + m.name + "__MIA_MSG;")

    # Generate encode methods
    for m in dbc.messages:
        if not gen_all and m.sender != self_node:
            print ("\n/// Not generating code for " + m.get_struct_name()[:-2] + "_encode() since the sender is " + m.sender + " and we are " + self_node)
        else:
            print (m.get_encode_code())

    # Generate decode methods
    for m in dbc.messages:
        if not gen_all and not m.is_recipient_of_at_least_one_sig(self_node):
            print ("\n/// Not generating code for " + m.get_struct_name()[:-2] + "_decode() since we are not the recipient of any of its signals")
        else:
            print (m.get_decode_code())

    # Generate MIA handler for the dbc.messages we are a recipient of
    for m in dbc.messages:
        if not gen_all and not m.is_recipient_of_at_least_one_sig(self_node):
            continue

        print ("\n/// Handle the MIA for " + m.sender + "'s '" + m.name + "' message")
        print ("/// @param   time_incr_ms  The time to increment the MIA counter with")
        print ("/// @returns true if the MIA just occurred")
        print ("/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set")
        print ("static inline bool " + m.get_struct_name()[:-2] + "_handle_mia(" + m.get_struct_name() + " *msg, uint32_t time_incr_ms)")
        print ("{")
        print ("    bool mia_occurred = false;")
        print ("    const mia_info_t old_mia = msg->mia_info;")
        print ("    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= " + m.name + "__MIA_MS);")
        print ("")
        print ("    if (!msg->mia_info.is_mia) { ")
        print ("        msg->mia_info.mia_counter_ms += time_incr_ms;")
        print ("    }")
        print ("    else if(!old_mia.is_mia)   { ")
        print ("        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten")
        print ("        *msg = " + m.name + "__MIA_MSG;")
        print ("        msg->mia_info.mia_counter_ms = " + m.name + "__MIA_MS;")
        print ("        msg->mia_info.is_mia = true;")
        print ("        mia_occurred = true;")
        print ("    }")
        print ("\n    return mia_occurred;")
        print ("}")


if __name__ == "__main__":
    main(sys.argv[1:])