# Start off with defining the printer as a Python object.


class SolutionCtrlBlkPrinter:

    # The constructor takes the value and stores it for later.

    def __init__(self, val):
        self.val = val

    # The to_string method returns the value of the
    # si_signo attribute of the directory.

    def to_string(self):
        instr = str(self.val["_name"]) + " -> " + str(self.val["_str"])
        return instr


class Coord_t_Printer:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        instr = "xy [" + str(self.val["_x"]) + ":" + str(self.val["_y"]) + "]"
        return instr


class Dll_t_Printer:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        instr = (
            "dll ["
            + str(self.val["_first"])
            + " "
            + str(self.val["_last"])
            + " size = "
            + str(self.val["_size"])
            + "]"
        )
        return instr


# Next, define the lookup function that returns the
# printer object when it receives a siginfo_t.

# The function takes the GDB value-type, which, in
# our example is used to look for the siginfo_t.


def solutionCtrlBlkPprinter(val):
    if str(val.type) == "struct solutionCtrlBlock_t":
        return SolutionCtrlBlkPrinter(val)


def Coord_t_printer(val):
    if "coord_t" in str(val.type):
        return Coord_t_Printer(val)


def dll_head_t_printer(val):
    if "dll_head_t *" in str(val.type):
        return Dll_t_Printer(val)


# Finally, append the pretty-printer as object/ function to
# the list of registered GDB printers.

gdb.pretty_printers.append(solutionCtrlBlkPprinter)

# Our pretty-printer is now available when we debug
# the inferior program in GDB.
