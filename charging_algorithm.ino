// void buck_Enable(){                                                                  //Enable MPPT Buck Converter
//   buckEnable = 1;
//   digitalWrite(buck_EN,HIGH);
//   digitalWrite(LED,HIGH);
// }
// void buck_Disable(){   
//   PWM = 0;
//   Set_PWM(0);                                                              //Disable MPPT Buck Converter
//   // buckEnable = 0; 
//   // digitalWrite(buck_EN,LOW);
//   // digitalWrite(LED,LOW);
//   // PWM = 0;
// }   
void predictivePWM(){                                                                //PREDICTIVE PWM ALGORITHM 
  if(vin<=0){PPWM=0;}                                                       //Prevents Indefinite Answer when voltageInput is zero
  else if(vin <= vout) {
    PPWM = (1000 + (1.0 - (vin/vout))*1000);
    // Serial.println((String)"boost: " + PPWM + " Vin: " + vin + "Vout: " + vout);
  }  
  else{
    PPWM =(vout/vin)*1000;
    // Serial.println((String)"buck: " + PPWM + " Vin: " + vin + "Vout: " + vout);
  }            //Compute for predictive PWM Floor and store in variable
  PPWM = constrain(PPWM,0,pwmMaxLimited);
}   

void PWM_Modulation(){
  if(output_Mode==0){PWM = constrain(PWM,0,pwmMaxLimited);}                          //PSU MODE PWM = PWM OVERFLOW PROTECTION (limit floor to 0% and ceiling to maximim allowable duty cycle)
  else{
    predictivePWM();                                                                 //Runs and computes for predictive pwm floor
    PWM = constrain(PWM,PPWM,pwmMaxLimited);                                         //CHARGER MODE PWM - limit floor to PPWM and ceiling to maximim allowable duty cycle)                                       
  } 
  Serial.println((String)"setting PWM: " + float(PWM)/10);                                                                   //Reset IUV recovery boolean identifier 
  Set_PWM(float(PWM)/10);                                                        //Set PWM duty cycle and write to GPIO when buck is enabled
  // buck_Enable();                                                                     //Turn on MPPT buck (IR2104)
}
     
void Charging_Algorithm(){
  if(ERR>0||chargingPause==1){buck_Disable(); REC = 1;}                                       //ERROR PRESENT  - Turn off MPPT buck when error is present or when chargingPause is used for a pause override
  else{
    if(REC==1){                                                                      //IUV RECOVERY - (Only active for charging mode)
      REC=0;   
      Serial.println("IN REC");                                                                   //Reset IUV recovery boolean identifier 
      // buck_Disable();                                                                //Disable buck before PPWM initialization
      // lcd.setCursor(0,0);lcd.print("POWER SOURCE    ");                              //Display LCD message
      // lcd.setCursor(0,1);lcd.print("DETECTED        ");                              //Display LCD message 
      Serial.println("> Solar Panel Detected");                                      //Display serial message
      Serial.print("> Computing For Predictive PWM ");                               //Display serial message 
      for(int i = 0; i<40; i++){Serial.print(".");delay(30);}                        //For loop "loading... effect
      Serial.println("");                                                            //Display a line break on serial for next lines  
      // Read_Sensors();
      predictivePWM();
      Serial.println((String)" calculated PPWM: " + PPWM);   
      PWM = PPWM; 
      // lcd.clear();
    }  
    else{                                                                            //NO ERROR PRESENT  - Continue power conversion              
      /////////////////////// CC-CV BUCK PSU ALGORITHM ////////////////////////////// 
      if(MPPT_Mode==0){   
        Serial.println("IN CC_CV mode");                                                                   //Reset IUV recovery boolean identifier                                               //CC-CV PSU MODE
        if(currentOutput>currentCharging)       {PWM--;}                             //Current Is Above → Decrease Duty Cycle
        else if(voltageOutput>voltageBatteryMax){PWM--;}                             //Voltage Is Above → Decrease Duty Cycle   
        else if(voltageOutput<voltageBatteryMax){PWM++;}                             //Increase duty cycle when output is below charging voltage (for CC-CV only mode)
        else{}                                                                       //Do nothing when set output voltage is reached 
        PWM_Modulation();                                                            //Set PWM signal to Buck PWM GPIO       
      }     
        /////////////////////// MPPT & CC-CV CHARGING ALGORITHM ///////////////////////  
      else{                                                                                                                                                         
        if(cout>currentCharging){PWM--;}                                      //Current Is Above → Decrease Duty Cycle
        else if(vout>voltageBatteryMax){PWM--;}                               //Voltage Is Above → Decrease Duty Cycle   
        else{                                                                          //MPPT ALGORITHM
          if(powerInput>powerInputPrev && vin>=voltageInputPrev + 0.01)     {PWM--;}  //  ↑P ↑V ; →MPP  //D--
          else if(powerInput>=powerInputPrev && vin<=(voltageInputPrev)){PWM++;}  //  ↑P ↓V ; MPP←  //D++
          else if(powerInput<powerInputPrev && vin>voltageInputPrev){PWM++;}  //  ↓P ↑V ; MPP→  //D++
          else if(powerInput<powerInputPrev && vin<voltageInputPrev){PWM--;}  //  ↓P ↓V ; ←MPP  //D--
          else if(vout<voltageBatteryMax)                           {PWM++;}  //  MP MV ; MPP Reached - 
          powerInputPrev   = powerInput;                                               //Store Previous Recorded Power
          voltageInputPrev = vin;                                             //Store Previous Recorded Voltage        
        }  
  
        PWM_Modulation();                                                              //Set PWM signal to Buck PWM GPIO                                                                       
      }  
    }
  }
}
