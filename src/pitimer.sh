#!/usr/bin/env bash
# /etc/init.d/pi-timer

function log_message {
    echo $1;
    logger -p info $1;
}

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
    log_message "Starting pi-timer...";
    if [ -z "${process_id}" ]; then
        log_message "pi-timer starting up";
        /bin/bash -c "/usr/local/bin/pi-timer --daemon=true --port=8080 --random=true";
    else
        log_message "pi-timer is already running: ${process_id}";
    fi
    ;;
  stop)
    log_message "Stopping pi-timer...";

    if [ -z "${process_id}" ]; then
        log_message "pi-timer is not running";
    else
        log_message "Killing ${process_id}";
        kill ${process_id}
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
