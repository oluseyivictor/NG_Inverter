#include <main232.h>
#define BATTERY_LOW  11
#define BATTERY_FULL 13.8

unsigned int16 value[3];
volatile unsigned int16 TBL_POINTER_NEW, TBL_POINTER_OLD;
int1 Direction,Overload_Condition;
unsigned int8 step = 0;
volatile int1 Turn_On;
unsigned int8 Grid_Available = 0;
float Battery_Voltage;
float Temperature;

CONST unsigned char DUTY_CYCLE1[25]={0x13,0x15,0x17,0x19,0x1B,0x1D,0x1F,0x21,0x23,0x25,0x27,0x29,0x2A,0x2C,0x2E,0x30,0x32,0x34,0x36,0x38,0x3A,0x3C,0x3E,0x40,0x42};
CONST unsigned char DUTY_CYCLE2[25]={0x88,0x7C,0x70,0x58,0x40,0x4C,0x10,0x34,0x28,0x1C,0x10,0x04,0xF8,0xEC,0xE0,0xD4,0xC8,0xBC,0xB0,0xA0,0x98,0x8C,0x80,0x74,0x64};

#INT_TIMER2
void  TIMER2_isr(void) 
{

if (Turn_On)
{
 TBL_POINTER_NEW=TBL_POINTER_OLD + 2045;

 if (TBL_POINTER_NEW < TBL_POINTER_OLD){

              TMR1H = 0;
              TMR1L = 0; 
           if (Direction == 0){
      
               output_low(PIN_B5);
                output_high(PIN_B6);
                CCP1CON=10;
               CCPR1H=DUTY_CYCLE1[step];
               CCPR1L=DUTY_CYCLE2[step];

              Direction = 1;
            

           }

           else{
            
           output_low(PIN_B6);
            output_high(PIN_B5); 
               CCP1CON=10;
               CCPR1H=DUTY_CYCLE1[step];
               CCPR1L=DUTY_CYCLE2[step];
  
                Direction = 0;

           }
 }
   
     TBL_POINTER_OLD=TBL_POINTER_NEW;
     clear_interrupt(INT_TIMER2);

}
else
{
 output_low(PIN_B6);
 output_low(PIN_B5); 
CCP1CON=0;
}

}

#INT_CCP1
void  CCP1_isr(void) 
{
 output_low(PIN_B6);
 output_low(PIN_B5); 
CCP1CON=0;

}

#INT_TIMER1
void  TIMER1_isr(void) 
{

}


void GPIO_Config()
{


 output_low(PIN_B6);
 output_low(PIN_B5); 
 
  output_low(PIN_A0);
 output_low(PIN_A1);
 
   output_low(PIN_C3);
 output_low(PIN_C6);
 
  output_low(PIN_B7);
 
}

void Read_Measure()
{
set_adc_channel(2); 
delay_ms(100);
value[0] = Read_adc(); 
Battery_Voltage = ((value[0] * 5)/1024 );   // Battery volt
Battery_Voltage = Battery_Voltage * 3.7;
delay_ms(100);

 set_adc_channel(3);
 delay_ms(100); 
 value[1] = Read_adc(); 
 delay_ms(100);
 
 set_adc_channel(4);
 delay_ms(100); 
 value[2] = Read_adc(); 
 Temperature = ((value[2] * 5)/1024 );   // Temperature
 delay_ms(100);
 
}


void LED_Sequence()
{
   
   if (Turn_On == 1)
   {
   Inverter_Working = 1;   //INVERTER_LED ON
   }
   else
   {
   Inverter_Working = 0;
   }
   
   
   
   if (Grid_Available == 1)
   {
   Mains_Detect = 1;
   }
   else
   {
   Mains_Detect = 0;
   }
   
   
   
   if (Battery_Voltage < BATTERY_LOW )
   {
   Low_Battery = 1;    //Light up low battery LED
   Turn_On = 0;
   Buzzer = 1;
   delay_ms(2000);
   Buzzer = 0;
   }
   else 
   {
   Low_Battery = 0;
   Buzzer = 0;
   }
   
}




void Check_Button()
{
if (!input(PIN_B4) && (!Overload_Condition) && (!Grid_Available) ) // when switch is pressed and there is no overload condition
{
delay_ms(100);
   if (!input(PIN_B4))
   {
   Turn_On = 1;      //iNVERTER IS ON
   }
}
else
{
delay_ms(100);
 if (input(PIN_B4)) 
 {
 Turn_On = 0;
 Overload_Condition = 0;
 }
}
}

void Mains_Detect()
{
if (input(PIN_C4))
{
delay_ms(100);
   if (input(PIN_C4))
   {
   Grid_Available = 1;
   Turn_On = 0;
   }
}
else
{
delay_ms(100);
 if (!input(PIN_C4)) Grid_Available = 0;
}
}


void Charge_Batt()
{
if (Grid_Available == 1)
{
Turn_On = 0;   //SWITCH OFF INVERTER
delay_ms(1000);
Charging_Relay = 1;
}
else {
Charging_Relay = 0;
}
}

void Overload_Detect()
{
if (input(PIN_C5))
{
Turn_On = 0;     //shutDown Inverter 
Buzzer = 1;
delay_ms(3000);
Buzzer = 0;
Overload_Condition = 1;
}
}

void main()
{

GPIO_Config();
Turn_On = 0;
step = 10;
Overload_Condition = 0;
 
  setup_adc_ports(sAN2|sAN3|sAN4);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   
   
   setup_timer_2(T2_DIV_BY_4,159,1);      //312 us overflow, 312 us interrupt
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);      //32.7 ms overflow

    enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_TIMER2);
   enable_interrupts(INT_CCP1);
   enable_interrupts(GLOBAL);

   while(1)
   {
 
   Read_Measure();
   LED_Sequence();
   Check_Button();     //On/OFF
   Mains_Detect();
   Overload_Detect();
   Charge_Batt();
   
   //Buzzer = 1;

      //TODO: User Code
   }

}
