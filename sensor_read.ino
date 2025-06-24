double read_in_temp() {
  // double intemp = input_therm.read();
  double intemp = analogRead(7);
  intemp = calc_temp(intemp, 10000, 3380, 10000);
  // intemp = (intemp/((3.3-intemp)/10000));
  return intemp;
}

double read_out_temp() {
  // double outtemp = 0;  // output_therm.read();
  double outtemp = analogRead(8);
  outtemp = calc_temp(outtemp, 10000, 3380, 10000);
  // outtemp = (outtemp/((3.3-outtemp)/10000));
  return outtemp;
}

double read_phase_a_cur() {
  ads1015.setGain(GAIN_TWO);
  double temp = ads1015.computeVolts(ads1015.readADC_SingleEnded(0));
  // current is  ((V-offset)/amp_gain)/R = I
  double cur = -((temp - 1.65) / 50) / 0.002;
  return cur;
}

double read_phase_b_cur() {
  ads1015.setGain(GAIN_TWO);
  double temp = ads1015.computeVolts(ads1015.readADC_SingleEnded(1));
  // current is  (V/amp_gain)/R = I
  double cur = -((temp - 1.65) / 50) / 0.002;
  return cur;
}

double read_in_cur(double ref) {
  ads1015_B.setGain(GAIN_TWO);
  double temp = 0;
  for (int i = 0; i < avgCountCS; i++) {
    temp = temp + ads1015_B.computeVolts(ads1015_B.readADC_SingleEnded(1));
  }
  // current is  (V/amp_gain)/R = I
  double cur = (((temp / avgCountCS) - ref) / 50) / 0.0005;
  return cur;
}

double read_out_cur(double ref) {
  ads1015.setGain(GAIN_TWO);
  double temp = ads1015.computeVolts(ads1015.readADC_SingleEnded(2));
  // current is  (V/amp_gain)/R = I
  // Serial.print((String)"in cur: " + temp)
  double cur = ((temp - ref) / 50) / 0.0005;
  return cur;
}

double read_in_volt() {
  ads1015_B.setGain(GAIN_TWO);
  double temp = 0;
  for (int i = 0; i < avgCountVS; i++) {
    temp = temp + ads1015_B.computeVolts(ads1015_B.readADC_SingleEnded(0));
  }
  // volts is v divider: (4.125/104.125)* v_meas
  double volts = (temp / avgCountVS) / (4.125 / 104.125);
  return volts;
}

double read_out_volt() {
  ads1015.setGain(GAIN_TWO);
  double temp = ads1015.computeVolts(ads1015.readADC_SingleEnded(3));
  double volts = temp / (4.125 / 104.125);
  return volts;
}

double cal_in_cur(double cur) {
  return cur * 0.6664 - 0.04624;
  return cur;
}

double cal_out_cur(double cur) {
  return cur * -0.973 - 0.006056;
  return cur;
}

double calc_temp(double adcVal, uint16_t nomRes, uint16_t bCoef, uint16_t SeriesRes) {
  uint8_t i;
  uint16_t sample;
  float val = 0;
  // convert the value to resistance
  // val = 1-(ADC_RESOLUTION / adcVal - 1);
  val = ((ADC_RESOLUTION*10000)/(ADC_RESOLUTION-adcVal))-10000;
  // val = SeriesRes * adcVal;

#ifdef VERBOSE_SENSOR_ENABLED
  Serial.print("Thermistor resistance ");
  Serial.println(val);
#endif

  float steinhart;
  steinhart = val / nomRes;  // (R/Ro)
#ifdef PANSTAMP_NRG
  steinhart = logf(steinhart);  // ln(R/Ro)
#else
  steinhart = log(steinhart);  // ln(R/Ro)
#endif
  steinhart /= bCoef;                                // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                       // Invert
  steinhart -= 273.15;                               // convert to C

  return (int)(steinhart);
}

void read_sensors() {
  double ref = ads1015_B.computeVolts(ads1015_B.readADC_SingleEnded(2));
  // Serial.print((String)"ref: " + ref );
  temp_in = read_in_temp();
  temp_out = read_out_temp();
  Serial.print("in temp: ");
  Serial.print(temp_in);
  Serial.print(" Temp out: ");
  Serial.println(temp_out);
  vin = read_in_volt();
  vout = read_out_volt();
  cin = read_in_cur(ref);
  // cin = cin - cin_tare;
  cin = cal_in_cur(cin);
  cout = read_out_cur(ref);
  // cout = cout - cout_tare;
  cout = cal_out_cur(cout);
  c_a = read_phase_a_cur();
  c_b = read_phase_b_cur();
  powerInput = vin * cin;
  powerOutput = vout * cout;
  outputDeviation = (vout / 20) * 100.000;

  batteryPercent = ((voltageOutput - voltageBatteryMin) / (voltageBatteryMax - voltageBatteryMin)) * 101;
  batteryPercent = constrain(batteryPercent, 0, 100);

  currentRoutineMillis = millis();
  if (currentRoutineMillis - prevRoutineMillis >= millisRoutineInterval) {     //Run routine every millisRoutineInterval (ms)
    prevRoutineMillis = currentRoutineMillis;                                  //Store previous time
    Wh = Wh + (powerInput / (3600.000 * (1000.000 / millisRoutineInterval)));  //Accumulate and compute energy harvested (3600s*(1000/interval))
    kWh = Wh / 1000.000;
    MWh = Wh / 1000000.000;
    daysRunning = timeOn / (86400.000 * (1000.000 / millisRoutineInterval));  //Compute for days running (86400s*(1000/interval))
    timeOn++;                                                                 //Increment time counter
  }
}
