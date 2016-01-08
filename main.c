#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "Board_Buttons.h"
#include "Board_LED.h"
#include "stm32f4xx.h"
#include "led_timer.h"
#include "led_pwm.h"
#include "speaker.h"
#include "serial_port_usb/serial_port_usb.h"
#define ARM_MATH_CM4
#include "arm_math.h"


volatile uint32_t mmsTicks = 0;                      /* counts 0.1ms timeTicks       */

// SysTick Handler (Interrupt Service Routine for the System Tick interrupt)
void SysTick_Handler(void){
  mmsTicks++;
}

// initialize the system tick 
void init_systick(void){
	SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 10000)) { /* SysTick 0.1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }
}


int32_t main(){
	SystemInit();
	init_systick();
	init_serial_port_usb();  //initializes the usb serial port
	init_speaker();          //initializes the speaker
	
	int16_t audio_sample;    //one audio sample is 16 bits which is what the send_to_speaker() function uses as an argument
	float audio_freq = 0;    //the frequency of the audio sample
	int volume = 700;        //volume is how loud the output volume will be, it is the amplitude of the sin wave 
	uint8_t k;	             //ascii key character value, used to hold the current key press
	float hold_freq_time = 0;   //amount of time to output a frequency for a single key, k
	float time_last_freq_change = 0;   //holds the most recent time when the key, k was changed
	
	while (1) {
		float ts = mmsTicks / 10000.0;   //time in seconds, mmsTicks increments every 0.1ms so dividing by 10,000 will give you time in seconds
		read_serial_usb_byte(&k); // this function reads a key from the serial port and will return the value of 
															//the key by writing it in the memory pointed to by &k
						
						//depending on which ascii key, k, was pressed, it writes the k value back to the serial usb port, sets the corresponding 
						//frequency of that key, sets how long the frequency lasts, and sets the current time at which the key was pressed
				switch (k) {
					
						//5th harmonic first regular row on a standard US keyboard 
						case '1': write_serial_usb_byte(k); audio_freq = 440; hold_freq_time = 0.360 ;time_last_freq_change = ts; break; //A4
						case '2': write_serial_usb_byte(k); audio_freq = 466; hold_freq_time = 0.357 ;time_last_freq_change = ts; break; //A4# 
						case '3': write_serial_usb_byte(k); audio_freq = 494; hold_freq_time = 0.353 ;time_last_freq_change = ts; break; //B4
						case '4': write_serial_usb_byte(k); audio_freq = 523; hold_freq_time = 0.350 ;time_last_freq_change = ts; break; //C4
						case '5': write_serial_usb_byte(k); audio_freq = 554; hold_freq_time = 0.347 ;time_last_freq_change = ts; break; //C5#
						case '6': write_serial_usb_byte(k); audio_freq = 587; hold_freq_time = 0.344 ;time_last_freq_change = ts; break; //D5
						case '7': write_serial_usb_byte(k); audio_freq = 622; hold_freq_time = 0.341 ;time_last_freq_change = ts; break; //D5#
						case '8': write_serial_usb_byte(k); audio_freq = 659; hold_freq_time = 0.338 ;time_last_freq_change = ts; break; //E5
						case '9': write_serial_usb_byte(k); audio_freq = 698; hold_freq_time = 0.334 ;time_last_freq_change = ts; break; //F5
						case '0': write_serial_usb_byte(k); audio_freq = 740; hold_freq_time = 0.330 ;time_last_freq_change = ts; break; //F5#
						case '-': write_serial_usb_byte(k); audio_freq = 784; hold_freq_time = 0.327 ;time_last_freq_change = ts; break; //G5
						case '=': write_serial_usb_byte(k); audio_freq = 831; hold_freq_time = 0.323 ;time_last_freq_change = ts; break; //G5#
						
						//6th harmonic second regular row on a standard US keyboard 					
						case 'q': write_serial_usb_byte(k); audio_freq = 880; hold_freq_time = 0.320 ;time_last_freq_change = ts; break; //A5
						case 'w': write_serial_usb_byte(k); audio_freq = 932; hold_freq_time = 0.318 ;time_last_freq_change = ts; break; //A5#
						case 'e': write_serial_usb_byte(k); audio_freq = 988; hold_freq_time = 0.316 ;time_last_freq_change = ts; break; //B5
						case 'r': write_serial_usb_byte(k); audio_freq = 1047; hold_freq_time = 0.314 ;time_last_freq_change = ts; break; //C5							case 't': write_serial_usb_byte(k); audio_freq = 1109; hold_freq_time = 0.312 ;time_last_freq_change = ts; break; //C6#
						case 'y': write_serial_usb_byte(k); audio_freq = 1175; hold_freq_time = 0.310 ;time_last_freq_change = ts; break; //D6
						case 'u': write_serial_usb_byte(k); audio_freq = 1245; hold_freq_time = 0.308 ;time_last_freq_change = ts; break; //D6#
   					case 'i': write_serial_usb_byte(k); audio_freq = 1319; hold_freq_time = 0.306 ;time_last_freq_change = ts; break; //E6
						case 'o': write_serial_usb_byte(k); audio_freq = 1397; hold_freq_time = 0.304 ;time_last_freq_change = ts; break; //F6
						case 'p': write_serial_usb_byte(k); audio_freq = 1480; hold_freq_time = 0.302 ;time_last_freq_change = ts; break; //F6#
						case '[': write_serial_usb_byte(k); audio_freq = 1568; hold_freq_time = 0.300 ;time_last_freq_change = ts; break; //G6
						case ']': write_serial_usb_byte(k); audio_freq = 1661; hold_freq_time = 0.298 ;time_last_freq_change = ts; break; //G6#
		
						//7th harmonic shift + first regular row on a standard US keyboard 			
						case '!': write_serial_usb_byte(k); audio_freq = 1760; hold_freq_time = 0.296 ;time_last_freq_change = ts; break; //A6
						case '@': write_serial_usb_byte(k); audio_freq = 1865; hold_freq_time = 0.294 ;time_last_freq_change = ts; break; //A6#
						case '#': write_serial_usb_byte(k); audio_freq = 1980; hold_freq_time = 0.292 ;time_last_freq_change = ts; break; //B6
	 					case '$': write_serial_usb_byte(k); audio_freq = 2093; hold_freq_time = 0.290 ;time_last_freq_change = ts; break; //C7
		   			case '%': write_serial_usb_byte(k); audio_freq = 2218; hold_freq_time = 0.288 ;time_last_freq_change = ts; break; //C7#
					  case '^': write_serial_usb_byte(k); audio_freq = 2349; hold_freq_time = 0.286 ;time_last_freq_change = ts; break; //D7
						case '&': write_serial_usb_byte(k); audio_freq = 2489; hold_freq_time = 0.284 ;time_last_freq_change = ts; break; //D7#
						case '*': write_serial_usb_byte(k); audio_freq = 2637; hold_freq_time = 0.282 ;time_last_freq_change = ts; break; //E7
						case '(': write_serial_usb_byte(k); audio_freq = 2794; hold_freq_time = 0.280 ;time_last_freq_change = ts; break; //F7
   					case ')': write_serial_usb_byte(k); audio_freq = 2960; hold_freq_time = 0.278 ;time_last_freq_change = ts; break; //F7#
						case '_': write_serial_usb_byte(k); audio_freq = 3136; hold_freq_time = 0.276 ;time_last_freq_change = ts; break; //G7
	   				case '+': write_serial_usb_byte(k); audio_freq = 3322; hold_freq_time = 0.274 ;time_last_freq_change = ts; break; //G7#
	
						//8th harmonic shift + second regular row on a standard US keyboard 				
						case 'Q': write_serial_usb_byte(k); audio_freq = 3520; hold_freq_time = 0.272 ;time_last_freq_change = ts; break; //A7
						case 'W': write_serial_usb_byte(k); audio_freq = 3729; hold_freq_time = 0.270 ;time_last_freq_change = ts; break; //A7#
						case 'E': write_serial_usb_byte(k); audio_freq = 3951; hold_freq_time = 0.268 ;time_last_freq_change = ts; break; //B7
						case 'R': write_serial_usb_byte(k); audio_freq = 4186; hold_freq_time = 0.266 ;time_last_freq_change = ts; break; //C8
				} 
			
					//this checks to see if the amount of time each key should output its frequency has elapsed 
					//it compares the current time, ts, with how long each key should output its frequecy + when the key was pressed
					//it 'turns off' the speaker by setting its frequency to 0
					if (ts > ( hold_freq_time + time_last_freq_change)) {  
						audio_freq =0;	
						time_last_freq_change = ts;		
					}
					
		//a sin function is used so that it outputs a signal in uniform intervals
		audio_sample = (int16_t) (volume * arm_sin_f32(audio_freq*ts)); // calculate one sample for the speaker
		//the CMSIS arm_sin_f32 function from arm_math.h is typically faster than sin() from math.h
		send_to_speaker(audio_sample);  //send one audio sample to the audio output
	
	}
}



