#include <16F690.h>
#device ADC=10
#use delay(internal=8000000)

#FUSES MCLR                    //No Power Up Timer 


#byte CCP1CON = 0x017
#byte CCPR1H = 0x016
#byte CCPR1L = 0x015
#byte T1CON = 0x010
#byte TMR1H = 0x00F
#byte TMR1L = 0x00E

#bit Mains_Detect = 0x005.0
#bit Inverter_Working = 0x005.1
#bit Low_Battery  = 0x007.3

#bit Buzzer = 0x007.6
#bit Charging_Relay = 0x006.7



