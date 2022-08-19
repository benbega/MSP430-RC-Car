#include <msp430.h>
#include <stdio.h>

//Variables
int dataToSend = 55;
int dataTempDigitOne = 0;
int dataTempDigitTwo = 0;


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //I2C Setup
    UCB0CTLW0   |=  UCSWRST; //RESET

    UCB0CTLW0   |= UCSSEL_3; //smclk
    UCB0BRW = 10; // Divide by 10. SCL = 100kHz

    UCB0CTLW0   |=  UCMODE_3; // i2c mode
    UCB0CTLW0   |=  UCMST; // i2c master device
    UCB0CTLW0   |=  UCTR; // put into TX mode
    UCB0I2CSA   =   0x0012; // slave address for MSP2310 slave

    UCB0CTLW1   |=  UCASTP_2; // stop when UCB1CNT is reached
    UCB0TBCNT   =   0x01; //send 1 byte of data

    P1SEL1  &=  ~BIT3; // Set P1.3 as SCL
    P1SEL0  |=  BIT3; //

    P1SEL1  &=  ~BIT2; // Set P1.2 as SDA
    P1SEL0  |=  BIT2;

    UCB0CTLW0   &=  ~UCSWRST;
    PM5CTL0 &=  ~LOCKLPM5;

    UCB0IE  |=  UCTXIE0; // Enable i2x TX IRQ

    // Inputs
    P2DIR   &=  ~BIT0; //P2.0 as input - Button 1
    //P2REN   |=  BIT0;
    P2OUT   &=  ~BIT0;

    P2DIR   &=  ~BIT2; //P2.2 as input - Button 2
    //P2REN   |=  BIT2;
    P2OUT   &=  ~BIT2;

    P2DIR   &=  ~BIT4; //P2.4 as input - Button 3
    //P2REN   |=  BIT2;
    P2OUT   &=  ~BIT4;

    P2DIR   &=  ~BIT6; //P2.6 as input - Button 4
    //P2REN   |=  BIT2;
    P2OUT   &=  ~BIT6;



    // Outputs
    P1DIR   |=  BIT6; //P1.6 as output - Motor Driver INPUT 1
    P1DIR   |=  BIT7; //P1.7 as output - Motor Driver INPUT 2
    P2DIR   |=  BIT1; //P2.1 as output - LED 1
    P2DIR   |=  BIT3; //P2.3 as output - LED 2
    P2DIR   |=  BIT5; //P2.5 as output - LED 3
    P2DIR   |=  BIT7; //P2.7 as output - LED 4
    P4DIR   |=  BIT1; //P4.1 as test output


    P1OUT   &=  ~BIT6; //turn off P1.6
    P1OUT   &=  ~BIT7; //turn off P1.7
    P2OUT   &=  ~BIT1; //turn off P2.1
    P2OUT   &=  ~BIT3; //turn off P2.3
    P2OUT   &=  ~BIT5; //turn off P2.5
    P2OUT   &=  ~BIT7; //turn off P2.7
    P4OUT   &=  ~BIT1; //P4.1 as test output

    PM5CTL0 &=  ~LOCKLPM5;

    //IRQ Setup
    P2IE    |=  BIT0; //Enable P2.0 IRQ
    P2IES   |=  BIT0; //enable High to Low sensitivity
    P2IFG   &=  ~BIT0; //Clear P2.0 IRQ Flag

    P2IE    |=  BIT2; //Enable P2.2 IRQ
    P2IES   |=  BIT2; //enable High to Low sensitivity
    P2IFG   &=  ~BIT2; //Clear P2.2 IRQ Flag

    P2IE    |=  BIT4; //Enable P2.4 IRQ
    P2IES   |=  BIT4; //enable High to Low sensitivity
    P2IFG   &=  ~BIT4; //Clear P2.4 IRQ Flag

    P2IE    |=  BIT6; //Enable P2.6 IRQ
    P2IES   |=  BIT6; //enable High to Low sensitivity
    P2IFG   &=  ~BIT6; //Clear P2.6 IRQ Flag

    __enable_interrupt(); //Enable maskable IRQs

    while(1){
//      P1OUT   &=  ~BIT6; //OFF
//      P1OUT   &=  ~BIT7;
//      Delay(1000);
//
//      P1OUT   |=  BIT6; //Backward
//      P1OUT   &=  ~BIT7;
//      Delay(1000);
//
//      P1OUT   &=  ~BIT6; //OFF
//      P1OUT   &=  ~BIT7;
//      Delay(1000);
//
//      P1OUT   &=  ~BIT6;//Forward
//      P1OUT   |=  BIT7;
//      Delay(1000);

        //UCB0CTLW0   |=  UCTXSTT; // Generate start condition

    }

    return 0;
}

void Delay(int inputTime){
    //inputTime in milliseconds
    int i;
    i = inputTime;
    int k;
    while(i > 0){
        i--;
        for(k = 0; k < 103; k++){
            //delay loop ~~~ 1 millisecond??
        }
    }
}

void DelayHalf(int inputTime){
    //inputTime in milliseconds
    int i;
    i = inputTime;
    int k;
    while(i > 0){
        i--;
        for(k = 0; k < 52; k++){
            //delay loop ~~~ 0.5 millisecond??
        }
    }
}

// Interrupt Service Routines

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    UCB0TXBUF = dataToSend;
}

#pragma vector = PORT2_VECTOR
__interrupt void ISR_Port2_B0(void){

    if(P2IFG&BIT0){//Button ONE pressed -- INCREASE SPEED
        P2OUT   ^=  BIT1; //toggle P2.1

        //math for selecting data value
        //increasing speed, need to increment the hundreds spot by 1
        if(dataToSend < 90){
            dataToSend += 10;
        }

        UCB0CTLW0   |=  UCTXSTT; // Generate start condition
    }

    else if(P2IFG&BIT2){//Button TWO pressed -- DECREASE SPEED
        P2OUT   ^=  BIT3; //toggle P2.3

        //math for selecting data value
        //increasing speed, need to decrement the hundreds spot by 1
        if(dataToSend > 20){
            dataToSend -= 10;
        }

        UCB0CTLW0   |=  UCTXSTT; // Generate start condition
    }

    else if(P2IFG&BIT4){//Button THREE pressed -- TURN LEFT
        P2OUT   ^=  BIT5; //toggle P2.5
        //math for selecting steer value
        //

        dataTempDigitOne = dataToSend/10;
        dataTempDigitTwo = dataToSend-10*dataTempDigitOne;

        if(dataTempDigitTwo > 1){
            dataToSend -= 1;
        }


        UCB0CTLW0   |=  UCTXSTT; // Generate start condition
    }

    else if(P2IFG&BIT6){//Button FOUR pressed -- TURN RIGHT
        P2OUT   ^=  BIT7; //toggle P2.7
        //math for selecting steer value
        //

        dataTempDigitOne = dataToSend/10;
        dataTempDigitTwo = dataToSend-10*dataTempDigitOne;

        if(dataTempDigitTwo < 9){
            dataToSend += 1;
        }

        UCB0CTLW0   |=  UCTXSTT; // Generate start condition
        }


    int i = 10;
    int k;
    while(i > 0){
        i--;
        for(k = 0; k < 103; k++){
        //delay loop ~~~ 1 millisecond??
        }
    }

    P2IFG   &=  ~BIT0;
    P2IFG   &=  ~BIT2;
    P2IFG   &=  ~BIT4;
    P2IFG   &=  ~BIT6;
}

