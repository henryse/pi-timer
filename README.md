# Raspberry PI Timer

Ths is the first in hopefully a series fo simple tools for managing Raspberry PIs.

## Introduction

The purpose of this application is to allow you to turn a pin on the GPIO on and off based on two simple timers:

* Sleep Time
* Run Time

The tool supports a simple diagnostic port that can viewed in a browser.

## Setup

YOu will need to install the following two items on your Raspberry PI to build this application:

* [CMake 3.0+](https://cmake.org)
* [WiringPI](http://wiringpi.com)

### [CMake 3.0+](https://cmake.org)

To [install CMake](https://cmake.org/install/) on the Raspberry PI do the following:

```

    wget https://cmake.org/files/v3.5/cmake-3.5.2.tar.gz
    
    tar -xvf cmake-3.5.2.tar.gz
    
    cd cmake-3.5.2
    
    ./bootstrap
    
    make
    
    sudo make install

```

### [WiringPI](http://wiringpi.com)

To install WiringPI please see this [link](http://wiringpi.com/download-and-install/).

### Building pi-timer

Once you have installed the above items, simply do the following:

```

    git clone https://github.com/henryse/pi-timer.git
    
    cd pi-timer
    
    cmake .
    
    make
    
    cd output
    
    ./pi-timer --help

```

## Configuration

The following command line options are available:

* run        number of minutes to run, default: 5
* sleep      number of minutes to sleep, default: 55
* gpio       gpio pin to signal fan, default: 4
* daemon     run as daemon, default: false
* port       port to listen to, default: 9080
* help       get help message

## Web Server Port

The debug port supports the following commands:

* GET http://pi-timer-address/            -  Show a diagnostic web page with configuration data. 
* GET http://pi-timer-address/health      -  Health Status of the process, is it running?
* GET http://pi-timer-address/buildInfo   -  Build JSON document about the version of the process.
