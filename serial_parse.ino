void enter_debug(MyCommandParser::Argument *args, char *response){
  DEBUG = 1;
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void leave_debug(MyCommandParser::Argument *args, char *response){
  DEBUG = 0;
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
}

void phaseA(MyCommandParser::Argument *args, char *response) {
  Set_PWMA(args[0].asDouble);
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);

}

void toggle_fan(MyCommandParser::Argument *args, char *response) {
  overrideFan = !overrideFan;
  enableFan = !enableFan;
  digitalWrite(FANPIN,HIGH);
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);

}

void dualphase(MyCommandParser::Argument *args, char *response) {
  Set_PWM(args[0].asDouble);
  strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);

}
