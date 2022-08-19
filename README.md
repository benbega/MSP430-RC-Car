**Introduction**

This is a "remote controlled" car (not wireless) based on the [MSP430 platform](https://www.ti.com/microcontrollers-mcus-processors/microcontrollers/msp430-microcontrollers/overview.html). An [MSP430-2355](https://www.ti.com/product/MSP430FR2355) acts as a master device, and sends signals to an [MSP430-2310](https://www.ti.com/product/MSP430FR2310) over the integrated [I2C](https://en.wikipedia.org/wiki/I%C2%B2C) modules. The slave MSP430 then sends signals to an [L293D Quadruple Half-H Driver](https://www.ti.com/product/L293D) chip, which controls the 2 DC motors. The slave device also sends control signals to a servo, which steers the front two wheels of the car.

A circuit diagram, consisting of the basic inputs, outputs, and hardware connections, is included in the repository.

**Master Devices**
On startup, the MSP430-2355 initializes it's I2C hardware module to function as a Master device, and gets assigned an address for future communications. The MCU also initializes input and output pins, including 4 inputs that are tied to basic push buttons in the circuit, and 6 outputs, 2 of which are used for I2C communications, and 4 that are used to power indicator LEDs. 

After initializing all of the needed ports, the MSP430 master device sits in a permanent loop, waiting for button presses. The program, using an [interrupt based architecture](https://ece353.engr.wisc.edu/interrupts/interrupts/#:~:text=the%20desired%20behavior.-,Interrupt%20Service%20Routine,-When%20an%20interrupt), waits in a low power state, and then sends a corresponding signal over I2C to the slave device. 

Upon startup, the MSP430-2310 slave device initializes two pins for I2C communications, 4 outputs for motor pins, and 1 output for the servo. After initializing all of the needed ports, the MSP430 slave device sits in a permanent loop, running [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) signals, and waiting for I2C messages from the master device. 

**I2C/Slave Device**
There are a total of 4 input buttons connected to the master device, 2 controlling the wheels, and 2 controlling the servo. The 2 buttons controlling the motor increment a variable either up or down, in steps of 10, from a range of 0-100. Values of 20-40 correspond to a reverse direction, with 40 being minimal reverse speed, and 20 being a maximum reverse speed. Values of 60-90 correspond to a forward direction, with 60 being a minimal forward speed, and 90 being maximum forward speed. 
In order to send all of the speed and direction information over I2C in one data packet, the tens spot was used to control the speed, as explained above, while the ones spot of the integer packet was used by the slave to control the direction of the servo. A value of 1-4 in the ones spot corresponded to a left turn of the servo, with 1 being a maximum left turn, and 4 being a slight left turn. A value of 6-9 in the ones spot corresponded to a right turn of the servo, with 6 being a minimal right turn, and 9 being a maximum right turn.

Upon receiving an I2C packet, the slave MSP breaks apart the packet into two variables, a speed variable, corresponding to the digit in the tens place of the packet, and a turn variable corresponding to the digit in the ones place of the packet. The slave then continues to loop, while sending appropriate signals to the motor driver and servo. The motor driver signals are bit-banged using delay loops in the code, while the built in timer module is used to control the servo. The timer module is used for the servo needing tighter timing tolerances on its PWM signal. The slave device continues controlling these signals in a loop, using the last sent I2C value if no new one is provided.

Values of 5 for either the speed or steering variable are equal to the center values of the respective hardware. This means a value of 5 in the tens spot corresponds to the wheels being stopped, and the steering being centered.

**State of the Project**

This project is what I would consider complete, as it was done solely to complete my final project requirements for the EELE 465 Microcontrollers class at Montana State University. The software is written in very simple manners, making it pretty robust and error free. Occasionally, the I2C communications will get out of cycle and will error out. This is fixed by power-cycling the circuit.

I consider this project to be a success, as it both completed the hardest class I took in my path to graduation, and was very fun to build. This used the same hardware platform as my other [RC car project, based on Raspberry Pis](https://github.com/benbega/RaspberryPi-RemoteCar), but is significantly lower power, and has much less control.

Although I don't plan on working on this project further, I am glad I got it this far, and am proud of the result.