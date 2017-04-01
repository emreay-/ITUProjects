/*
SCL		-> MUX PIN 3
SDA		-> MUX PIN 13
PIN 19	-> MUX PIN 10 with a pull down 10K resistor.
PIN 19 is used for selecting one of the pressure sensor.
MUX PIN 6 , 7 , 8 , 9 is connected to GND.
*/

#include <SFE_BMP180.h>
#include <Wire.h>

#define selectPin 30
#define local_altitude 100.0
#define SELECT_BMP_NARROW digitalWrite(selectPin,LOW); //X0-Y0 is selected 
#define SELECT_BMP_WIDE digitalWrite(selectPin,HIGH);  //X1-Y1 is selected

struct 
{
  String _id ;
  double  _pres_value;
  double _temp_value;
}Data;

SFE_BMP180 bmp_narrow, bmp_wide;

boolean readSensors(Data *data1, Data *data2);

//struct constructor function
Data newData(String id)
{
  Data data ; 
  data._id = id ;
  return data ;
}

void setup()
{
	boolean failed = false ;
	pinMode(selectPin, OUTPUT);
	
	Serial.begin(9600);
        
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
        Data narrow_data = newData("bmp_narrow");
        Data wide_data = newData("bmp_wide");
  
        boolean sensor_is_read = readSensors(&narrow_data,&wide_data);
  
  
        if(sensor_is_read){
      	  /* print the value to serial terminal */
      	  Serial.println();
      	  Serial.print("provided altitude: ");
      	  Serial.print(local_altitude,0);
      	  Serial.print(" meters ");
      
	  /* Pressure Sensor #1 value */
	  // Print out the measurement:
	  Serial.println("BMP Narrow Data:");
          Serial.print("temperature: ");
	  Serial.print(narrow_data._temp_value,2);
          Serial.print(" deg C ");
	  
	  // Print out the measurement:
	  Serial.print("absolute pressure: ");
	  Serial.print(narrow_data._pres_value,2);
	  Serial.print(" mb ");
	  Serial.print(narrow_data._pres_value*0.0295333727,2);
	  Serial.println(" inHg");

	  Serial.println();
	  /* Pressure Sensor #2 value */
	  // Print out the measurement:
	  Serial.println("BMP Wide Data:");
	  Serial.print("temperature: ");
	  Serial.print(wide_data._temp_value,2);
          Serial.print(" deg C ");
	  
	  // Print out the measurement:
	  Serial.print("absolute pressure: ");
	  Serial.print(wide_data._pres_value,2);
	  Serial.print(" mb ");
	  Serial.print(wide_data._pres_value*0.0295333727,2);
	  Serial.println(" inHg");

	  Serial.println();
       }
      else
	  Serial.println("Error on sensor reading!");
  
  delay(1000);
}
boolean readSensors(Data *data1, Data *data2)
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
			data1->_temp_value = temp_narrow;
			data2->_temp_value = temp_wide;
      
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
					data1->_pres_value = pr_narrow ; 
					data2->_pres_value = pr_wide;
					
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
