/*
Emre Ay
Differential flow rate measuring project for Modelling & Control of 
Biological Systems course, Spring 2015, Istanbul Technical University.

Bosch BMP180 sensors are used with an analog multiplexer. 
*/
#include <SFE_BMP180.h>
#include <Wire.h>
#include <Math.h>
#include <LiquidCrystal.h>

#define selectPin 30
#define local_altitude 100.0
#define SELECT_BMP_NARROW digitalWrite(selectPin,LOW); //X0-Y0 is selected 
#define SELECT_BMP_WIDE digitalWrite(selectPin,HIGH);  //X1-Y1 is selected
#define MBAR2MMHG 0.7500616827042
#define MBAR2PA 100
#define MCUBE2CC 10000
#define air_density 1.184
#define narrow 0.014
#define wide 0.02
#define gas_constant 287.058

SFE_BMP180 bmp_narrow, bmp_wide;

struct BMP_DATA
{
  String _id;
  double _pressure;
  double _temperature;
};

boolean readSensors(BMP_DATA *bmp1, BMP_DATA *bmp2);
BMP_DATA newBmp(String id);
LiquidCrystal lcd(45,43,41,39,37,35);

void setup()
{
	boolean failed = false ;
	pinMode(selectPin, OUTPUT);
	
	Serial.begin(9600);
        lcd.begin(16,2);
        //bmp narrow sensor is activated		
	SELECT_BMP_NARROW
	if(bmp_narrow.begin()){
		Serial.println("BMP NARROW initialized successfully.");
	}
	else
        {
                failed = true ;
        }
 
        //bmp wide sensor is activated
	SELECT_BMP_WIDE
	if(bmp_wide.begin()){
		  Serial.println("BMP WIDE initialized successfully.");
	}
	else 
		failed = true ;

	if(failed){
		Serial.println("BMP sensor initialization failed");
		while (1);
	}
}

void loop()
{
        BMP_DATA narrow_data = newBmp("bmp_narrow");
        BMP_DATA wide_data = newBmp("bmp_wide");
  
        boolean sensor_is_read = readSensors(&narrow_data,&wide_data);
  
  
        if(sensor_is_read){
      	  /* print the value to serial terminal */
      	  Serial.println();
      	  Serial.print("provided altitude: ");
      	  Serial.print(local_altitude,0);
      	  Serial.println(" meters ");
          Serial.println("");
      
	  /* Pressure Sensor #1 value */
	  // Print out the measurement:
	  Serial.println("BMP Narrow Data:");
          Serial.print("temperature: ");
	  Serial.print(narrow_data._temperature,2);
          Serial.print(" deg C ");
	  
	  // Print out the measurement:
	  Serial.print("absolute pressure: ");
	  Serial.print(narrow_data._pressure,2);
	  Serial.print(" mb ");
	  Serial.print(narrow_data._pressure*MBAR2MMHG,2);
	  Serial.println(" mmHg");

	  Serial.println();
	  /* Pressure Sensor #2 value */
	  // Print out the measurement:
	  Serial.println("BMP Wide Data:");
	  Serial.print("temperature: ");
	  Serial.print(wide_data._temperature,2);
          Serial.print(" deg C ");
	  
	  // Print out the measurement:
	  Serial.print("absolute pressure: ");
	  Serial.print(wide_data._pressure,2);
	  Serial.print(" mb ");
	  Serial.print(wide_data._pressure*MBAR2MMHG,2);
	  Serial.println(" mmHg");

	  Serial.println();
          
          double flow, v1,alpha, dummy, delta_pressure,ro1, ro2, temp1, temp2, A1;
          A1 = M_PI*pow(narrow,2);
          temp1 = narrow_data._temperature+273.15;
          temp2 = wide_data._temperature+273.15;
          ro1 = narrow_data._pressure*MBAR2PA/(gas_constant*temp1);
          ro2 = wide_data._pressure*MBAR2PA/(gas_constant*temp2);
          delta_pressure = -(narrow_data._pressure-wide_data._pressure)*MBAR2PA;          
          dummy = (ro2*pow(narrow,2)-ro1*pow(wide,2));
          alpha = sqrt(2*delta_pressure/dummy);
          v1 = wide*alpha;
          flow = A1*v1;

          Serial.print("Flow [cc/s]:");
          Serial.print(flow*MCUBE2CC,3);
          lcd.setCursor(0,0);
          lcd.print(flow*MCUBE2CC,3);
       }
      else
	  Serial.println("Error on sensor reading!");
  
  delay(500);
}
boolean readSensors(BMP_DATA *bmp1, BMP_DATA *bmp2)
{
	char status_narrow, status_wide ;
	double temp_narrow , temp_wide , pr_narrow, pr_wide;
  
	//activate bmp narrow
	SELECT_BMP_NARROW
        //function returns number of ms to wait for succes
        //and 0 for fail
	status_narrow = bmp_narrow.startTemperature(); 
  
        //activate bmp wide
	SELECT_BMP_WIDE	
        //function returns number of ms to wait for succes
        //and 0 for fail	
	status_wide = bmp_wide.startTemperature();
  
        //if the status are successful
	if (status_narrow != 0 && status_wide != 0)
        {
	        // Wait for the measurement to complete:
		delay(max(status_narrow,status_wide));
		
                //activate bmp narrow
		SELECT_BMP_NARROW
                //funciton returns 1 for succes, 0 for fail	
		status_narrow = bmp_narrow.getTemperature(temp_narrow);
		
                //activate bmp wide
		SELECT_BMP_WIDE	
                //funciton returns 1 for succes, 0 for fail	
		status_wide = bmp_wide.getTemperature(temp_wide);

                //if the status are successful
		if(status_wide != 0 && status_narrow != 0)
                {
			/* save the temparature */
			bmp1->_temperature = temp_narrow;
			bmp2->_temperature = temp_wide;
      
			//activate bmp narrow
		        SELECT_BMP_NARROW
                        //function returns number of ms to wait for succes
                        //and 0 for fail, the argument (0-3) is for oversampling value
			status_narrow = bmp_narrow.startPressure(3);
			
			//activate bmp wide
		        SELECT_BMP_WIDE	
                        //function returns number of ms to wait for succes
                        //and 0 for fail, the argument (0-3) is for oversampling value
			status_wide = bmp_wide.startPressure(3);
            
			if(status_narrow != 0 && status_wide != 0)
                        {
				
				delay(max(status_narrow,status_wide));
				
				//activate bmp narrow
		                SELECT_BMP_NARROW
                                //funciton returns 1 for succes, 0 for fail
				status_narrow = bmp_narrow.getPressure(pr_narrow,temp_narrow);
				
				//activate bmp wide
		                SELECT_BMP_WIDE
				status_wide= bmp_wide.getPressure(pr_wide,temp_wide);
				
				
				if(status_narrow != 0 && status_wide != 0)
                                {
					/* save pressure value */
					bmp1->_pressure = pr_narrow ; 
					bmp2->_pressure = pr_wide;
					
					return true ;
				}
				else Serial.println("error reading pressure measurement\n");
			}
			else Serial.println("error starting pressure measurement\n");  
		}
		else Serial.println("error reading temperature measurement\n");  
	 }
	 else Serial.println("error starting temperature measurement\n");  
	 
	 return false ;
}

BMP_DATA newBmp(String id)
{
  BMP_DATA _bmp ; 
  _bmp._id = id ;
  return _bmp ;
}

