#! /bin/sh
# /etc/init.d/pi-timer

# The following part always gets executed.
echo "Starting pi-timer"

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