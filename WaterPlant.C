/*************************************************************

  This sketch shows how to write values to Virtual Pins

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App dashboard setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "IDIDIDIIIDIDIDDI"
#define BLYNK_TEMPLATE_NAME "plant"
#define BLYNK_AUTH_TOKEN "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define RelayOut 2

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "WIFI_2.4G";
char pass[] = "Password";
int interval=0;
int interintervalstart=0;
int interintervalstop=0;

int morninghour	=7;
int morningmin	=30;
boolean morningactive=false;

int eveninghour	=20;
int eveningmin	=30;
boolean eveningactive=false;


int wateringTime=360;


boolean issueWaterActive=false;
boolean ManualActive=false;
boolean AutoActive=false;
boolean morningcomplete=false;
boolean eveningcomplete=false;

BlynkTimer timer;
WidgetRTC rtc;

//V0 is virtual value of issueWaterActiveusing for output relay control
//V1 is Manual Switch Button of mobile
//V2 is second of times to show
//V3 terminal string of date that using for watering

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

void clockDisplay(boolean state)
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

 // String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentTime = String(hour()) + ":" + minute();
  String currentDate = String(day()) + "/" + month() + "/" + year() + "-";
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  // Send time to the App
  //Blynk.virtualWrite(V1, currentTime);
  // Send date to the App
  String startText="";
  String endText=" Control Stop ";
  if(ManualActive && !AutoActive){
	startText=" Manual Start ";
  }
  if(!ManualActive && AutoActive){
	startText=" Auto Start ";
  }
  if(ManualActive&& AutoActive){
	startText=" Control Start ";	
  }
  
	String dattim="";
  if(state){
	  dattim=currentDate+currentTime+startText;
  }else{
	  String tim="for " + (String)interval + "s" ;
	  dattim=currentDate+currentTime+endText+tim;
  }
	
	Blynk.virtualWrite(V3, dattim);
}

void pass_wateringtime(){
	if(eveningactive){
		eveningcomplete=true;
		morningcomplete=false;
	}
	if(morningactive){
		morningcomplete=true;
		eveningcomplete=false;
	}
}

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  
  Blynk.virtualWrite(V2,interval);

  if((hour()== morninghour)&& !morningcomplete){
	  if(minute()>= morningmin){
		morningactive=true;
	  }else{
		morningactive=false;
	  }
  }else{
	  morningactive=false;
  }
  
  if((hour()== eveninghour)&& !eveningcomplete){
	  if(minute()>= eveningmin){
		eveningactive=true;
	  }else{
		eveningactive=false;
	  }
  }else{
	  eveningactive=false;
  }
  
  if(morningactive||eveningactive){
	  AutoActive= true;

  }
  
  
  if( ManualActive || AutoActive ){
	  if(ManualActive && AutoActive){		//this function will cancle the schedual automatic watering process
		  pass_wateringtime();
		  AutoActive=false;
	  }
	issueWaterActive=true;
  }
  
  if( !ManualActive && !AutoActive ){
	issueWaterActive=false;
  }    
  
  if(issueWaterActive == true){	
  
		digitalWrite(RelayOut,LOW);
		
		if(interval==1){
			clockDisplay(1);
		}
		interval++;
		if(interval>=wateringTime){
			pass_wateringtime();
			issueWaterActive=false;
			ManualActive=false;
			Blynk.virtualWrite(V0,issueWaterActive); //force manual stop
			AutoActive= false;
		}
  }else{
	  if(interval!=0){
		interintervalstop=interval;
		clockDisplay(0);
		}
		digitalWrite(RelayOut,HIGH);
		interval=0;
  }	  
  Blynk.virtualWrite(V1,issueWaterActive);

}


BLYNK_WRITE(V0)
{
 int  pinValuev1 = param.asInt(); // assigning incoming value from pin V1 to a variable
 
	if(pinValuev1){
		ManualActive = true;
	}else{
		ManualActive = false;
	}
  // process received value
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void setup()
{
  // Debug console
  pinMode(RelayOut,OUTPUT);
  digitalWrite(RelayOut,HIGH);
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)
  timer.setInterval(1000L, myTimerEvent);

}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

