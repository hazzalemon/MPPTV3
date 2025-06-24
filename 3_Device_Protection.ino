

void Device_Protection(){
  //ERROR COUNTER RESET
  currentRoutineMillis = millis();
  if(currentErrorMillis-prevErrorMillis>=errorTimeLimit){                                           //Run routine every millisErrorInterval (ms)
    prevErrorMillis = currentErrorMillis;                                                           //Store previous time
    if(errorCount<errorCountLimit){errorCount=0;}                                                   //Reset error count if it is below the limit before x milliseconds  
    else{}                                                                                          // TO ADD: sleep and charging pause if too many errors persists   
  } 
  //FAULT DETECTION     
  ERR = 0;                                                                                          //Reset local error counter
  if((temp_in>temperatureMax)||(temp_out > temperatureMax))                           {OTE=1;ERR++;errorCount++;}else{OTE=0;}  //OTE - OVERTEMPERATURE: System overheat detected
  if(cin>currentInAbsolute)                       {IOC=1;ERR++;errorCount++;}else{IOC=0;}  //IOC - INPUT  OVERCURRENT: Input current has reached absolute limit
  if(cout>currentOutAbsolute)                     {OOC=1;ERR++;errorCount++;}else{OOC=0;}  //OOC - OUTPUT OVERCURRENT: Output current has reached absolute limit 
  if(vout>voltageBatteryMax+voltageBatteryThresh) {OOV=1;ERR++;errorCount++;}else{OOV=0;}  //OOV - OUTPUT OVERVOLTAGE: Output voltage has reached absolute limit                     
  if(vin<vInSystemMin&&vout<vInSystemMin){FLV=1;ERR++;errorCount++;}else{FLV=0;}  //FLV - Fatally low system voltage (unable to resume operation)

  if(output_Mode==0){                                                                               //PSU MODE specific protection protocol
    REC = 0; BNC = 0;                                                                               //Clear recovery and battery not connected boolean identifiers
  }
  else{                                                                                             //Charger MODE specific protection protocol
    if(vout<vInSystemMin)                   {BNC=1;ERR++;}      else{BNC=0;}               //BNC - BATTERY NOT CONNECTED (for charger mode only, does not treat BNC as error when not under MPPT mode)
    if(vin<voltageBatteryMax+voltageDropout){IUV=1;ERR++;REC=1;}else{IUV=0;}               //IUV - INPUT UNDERVOLTAGE: Input voltage is below max battery charging voltage (for charger mode only)     
  } 
}
