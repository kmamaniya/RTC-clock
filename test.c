/*

* Project Name: 	Table Clock
* Author List: 		Karan Mamaniya
* Filename: 		test.c
* Functions: 		
* Global Variables:	currentTime, AlarmTime, dispArr[], dispArr1[]
*
*/

#include "Includes.h"
#include "DHT11.h"
#include <reg51.h>
#define debounceTime 5000

unsigned short currentTime;
unsigned char dispArr1[7];
unsigned char dispArr[10];
sbit modepin = P0^0;
sbit changetens = P0^1;
sbit changeunits= P0^2;
sbit setpin = P0^3;
sbit AMLed = P3^2;
sbit PMLed = P3^3;
sbit buzzer = P3^4;
sbit latch = P3^0;
int flag=0;
unsigned char AlarmHH[10]={1,1,1,1,1,1,1,1,1,1};
unsigned char AlarmMM[10]={0,0,0,0,0,0,0,0,0,0};
unsigned char AlarmAP[10]={0,0,0,0,0,0,0,0,0,0};
unsigned char YY1=20,YY2,MM,DD,HH,MIN,SS,AP,temperature;
unsigned short AlarmTime[10];
int bitflag=0;
int timeset=0;
int tempflag=0;

/*
* Function Name:	CalculateTempTo7seg
* Input:			unsigned char
* Output:			void, values updates in the display array
* Logic:			decode BCD from decimal data
*/
void CalculateTempTo7seg(unsigned char temp){
	//Temperature
	temperature=temp;
	dispArr[8]=temperature/10;
	dispArr[9]=temperature%10;
	delay(100);
}

/*
* Function Name:	CalculateDateTo7seg
* Input:			char *
* Output:			void, values updates in the display array
* Logic:			decode BCD from decimal data
*/
void CalculateDateTo7seg(char * pDateArray){   // Displays date in DD:MM:YYYY format
	
	//Date
	DD=pDateArray[1];
	dispArr[0]=(char)(DD/10);
	dispArr[1]=(char)(DD%10);
	delay(100);
	
	//Months
	MM=pDateArray[2];
	dispArr[2]=(char)(MM/10);
	dispArr[3]=(char)(MM%10);
	delay(100);
	
	//Year
	dispArr[4]=(char)YY1/10;
	dispArr[5]=(char)YY1%10;
	YY2=pDateArray[3];
	dispArr[6]=(char)(YY2/10);
	dispArr[7]=(char)(YY2%10);
	YY2=pDateArray[3];
	delay(100);
}

/*
* Function Name:	CalculateTimeTo7seg
* Input:			char *
* Output:			void, values updates in the display array
* Logic:			decode BCD from decimal data, store the current time in a variable
*/
void CalculateTimeTo7seg(char *pTimeArray) {  // Displays date in DD:MM:YYYY format
	
	//Hours
	HH=pTimeArray[2];
	dispArr1[0]=(HH/10);
	dispArr1[1]=(HH%10);
	delay(100);
	
	//Minutes
	MIN=pTimeArray[1];
	dispArr1[2]=(MIN/10);
	dispArr1[3]=(MIN%10);
	delay(100);
	
	//Seconds
	SS=pTimeArray[0];
	dispArr1[4]=(SS/10);
	dispArr1[5]=(SS%10);
	delay(100);
	
	//AM PM
	AP=(unsigned char)pTimeArray[3];
	dispArr1[6]=AP;
	currentTime = 60*pTimeArray[2]+pTimeArray[1] + AP*720;    // 60 MINS * NO. OF HOURS + MINUTES  + 12 HOURS for AM/PM
	delay(100);
}

/*
* Function Name:	display
* Input:			void
* Output:			void, Displays time, date, temperature on 7 Segment LED
* Logic:			chose the led to be displayed by address on port 1 and latch it with data by port 2
*/
void display(){
	char i;
	
	/// AM PM LED DISPLAY
	if(dispArr1[6]==1){
		PMLed=1;delay(100);
		AMLed=0;
	}
	else if(dispArr1[6]==0){
		PMLed=0;delay(100);
		AMLed=1;
	}
	else{
		PMLed=0;delay(100);
		AMLed=0;
	}
	delay(100);
	
	
	//DATE DISPLAY
	for(i=0;i<10;i++){
		P2 = dispArr[i];
		delay(10);
		P1 = (unsigned char)i;
		delay(10);
		latch=0;
		delay(10);
		latch=1;
		delay(10);
	}
	delay(100);
	
	
	//TIME DISPLAY
	for(i=0;i<6;i++){
		P2 = dispArr1[i];
		delay(10);
		P1 = (unsigned char)(i+10);
		delay(10);
		latch=0;
		delay(10);
		latch=1;
		delay(10);
	}
	delay(100);
}


