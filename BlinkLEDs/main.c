#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *) 0x400253FC))
    #define GPIO_PORTF_DIR_R        (*((volatile unsigned long *) 0x40025400))
    #define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *) 0x40025420))
    #define GPIO_PORTF_PUR_R        (*((volatile unsigned long *) 0x40025510))
    #define GPIO_PORTF_DEN_R        (*((volatile unsigned long *) 0x4002551C))
    #define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *) 0x40025520))
    #define GPIO_PORTF_CR_R         (*((volatile unsigned long *) 0x40025524))
    #define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *) 0x40025528))
    #define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *) 0x4002552C))
    #define SYSCTL_RCGC2_R          (*((volatile unsigned long *) 0x400FE108))

    unsigned long SW1,SW2;  // input from PF4,PF0
    unsigned long Out;      // outputs to PF3,PF2,PF1 (multicolor LED)

    //   Function Prototypes
    void PortF_Init(void);
    void Delay(void);

    int main(void){
        PortF_Init();  // make PF1 out (PF1 built-in LED)

        while(1){
            SW1 = GPIO_PORTF_DATA_R&0x10;       // read PF4 into SW1
            SW2 = GPIO_PORTF_DATA_R&0x01;       // read PF0 into SW2

            if (SW1 == 0 && SW2 == 0)           // both pressed
            {
                GPIO_PORTF_DATA_R = 0x04;       // LED is blue
            } else if (SW1 == 0 && SW2 != 0)    // just SW1 pressed
            {
                GPIO_PORTF_DATA_R = 0x02;       // LED is red
            } else if (SW1 != 0 && SW2 == 0)    // just SW2 pressed
            {
                GPIO_PORTF_DATA_R = 0x08;       // LED is green
            }else                               // neither switch
            {
                GPIO_PORTF_DATA_R = 0x00;       // LED is off
            }
        }
    }

    void PortF_Init(void){
        volatile unsigned long delay;
        SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
        delay = SYSCTL_RCGC2_R;           // delay
        GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0
        GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
        GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
        GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL
        GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output
        GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
        GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0
        GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0
    }

    void Delay(void){
        unsigned long volatile time;
        time = 145448;  // 0.1sec
        while(time){
            time--;
        }
    }
