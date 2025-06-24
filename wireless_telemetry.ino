#ifdef blynk
void setupWiFi(){
  if(enableWiFi==1){
    Blynk.begin(auth,ssid,pass);
    WIFI = 1;
  }
}

void Wireless_Telemetry(){
  ////////// WIFI TELEMETRY ////////// 
  if(enableWiFi==1){
    int LED1, LED2, LED3, LED4;                      //Declare LED brightness variable 
    if(buckEnable==1)      {LED1=200;}else{LED1=0;}  //BATTERY CHARGING STATUS
    if(batteryPercent>=99 ){LED2=200;}else{LED2=0;}  //FULL CHARGE STATUS
    if(batteryPercent<=10) {LED3=200;}else{LED3=0;}  //LOW BATTERY STATUS
    if(IUV==0)             {LED4=200;}else{LED4=0;}  //PV INPUT PRESENCE STATUS

    Blynk.run();  
    Blynk.virtualWrite(V1,powerInput); 
    Blynk.virtualWrite(V2,batteryPercent);
    Blynk.virtualWrite(V3,vin);    
    Blynk.virtualWrite(V4,cin);   
    Blynk.virtualWrite(V5,vout); 
    Blynk.virtualWrite(V6,cout); 
    Blynk.virtualWrite(V7,WiFi.RSSI()); 
    Blynk.virtualWrite(V8,Wh/1000); 
    Blynk.virtualWrite(V9,energySavings);       
    Blynk.virtualWrite(V10,LED1);               //LED - Battery Charging Status
    Blynk.virtualWrite(V11,LED2);               //LED - Full Battery Charge Status
    Blynk.virtualWrite(V12,LED3);               //LED - Low Battery Charge Status
    Blynk.virtualWrite(V13,LED4);               //LED - PV Harvesting
    
    Blynk.virtualWrite(V14,voltageBatteryMin);  //Minimum Battery Voltage (Read & Write)
    Blynk.virtualWrite(V15,voltageBatteryMax);  //Maximum Battery Voltage (Read & Write)
    Blynk.virtualWrite(V16,currentCharging);    //Charging Current  (Read & Write)
    Blynk.virtualWrite(V17,electricalPrice);    //Electrical Price  (Write)
  }
  ////////// WIFI TELEMETRY ////////// 
  if(enableBluetooth==1){
    //ADD BLUETOOTH CODE
  }
}
#endif

  void init_mqtt(){
    client.setServer(mqtt_broker, mqtt_port);
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
  while (!client.connected()) {
    String client_id = "MPPTV3";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
    } else {
        Serial.print(client.state());
        delay(2000);
    }
  }
  }


void read_BMS(){
  
}


void mqtt_publish(){
  client.publish("SolarVoltage", String(vin).c_str(), true);
  client.publish("BattVoltage", String(vout).c_str(), true);
  client.publish("SolarCurrent", String(cin).c_str(), true);
  client.publish("BattCurrent", String(cout).c_str(), true);
  client.publish("PowerInput", String(powerInput).c_str(), true);
  client.publish("KiloWattHours", String(Wh/1000).c_str(), true);
  client.publish("WiFiRSSI", String(WiFi.RSSI()).c_str(), true);
  client.publish("inTemp", String(temp_in).c_str(), true);
  client.publish("outTemp", String(temp_out).c_str(), true);

  

}
  