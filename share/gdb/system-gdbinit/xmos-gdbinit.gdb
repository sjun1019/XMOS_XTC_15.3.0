
# Run all xcores at the same time (when using 'continue' etc)
set schedule-multiple on

# Enable the xmos-specific overriding of certain commands - such as enabling 'file' to open xe files
set cosmetic-command on

# Enable automatically setting the exception catchpoint, so the processor halts and reports when there is an exception
set xcore-catchpoints on

# If one core completes, don't automatically halt the others - keep going (this is user-preference)
set silent-exit on

# Our xGDBserver doesn't drop packets & simulators can be slow, so retrying just clogs the queue & loses state - set timeout to 1 hour
set remotetimeout 3600
