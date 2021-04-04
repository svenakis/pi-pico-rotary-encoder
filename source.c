#include "source.h"
#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/gpio.h"

/* read a rotary encoder that counts up or down counter 
 * minicom.py /dev/cu.usb... 115200 
 * prints via usb serial a counter up ( CW=clockwise ), down ( CCW=counterclockwise )
 * r = reboots to programming mode
 *
 * later: integrate a 16x2 LCD i2c display
*/
// Rotary Encoder Inputs
#define inputCLK 18
#define inputDT 19

int counter = 0; 
int currentStateCLK;
int previousStateCLK;
int currentStateDT; 
bool block = 0;

// buffer to write CCW or CW 
char encdir_buf[256];

// for scanf to read from input ser or usb ?
char inbuffer[1024]; 

int16_t ch;

// LED Outputs
#define ledCW 14
#define ledCCW 15
#define LED_PIN_25 25

#define HIGH 1
#define LOW 0

bool lock = 0;

// 16x2 LCD i2c display


int initAll() { 
   stdio_init_all();
   //stdout_uart_init();

   // Setup the USB as as a serial port
   stdio_usb_init();
   
   // Set encoder pins as inputs  
   //pinMode (inputCLK,INPUT);
   gpio_init(inputCLK);
   gpio_set_dir(inputCLK, GPIO_IN);

   //pinMode (inputDT,INPUT);
   gpio_init(inputDT);
   gpio_set_dir(inputDT, GPIO_IN);
   
   // Set LED pins as outputs
   //pinMode (ledCW,OUTPUT);
   gpio_init(ledCW);
   gpio_set_dir(ledCW, GPIO_OUT);
   
   //pinMode (ledCCW,OUTPUT);
   gpio_init(ledCCW);
   gpio_set_dir(ledCCW, GPIO_OUT);

   // pinMode (led_buildin, OUTPUT);
   gpio_init(LED_PIN_25);
   gpio_set_dir(LED_PIN_25, GPIO_OUT);
}



void encoder(){
    // Read the current state of inputCLK
    //currentStateCLK = digitalRead(inputCLK);
    currentStateCLK = gpio_get(inputCLK);
    // If the previous and the current state of the inputCLK are different then a pulse has occured
    if ( (currentStateCLK != previousStateCLK) && ( block == 0 ) ) {
        sleep_ms(2);
        currentStateDT  = gpio_get(inputDT);
        currentStateCLK = gpio_get(inputCLK);
        
        if ( (currentStateCLK == HIGH) && ( currentStateDT == HIGH) ) {
            //if ( (gpio_get(inputCLK) == HIGH) && ( gpio_get(inputDT) == HIGH) ) {
            block = 1;  // for the print function in main()
            // If the inputDT state is different than the inputCLK state then 
            // encoder is turning clockwise CCW
            counter --;
            sprintf(encdir_buf, "Direction:  %s", "CCW");
            //digitalWrite(ledCW, LOW);
            gpio_put(ledCCW, LOW);
            //digitalWrite(ledCCW, HIGH);
            gpio_put(ledCW, HIGH);
            //sleep_ms(1);                
        }
        // Encoder is rotating counterclockwise CW
        if( (currentStateCLK == HIGH) && (currentStateDT == LOW) ) {
            block = 1;  // for the print function in main()
            counter ++;
            sprintf(encdir_buf, "Direction %s", "CW");
            //digitalWrite(ledCW, HIGH); // in Arduino code
            
            gpio_put(ledCCW, HIGH);
            //digitalWrite(ledCCW, LOW); // in Arduino code
            gpio_put(ledCW, LOW);
           
            //sleep_ms(1);
        }
// print function is moved to source.c
        previousStateCLK = currentStateCLK; 
    }
    
    // send to core0
}


void main() { 
    initAll();  // activate GPIO in output uart 
    printf("Starting Rotary Encoder");
    
    sprintf(inbuffer, "%s", "ABC");
    
    // give USB time to be setup
    sleep_ms(1000);

    while(true){
        // Read the initial state of inputCLK
        // Assign to previousStateCLK variable
        // previousStateCLK = digitalRead(inputCLK);
        previousStateCLK = gpio_get(inputCLK);
        encoder();
       
        // read serial input to reboot
	if(lock=0) { 
		lock=1;
		puts("USB with debug");
	}
	//sleep_ms(1000);
	ch = getchar_timeout_us(1);
        while (ch != PICO_ERROR_TIMEOUT) {
		printf("For reboot press r\n");
		printf("c: %c = ", ch);
		printf("i: %i\n", ch);
		if( ch == 114  ) { 
			printf("Reboot\n");
			reset_usb_boot(0,0);
		}
		
		ch = getchar_timeout_us(1);
	}
	/*
	 scanf("%1024s", inbuffer);
	    if (strcmp(inbuffer, "on") == 0)
	    {  
            printf("got on");
		    gpio_put(LED_PIN_25, 1);
	    } 
	    else if (strcmp(inbuffer, "off") == 0)
	    
	    { 
            printf("goot off");
		    gpio_put(LED_PIN_25, 0);
    	}
	    else if ( strcmp(inbuffer, "re") == 0)
	    {
            printf("reboot");
		    reset_usb_boot(0,0);
    	}
        */
        if(block == 1) {
            printf("Direction: ");
            printf("%3s", encdir_buf);
            //puts(encdir_buf);
            printf(" -- Value: ");
            printf("%i", counter);
            printf("\n");
            //puts(counter);
            // Update previousStateCLK with the current state  
            block = 0;
        } 
    }
}
