#! /bin/sh
# /etc/init.d/pi-timer-start

# The following part always gets executed.
echo "Starting pi-timer"

# The following part carries out specific functions depending on arguments.
case "$1" in
  start)
    echo "Starting pi-timer..."
    sudo pi-timer --daemon=true --port=8080 --random=true
    ;;
  stop)
    echo "Starting pi-timer..."
    sudo kill -9 $(ps -ax | grep pi-timer | awk '{print $1, $5}' | grep pi-timer | awk '{print $1}')
    ;;
  *)
    echo "Usage: /etc/init.d/pi-timer-start {start|stop}"
    exit 1
    ;;
esac

exit 0