/*x
* Function Name:	setTime
* Input:			void
* Output:			void, sets user inputed time in the RTC
* Logic:			dont update if no changed made, Store the variables temporarily in HH,MM,SS, AP display 0 for rest of the values 
*/
void setTime(){
	int i;
	timeset=0;
	bitflag=0;
		while(1){
				if(modepin==0){
					delay(debounceTime);
					if(modepin==0){
						while(modepin!=1);
						flag = 2;
						if(timeset==1)			// SET TIME ONLY IF CHANGED
							Set_DS1307_RTC_Time(AP,HH,MIN,SS);
						Set_DS1307_RTC_Date(DD,MM,YY2,4);
						break;
					}
				}
				for(i=0;i<10;i++)			//Make the date segment 00 00 0000
					dispArr[i]=0;
				
				dispArr1[0]=HH/10;			//Display the temporary variables
				dispArr1[1]=HH%10;
				dispArr1[2]=MIN/10;
				dispArr1[3]=MIN%10;
				dispArr1[4]=SS/10;
				dispArr1[5]=SS%10;
				dispArr1[6]=AP;
				
				display();
				
				
				// IF DECREMENT IS PRESSED
				if(changetens==0){
					delay(debounceTime);
					if(changetens==0){
						timeset=1;				//marks change in value, hence update time
						while(changetens!=1);
						switch(bitflag){
							case 0: HH--;
											if(HH==0)
												HH=12;
											break;
							case 1: MIN--;
											if(MIN>=60)
												MIN=59;
											break;
							case 2: SS--;
											if(SS>=60)
												SS=59;
											break;
							case 3: AP++;
											AP%=2;
							default: break;
						}
					}
				}
				
				
				// IF INCREMENT IS PRESSED
				if(changeunits==0){
					delay(debounceTime);
					if(changeunits==0){
						timeset=1;			//marks change in value, hence update time
						while(changeunits!=1);
						switch(bitflag){
							case 0: HH++;
											if(HH>12)
												HH=1;
											break;
							case 1: MIN++;
											MIN=MIN%60;
											break;
							case 2: SS++;
											SS=SS%60;
											break;
							case 3: AP++;
											AP%=2;
							default: break;
						}
					}
				}
				
				
			//IF CHANGE BLOCK IS PRESSED
			if(setpin==0){
				delay(debounceTime);
				if(setpin==0){
					while(setpin!=1);
					bitflag++;
					bitflag%=4;
				}
			}
		}
	delay(100);
}


/*
* Function Name:	setDate
* Input:			void
* Output:			void, sets user inputed date in the RTC
* Output:			void, sets user inputed date in the RTC
* Logic:			update even if no changed made, Store the variables temporarily in DD,MM,YYYY display 0 for rest of the values 
*/
void setDate(){
		int i;
		bitflag=0;
		while(flag<2){
		for(i=0;i<6;i++)			//display 00:00:00 for time
			dispArr1[i]=0;
		
		
		dispArr1[6]=2;				//Display temporary variables
		dispArr[8]=0;
		dispArr[9]=0;
		dispArr[0]=DD/10;
		dispArr[1]=DD%10;
		dispArr[2]=MM/10;
		dispArr[3]=MM%10;
		dispArr[4]=YY1/10;
		dispArr[5]=YY1%10;
		dispArr[6]=YY2/10;
		dispArr[7]=YY2%10;
		display();
			
		//IF INCREMENT IS PRESSED
		if(changetens==0){
			delay(debounceTime);
			if(changetens==0){
				while(changetens!=1);
				switch(bitflag){
					case 0: DD--;
									if(DD==0)
										DD=31;
									break;
					case 1: MM--;
									if(MM==0)
										MM=12;
									break;
					case 2:	YY1--;
									if(YY1>99)
										YY1=99;
									break;
					case 3:	YY2--;
									if(YY2>99)
										YY2=99;
					default: break;
				}
			}
		}
		
		//IF DECREMENT IS PRESSED
		if(changeunits==0){
			delay(debounceTime);
			if(changeunits==0){
				while(changeunits!=1);
				switch(bitflag){
					case 0: DD++;
									if(DD>31)
										DD=1;
									break;
					case 1: MM++;
									if(MM>12)
										MM=1;
									break;
					case 2:	YY1++;
									YY1%=100;
									break;
					case 3:	YY2++;
									YY2%=100;
					default: break;
				}
			}
		}
		
		//IF CHANGE BLOCK IS PRESSED
		if(setpin==0){
			delay(debounceTime);
			if(setpin==0){
				while(setpin!=1);
				bitflag++;
				bitflag%=4;
			}
		}
		
		//IF CHANGE MODE IS PRESSED
		if(modepin==0){
			delay(debounceTime);
			if(modepin==0){
				while(modepin!=1);
				setTime();
			}
		}
	}	
	delay(100);
}

