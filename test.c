#include "Includes.h"
#include "DHT11.h"
#include <reg51.h>
#define debounceTime 1000

long currentTime;
long AlarmTime;
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
unsigned char AlarmHH=1;
unsigned char AlarmMM=0;
unsigned char AlarmAP=0;
unsigned char YY1=20,YY2,MM,DD,HH,MIN,SS,AP;
int bitflag=0;
int timeset=0;

void CalculateTempTo7seg(unsigned char temp){
	//Temperature
	dispArr[8]=temp/10;
	dispArr[9]=temp%10;
}

void CalculateDateTo7seg(char * pDateArray){   // Displays date in DD:MM:YYYY format
	//Date
	
	DD=pDateArray[1];
	dispArr[0]=(char)(DD/10);
	dispArr[1]=(char)(DD%10);
	//Months
	MM=pDateArray[2];
	dispArr[2]=(char)(MM/10);
	dispArr[3]=(char)(MM%10);
	//Year
	dispArr[4]=(char)YY1/10;
	dispArr[5]=(char)YY1%10;
	
	YY2=pDateArray[3];
	dispArr[6]=(char)(YY2/10);
	dispArr[7]=(char)(YY2%10);
	YY2=pDateArray[3];
}
void CalculateTimeTo7seg(char *pTimeArray) {  // Displays date in DD:MM:YYYY format
	//Hours
	HH=pTimeArray[2];
	dispArr1[0]=(HH/10);
	dispArr1[1]=(HH%10);
	//Minutes
	MIN=pTimeArray[1];
	dispArr1[2]=(MIN/10);
	dispArr1[3]=(MIN%10);
	//Seconds
	SS=pTimeArray[0];
	dispArr1[4]=(SS/10);
	dispArr1[5]=(SS%10);
	//AM PM
	AP=(unsigned char)pTimeArray[3];
	dispArr1[6]=AP;
	currentTime = 60*pTimeArray[2]+pTimeArray[1] + AP*720;
}

void display(){
	char i;
	
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
}

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
						if(timeset==1)
							Set_DS1307_RTC_Time(AP,HH,MIN,SS);
						Set_DS1307_RTC_Date(DD,MM,YY2,4);
						break;
					}
				}
				for(i=0;i<10;i++)
					dispArr[i]=0;
				
				dispArr1[0]=HH/10;
				dispArr1[1]=HH%10;
				dispArr1[2]=MIN/10;
				dispArr1[3]=MIN%10;
				dispArr1[4]=SS/10;
				dispArr1[5]=SS%10;
				dispArr1[6]=AP;
				display();
				
				if(changetens==0){
					delay(debounceTime);
					if(changetens==0){
						timeset=1;
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
				if(changeunits==0){
					delay(debounceTime);
					if(changeunits==0){
						timeset=1;
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
			if(setpin==0){
				delay(debounceTime);
				if(setpin==0){
					while(setpin!=1);
					bitflag++;
					bitflag%=4;
				}
			}
			delay(100);
		}
}

void setDate(){
		int i;
		bitflag=0;
		while(flag<2){
		for(i=0;i<6;i++)
			dispArr1[i]=0;
		dispArr1[6]=2;	
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
		if(setpin==0){
			delay(debounceTime);
			if(setpin==0){
				while(setpin!=1);
				bitflag++;
				bitflag%=4;
			}
		}
		if(modepin==0){
			delay(debounceTime);
			if(modepin==0){
				while(modepin!=1);
				setTime();
			}
		}
		delay(100);
	}	
}


void setAlarm(){
		int i=0;
		bitflag=0;
		while(flag<2){
			for(i=0;i<10;i++)
				dispArr[i]=0;
			dispArr1[0]=AlarmHH/10;
			dispArr1[1]=AlarmHH%10;
			dispArr1[2]=AlarmMM/10;
			dispArr1[3]=AlarmMM%10;
			dispArr1[4]=0;
			dispArr1[5]=0;
			dispArr1[6]=AlarmAP;
			display();
			if(changetens==0){
				delay(debounceTime);
				if(changetens==0){
					while(changetens!=1 );
					if(bitflag==0){
						AlarmHH--;
						if(AlarmHH==0)
							AlarmHH=12;
					}
					else if(bitflag==1){
						AlarmMM--;
						if(AlarmMM>=60)
							AlarmMM=59;
					}
					else {
							AlarmAP++;
							AlarmAP%=2;
					}
				}
			}
			if(changeunits==0){
				delay(debounceTime);
				if(changeunits==0){
					while(changeunits!=1);
					if(bitflag==0){
						AlarmHH++;
						if(AlarmHH>12)
							AlarmHH=1;
					}
					else if(bitflag==1){
						AlarmMM++;
						AlarmMM%=60;
					}
					else {
							AlarmAP++;
							AlarmAP%=2;
					}
				}
			}
			if(setpin==0){
				delay(debounceTime);
				if(setpin==0){
					while(setpin!=1);
					bitflag++;
					bitflag%=3;
				}
			}
			if(modepin==0){
			delay(debounceTime);
				if(modepin==0){
					while(modepin!=1);
					setDate();
				}
			}
			delay(100);
		}
		AlarmTime=60*AlarmHH+AlarmMM + AlarmAP*720;
		flag=0;
}

	
// Main function
void main()
{	
	modepin=1;
	changeunits=1;
	changetens=1;
	setpin=1;
	InitI2C();	// Initialize i2c pins	
	flag=0;
	//set initial time
	//Set_DS1307_RTC_Time(PM_Time, 12, 59, 58);	// Set time 10:54:00 AM
	//Set initial date
	//Set_DS1307_RTC_Date(24, 8, 77, 0);	// Set Set 24-08-2017
	while(1){
		flag=0;
		CalculateDateTo7seg(Get_DS1307_RTC_Date());
		CalculateTimeTo7seg(Get_DS1307_RTC_Time());
		CalculateTempTo7seg(12);
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
		delay(5000);
	}
}