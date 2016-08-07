#! /bin/sh
# /etc/init.d/pi-timer
# kFreeBSD do not accept scripts as interpreters, using #!/bin/sh and sourcing.
if [ true != "$INIT_D_SCRIPT_SOURCED" ] ; then
    set "$0" "$@"; INIT_D_SCRIPT_SOURCED=true . /lib/init/init-d-script
fi

### BEGIN INIT INFO
# Provides:          pi-timer
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: pi-timer script
# Description:       Used to control the pi-timer
### END INIT INFO

process_id=$(ps -e | grep pi-timer | awk '{print $1}')

# The following part carries out specific functions depending on arguments.
case "$1" in
  start)
    echo "Starting pi-timer..."
    if [ -z "${process_id}" ]; then
        sudo pi-timer --daemon=true --port=8080 --random=true;
    else
        echo "pi-timer is running: ${process_id}";
    fi
    ;;
  stop)
    echo "Stopping pi-timer..."
    if [ -z "${process_id}" ]; then
        echo "pi-timer is not running";
    else
        sudo kill -9 ${process_id};
    fi
    ;;
  *)
    echo "Usage: /etc/init.d/pi-timer {start|stop}"
    exit 1
    ;;
esac

exit 0

# Author: Henry Seurer <henry@gmail.com>

DESC="pi-timer"
DAEMON=/usr/local/bin/pi-timer
