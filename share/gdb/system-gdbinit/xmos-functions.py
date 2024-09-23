import gdb


class ListDevices(gdb.Command):
    """List the xmos devices that are connected to the system."""

    def __init__(self):
        super(ListDevices, self).__init__("listdevices", gdb.COMMAND_SUPPORT, gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        gdb.execute("connect --list-devices", from_tty)


if __name__ == "__main__":
    ListDevices()
