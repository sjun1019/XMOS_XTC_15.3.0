import gdb

class XmosVectorPrinter:
    # TODO: Put these color codes back in, somehow not inserting them if printing to file
    ANSI_CYAN = "" # "\033[0;36m"
    ANSI_RESET = "" # "\033[0m"

    """Prints an xmos RV Vector in the form which is currently configured in vCtrl."""
    def __init__(self, val):
        self.val = val
        self.vctrl = gdb.selected_frame().read_register("vCtrl")

    def _vector_type(self):
        type = (self.vctrl >> 8) & 3
        if type == 0: return 'v8_int32'
        if type == 1: return 'v16_int16'
        if type == 2: return 'v32_int8'
        return 'v32_int8' #default

    def to_string(self):
        vtype = self._vector_type()
        return (self.ANSI_CYAN +
                "uint256: " +
                self.ANSI_RESET +
                self.val["uint256"].format_string() +
                "  " +
                self.ANSI_CYAN +
                vtype +
                ": " +
                self.ANSI_RESET +
                self.val[vtype].format_string())

def xmos_build_pretty_printers():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("Xmos")
    pp.add_printer("XmosVectorPrinter", "^builtin_type_xmos_vreg$", XmosVectorPrinter)
    pp.add_printer("XmosVectorPrinter", "^builtin_type_vec256i$", XmosVectorPrinter)
    return pp

def xmos_init():
    """Initialize debugger environment for xmos processors.
    """
    gdb.printing.register_pretty_printer(gdb.current_objfile(), xmos_build_pretty_printers(), True)

if __name__ == "__main__":
    xmos_init()