/*
* Function Name:	setAlarm
* Input:			void
* Output:			void, stores user inputed alarm in the memore
* Logic:			update even if no changed made, Store the variables temporarily in AlarmHH,AlarmMM,AlarmSS,AlarmAP, display 0 for rest pf the values 
*/
void setAlarm(){
			int j=0;		
			int i=0;
			bitflag=0;
			while(flag<2){
				
				for(i=0;i<8;i++)			// DISPLAY 00 00 0000 for date
					dispArr[i]=0;
					
				dispArr1[0]=AlarmHH[j]/10;		//Display stored alarm or default alarm on 7 seg display
				dispArr1[1]=AlarmHH[j]%10;
				dispArr1[2]=AlarmMM[j]/10;
				dispArr1[3]=AlarmMM[j]%10;
				dispArr1[4]=0;
				dispArr1[5]=0;
				dispArr[8]=(j+1)/10;
				dispArr[9]=(j+1)%10;
				dispArr1[6]=AlarmAP[j];
				display();
				
				//IF DECREMENT IS PRESSED
				if(changetens==0){
					delay(debounceTime);
					if(changetens==0){
						while(changetens!=1 );
						if(bitflag==0){
							AlarmHH[j]--;
							if(AlarmHH[j]==0)
								AlarmHH[j]=12;
						}
						else if(bitflag==1){
							AlarmMM[j]--;
							if(AlarmMM[j]>=60)
								AlarmMM[j]=59;
						}
						else {
								AlarmAP[j]++;
								AlarmAP[j]%=2;
						}
					}
				}
				
				//IF INCREMENT IS PRESSED
				if(changeunits==0){
					delay(debounceTime);
					if(changeunits==0){
						while(changeunits!=1);
						if(bitflag==0){
							AlarmHH[j]++;
							if(AlarmHH[j]>12)
								AlarmHH[j]=1;
						}
						else if(bitflag==1){
							AlarmMM[j]++;
							AlarmMM[j]%=60;
						}
						else {
								AlarmAP[j]++;
								AlarmAP[j]%=2;
						}
					}
				}
				
				//IF CHANGE BLOCK IS PRESSED
				if(setpin==0){
					delay(debounceTime);
					if(setpin==0){
						while(setpin!=1);
						bitflag++;
						bitflag%=3;
					}
				}
				
				//IF CHANGE MODE IS PRESSED
				if(modepin==0){
				delay(debounceTime);
					if(modepin==0){
						while(modepin!=1);
						j++;
						if(j==10)
							setDate();
					}
				}
			}
			for(j=0;j<10;j++){
				AlarmTime[j]=60*AlarmHH[j]+AlarmMM[j] + AlarmAP[j]*720;		// update stored alarm time for comparison
				flag=0;
				delay(100);
			}
}

/*
* Function Name:	main
* Input:			void
* Output:			synchronization of the whole system
* Logic:			initialization, default set time or date (if any), read from rtc, temperature sensor and display, check alarm, check id mode switch is pressed
*/
void main(){	
	modepin=1;
	changeunits=1;
	changetens=1;
	setpin=1;
	InitI2C();	// Initialize i2c pins	
	flag=0;
	//set initial time
	//Set_DS1307_RTC_Time(PM_Time, 12, 59, 58);	// Set time 12:59:58 PM
	//Set_DS1307_RTC_Date(24, 8, 18, 0);	// Set Set 24-08-2017
	while(1){
		flag=0;
		CalculateDateTo7seg(Get_DS1307_RTC_Date());
		CalculateTimeTo7seg(Get_DS1307_RTC_Time());
		if(SS%5==4)
			tempflag=1;
		if(SS%5==0 && tempflag==1){
			delay(10000);
			CalculateTempTo7seg(getTemp());
			tempflag=0;
			delay(10000);
		}
		display();
		if(AlarmTime==currentTime)
			buzzer=1;
		else buzzer=0;
		if(modepin==0){
			delay(debounceTime);
			if(modepin==0){
				while(modepin!=1);
				setAlarm();
			}
		}
		delay(65000);
	}
}