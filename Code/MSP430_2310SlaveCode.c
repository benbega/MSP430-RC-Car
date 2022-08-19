#include <msp430.h>
#include <stdio.h>

// Variables
int inputData = 55;
int inputCopy = 55;

int speedVal;
int steerVal;

int i = 0;

//Timer Variables
int maxTime = 1000;
int timeOn = 500;

int motorPeriod = 100; //100ms period
int motorOn = 0;

// Functions
void DelayLoop(int inputTime);


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    UCB0CTLW0 |= UCSWRST; //RESET

    UCB0CTLW0 |= UCMODE_3; //SELECT I2C
    UCB0CTLW0 &= ~UCMST;   //slave MODE
    UCB0I2COA0 = 0x0012;     //SLAVE ADDRESS
    UCB0I2COA0 |= UCOAEN;   // slave address enable
    UCB0CTLW0 &= ~UCTR;      //RECEIVE MODE
    UCB0CTLW1 &= ~UCSWACK;  // auto acknowledge
    UCB0TBCNT = 0x01;      // Length of Receiving data


    UCB0CTLW1 &= ~UCTXIFG0;
    P1SEL1 &= ~BIT3;    //P1.3 = SCL
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;     //P1.2 = SDA
    P1SEL0 |= BIT2;

    P2DIR   |=  BIT7;
    P2OUT   |=  BIT7;

    P1DIR   |=  BIT6; //servo output
    P1OUT   |=  BIT6; //PWM starts at 1?

    P1DIR   |=  BIT0; //Motor output 1
    P1OUT   &=  ~BIT0; //Motor OFF

    P1DIR   |=  BIT1; //Motor output 2
    P1OUT   &=  ~BIT1; //Motor OFF

    P1DIR   |=  BIT4; //Motor Output 3
    P1OUT   &=  ~BIT4; //MOTOR OFF

    P1DIR   |=  BIT5; //Motor Output 4
    P1OUT   &=  ~BIT5; //MOTOR OFF

    //Setup Timer B0
    TB0CTL  |=  TBCLR;
    TB0CTL  |=  TBSSEL__ACLK;
    TB0CTL  |=  MC__UP;
    TB0CCR0 =   900; // CCR0 = 16,384
    TB0CCR1 =   63;  // CCR1 = 819

    //Timer Compare IRQ for CCR0 and CCR1
    TB0CCTL0    |=  CCIE;
    TB0CCTL0    &=  ~CCIFG;
    TB0CCTL1    |=  CCIE;
    TB0CCTL1    &=  ~CCIFG;


    PM5CTL0 &= ~LOCKLPM5;               // TURN ON DIGITAL I/O

    UCB0CTLW0 &= ~UCSWRST;              // OUT OF RESET

    UCB0IE |= UCRXIE0;                  //ENABLE I2C RX IRQ

    __enable_interrupt();

    while(1){//run an infinite loop for our code

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

        inputCopy = inputData;

        if(inputCopy != 0){

            speedVal = inputCopy/10;
            steerVal = inputCopy-10*speedVal;

            if(steerVal == 1){
                TB0CCR1 = 47; //7.7%
            }
            else if(steerVal == 2){
                TB0CCR1 = 50;
            }
            else if(steerVal == 3){
                TB0CCR1 = 53;
            }
            else if(steerVal == 4){
                TB0CCR1 = 56;
            }
            else if(steerVal == 5){
                TB0CCR1 = 59; //center-ish
            }
            else if(steerVal == 6){
                TB0CCR1 = 62; //7.7%
            }
            else if(steerVal == 7){
                TB0CCR1 = 65;
            }
            else if(steerVal == 8){
                TB0CCR1 = 68;
            }
            else if(steerVal == 9){
                TB0CCR1 = 71;
            }

            if(speedVal == 1 || speedVal == 9){
                motorOn = motorPeriod; //full power
            }
            else if(speedVal == 2 || speedVal == 8){
                motorOn = motorPeriod*0.75; //75% power
            }
            else if(speedVal == 3 || speedVal == 7){
                motorOn = motorPeriod*0.5; //50% power
            }
            else if(speedVal == 4 || speedVal == 6){
                motorOn = motorPeriod*0.25; //25% power
            }
            else if(speedVal == 5){
                motorOn = 0; //no power
            }
        }

        if(speedVal < 5){
            P1OUT   &=  ~BIT0;//backward
            P1OUT   |=  BIT1;//backward
            P1OUT   &=  ~BIT4;//backward
            P1OUT   |=  BIT5;//backwar

            DelayLoop(motorOn);
            P1OUT   &=  ~BIT0;//off
            P1OUT   &=  ~BIT1;//off
            P1OUT   &=  ~BIT4;//off
            P1OUT   &=  ~BIT5;//off
            DelayLoop(motorPeriod-motorOn);
        }
        else if(speedVal > 5){
            P1OUT   |=  BIT0;//forward
            P1OUT   &=  ~BIT1;//forward
            P1OUT   |=  BIT4;//backward
            P1OUT   &=  ~BIT5;//backwar

            DelayLoop(motorOn);
            P1OUT   &=  ~BIT0;//off
            P1OUT   &=  ~BIT1;//off
            P1OUT   &=  ~BIT4;//off
            P1OUT   &=  ~BIT5;//off
            DelayLoop(motorPeriod-motorOn);
        }
        else{
            P1OUT   &=  ~BIT0;
            P1OUT   &=  ~BIT1;

        }
    }

    return 0;
}


void DelayLoop(int inputTime){
    //inputTime in milliseconds
    int i;
    i = inputTime;
    int k;
    while(i > 0){
        i--;
        for(k = 0; k < 100; k++){
            //delay loop ~~~ 1 millisecond??
        }
    }
}


//---------------------------------------------
// Interrupt Service Routines
//---------------------------------------------

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){

    UCB0IE  &=  ~UCRXIE0; // Disable i2c RX0 IRQ - means we have all of our data in input reg
    if(UCB0RXBUF != '\0'){
        inputData   =   UCB0RXBUF; // Gets the data from i2c input buffer
    }

    UCB0IE  |=  UCRXIE0;
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void){
    P1OUT   |=  BIT6; // SERVO ON
    TB0CCTL0    &=  ~CCIFG; // Clear CCR0 Flag
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void){
    P1OUT   &=  ~BIT6; // SERVO OFF
    TB0CCTL1    &=  ~CCIFG; // Clear CCR1 Flag
}
