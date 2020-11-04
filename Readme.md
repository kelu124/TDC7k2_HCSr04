# Trying to have TDC7200 and HCSR04 run togethers.

## Hardware

* tdc7200evm
* hcsr04
* breadboard and wires/jumpers
* m5stack
* 5V-3.3V bidirectional level shifter (eg HW-221 )

## Present

* main: main arduino code
* backup of the libs
* TDCtests= auto test (without HCSR04)

## Ongoing issues

Get good reading from the TDC7200 :p

## Arduino Setup

![](/m5stack_params.png)

## Physical connections

Just need to but a birectional level translator between HCSR04 (5V) and the rest of the world (3.3V).

### Setup 

![](P_20201104_104838.jpg)
