# Raspberry PI Timer

Ths is the first in hopefully a series fo simple tools for managing Raspberry PIs.

## Introduction

The purpose of this application is to allow you to turn a pin on the GPIO on and off based on two simple timers:

* Sleep Time
* Run Time

The tool supports a simple diagnostic port that can viewed in a browser.

We use this tool to control a fan, but it could be used to control anything.   
We used a the following:

* [Power switch tail 2](https://www.adafruit.com/products/268) 
* [Raspberry PI](https://www.adafruit.com/products/3055) Model 2 or 3
* USB power connector and USB cable for power

The model 3 is nice because it has WiFi, so you can use the debug http port to view the web page to monitor the timer's status.

## Setup

YOu will need to install the following two items on your Raspberry PI to build this application:

* [CMake 3.0+](https://cmake.org)
* [WiringPI](http://wiringpi.com)

### [CMake 3.0+](https://cmake.org)

The project uses CMake 3.0.  The following is an example of how to install v3.5... but you really only need version 3.0.

To [install CMake](https://cmake.org/install/) on the Raspberry PI do the following:

```

    wget https://cmake.org/files/v3.5/cmake-3.5.2.tar.gz
    
    tar -xvf cmake-3.5.2.tar.gz
    
    cd cmake-3.5.2
    
    ./bootstrap
    
    make
    
    sudo make install

```

Sit back and wait.....  It will take a bit to build the whole thing.


### [WiringPI](http://wiringpi.com)

To install WiringPI please see this [link](http://wiringpi.com/download-and-install/).

### Building pi-timer

Once you have installed the above items, simply do the following:

```

    git clone https://github.com/henryse/pi-timer.git

    cd pi-timer

    make build

    cd build/Debug/output

    ./pi-timer --help

    make clean    

```

## Configuration

The following command line options are available:

* run        number of minutes to run, default: 5
* sleep      number of minutes to sleep, default: 55
* gpio       gpio pin to signal fan, default: 4
* daemon     run as daemon, default: false
* port       port to listen to, default: 9080
* random     use random time offset to turn on pin. default: false
* up_time    up time specifies 24 hour range where pin can be on, default: 9-18
* help       get help message

## Web Server Port

The debug port supports the following commands:

* GET http://pi-timer-address/            -  Show a diagnostic web page with configuration data. 
* GET http://pi-timer-address/health      -  Health Status of the process, is it running?
* GET http://pi-timer-address/buildInfo   -  Build JSON document about the version of the process.
