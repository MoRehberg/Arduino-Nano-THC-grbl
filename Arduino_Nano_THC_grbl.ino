// THC Moritz Rehberg 26.04.2018


/*
Variablen:

	I:	NH
		LimitDown
		zDir
		zPuls
		THCUp
		THCDown
		THCArc
		SpindleEnable

*/	

	//O:
        int PauseProgram    = 11;
		int ArcEnable  = 12;
		int zDir            = 13;
		int zPuls          = A0;

		

	//_Var:

		int icountSteps = 0;
		int iStep = 0;
		int iVelo = 0;	//mm/min
		int iDistRelease = 2;	//mm Freifahren
                int iDistPlunge = 2;    //mm Einstechtiefe
		int StepsPermm = 40;	// Schrittweite
                int iReleaseCurrent = 0;
		boolean ReleaseReset = 0;
		int iReleasePlunge = 0;
		boolean PlungeReset;
		




void setup(){

    Serial.begin(9600);
  
	  pinMode(3, INPUT);
	  digitalWrite(3, HIGH);	//activate pullup
	  pinMode(4, INPUT);
	  digitalWrite(4, HIGH);	//activate pullup
	  pinMode(5, INPUT);
	  digitalWrite(5, HIGH);	//activate pullup
	  pinMode(6, INPUT);
	  digitalWrite(6, HIGH);	//activate pullup
	  pinMode(7, INPUT);
	  digitalWrite(7, HIGH);	//activate pullup
	  pinMode(8, INPUT);
	  digitalWrite(8, HIGH);	//activate pullup
	  pinMode(9, INPUT);
	  digitalWrite(9, HIGH);	//activate pullup
	  pinMode(10, INPUT);
	  digitalWrite(10, HIGH);	//activate pullup
	
	
	  pinMode(PauseProgram, OUTPUT);
	  pinMode(ArcEnable, OUTPUT);
	  pinMode(zDir, OUTPUT);
	  pinMode(zPuls, OUTPUT);

	  iDistRelease = iDistRelease * StepsPermm;
	  iDistPlunge = iDistPlunge * StepsPermm;
	
	
}


//digitalRead


void   loop(){

  //int!!
  
boolean NH				 = !digitalRead(3);
boolean LimitDown		 = ! digitalRead(4);
boolean IzDir 			 =  digitalRead(5);
boolean IzPuls			 =  digitalRead(6);
boolean THCUp			 = ! digitalRead(7);
boolean THCDown			 = ! digitalRead(8);
boolean THCArc			 = ! digitalRead(9);
boolean SpindleEnable	 = ! digitalRead(10);






//IF NH, THEN Ausgaenge aus, PauseProgramm (1s Impuls)



if (NH) {
        
	digitalWrite(PauseProgram, LOW);
	digitalWrite(ArcEnable, LOW);
	  Serial.println("Not-Halt");
	iStep = 0;
}



else   {

					

//Schrittkette THC

	

	if (SpindleEnable && (iStep == 0) && ! NH) {

		iStep = 1; 		// initial, wenn schrittkette auf 0
		
	}
	
	else if (! SpindleEnable) {
  
                digitalWrite(zDir, IzDir);      //Signale von grbl Durchschalten
                digitalWrite(zPuls, IzPuls);
                digitalWrite(ArcEnable, LOW);
		iStep = 0;
	}



switch(iStep){

//Startsignal dann Schrittkette start, sonst disable move, Plasma aus

case 0:		                                    //Alle ausgänge aus
                digitalWrite(PauseProgram, LOW);
	        digitalWrite(ArcEnable, LOW);       
                  Serial.println("Schritt 0");
                if (SpindleEnable){
                iStep = 1;
                }
                break;
            
case 1:		                                    //Programm pause
        digitalWrite(PauseProgram, HIGH);   
                  Serial.println("Schritt 1");
                ReleaseReset = false;  
		iStep = 2;



case 2:		                                    //Antasten    
                Serial.println("Schritt 2");

                if(!LimitDown){      
	
		 		  moveDown(zDir,zPuls);
		
                  break;
                
                }


		else {
				  ReleaseReset = false;
                  iStep = 3;
              
                }
                
		
case 3:		                                    //Freifahren
     
                  Serial.println("Schritt 3");		

		if (iDistRelease >= iReleaseCurrent)	{
		
			moveUp(zDir,zPuls);
			
                       iReleaseCurrent++;

          //      Serial.println(digitalRead(PinStep));	

                      if (ReleaseReset){
                       iReleaseCurrent = 0;
                      }


                        Serial.println(iReleaseCurrent);

			break;
		}

		ReleaseReset = true;
		iStep = 4;
		


case 4:		                                     //Start Lichtbogen
                  Serial.println("Schritt 4");

                digitalWrite(ArcEnable, HIGH);     
                
                if (THCArc){          //continue when Arc is on
		  iStep = 5;
		}

                break;

case 5:		                                    //Einstechen

                  Serial.println("Schritt 5");

		   if(! THCUp){
			                                                          //weg einfügen
			 moveDown(zDir,zPuls);
			 break;
			   
		   }
		
			iStep = 6;
		


case 6:                                             //End Programm Pause

                  Serial.println("Schritt 6");

               digitalWrite(PauseProgram, LOW);    

               iStep = 7;



case 7:                                             //LOOP moveUp(),moveDown()

//!!!!!Abfrage ArcOn else Pause, Schritt 0?

                  Serial.println("Schritt 7");

               if (THCUp){
                 
                moveUp(zDir,zPuls); 
                
                  Serial.println("Up");                
                
                break;
                
               }
               
               else if (THCDown){
                 
                moveDown(zDir,zPuls);
               
                  Serial.println("Down");    
               
                break;
                
               }


}

}

}



int moveDown(int PinDir, int PinStep){			//Funktion Z-Achse senken
  digitalWrite(PinDir, LOW);
  
  
  delay(1);
	
  digitalWrite(PinStep, !digitalRead(PinStep));
 
  
}

int moveUp(int PinDir, int PinStep){				//Funktion Z-Achse anheben
  digitalWrite(PinDir, HIGH);
  
	
  delay(100);
  
  digitalWrite(PinStep, !digitalRead(PinStep));
  

  
}



boolean TON(long TimeValueMS, boolean IN){						// Timer
  long oldTime;
  long newTime;
  boolean OUT;
  
if (IN){
  newTime = millis();
    
    if (newTime - oldTime >= TimeValueMS){
      
     return 1;
      
    }
          
}
 
else {
 
  oldTime = millis();
  newTime = millis();
  return 0;
} 
  
}

