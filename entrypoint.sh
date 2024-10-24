bash
#!/bin/bash
set -e

# Run the make command (or any other command you want to execute)
if [ -n "$1" ]; then
    # Run the command passed to the entrypoint (e.g., a specific make target)
    exec "$@"
else
    # Default action: run 'make' command
    make all
fi
