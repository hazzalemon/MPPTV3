#include <CommandParser.h>
#include <PubSubClient.h>


// #define blynk
#define MQTT

#ifdef blynk
#define BLYNK_TEMPLATE_ID "TMPL64wHZJXaQ"
#define BLYNK_TEMPLATE_NAME "mppt"
#define BLYNK_AUTH_TOKEN "vtrfu5Mo-Mhvex1uPeEUcwZjPBC_4rkG"


#include <BlynkSimpleEsp32.h>
#include <Blynk.h>

#endif




#include <WiFi.h>
#include <GEM_u8g2.h>
#include <EEPROM.h>  //SYSTEM PARAMETER  - EEPROM Library (By: Arduino)
#include "arduino.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"


// MQTT Broker
const char *mqtt_broker = "192.168.0.11";
const char *topic = "MPPT/1";
const char *mqtt_username = "harry";
const char *mqtt_password = "";
const int mqtt_port = 1883;
String client_id = "MPPTV3";

WiFiClient espClient;
PubSubClient client(espClient);


Adafruit_ADS1015 ads1015;
Adafruit_ADS1015 ads1015_B;
TaskHandle_t Core2;

#define SDA_PIN 36
#define SCL_PIN 37
#define FANPIN 35
#define ADC_RESOLUTION 0xFFF
#define TEMPERATURENOMINAL 25

int pwmMaxLimited = 1900;
int pwmMax = 2000,

    millisRoutineInterval = 250,
    update_count = 0,
    test = 0,
    conv1 = 0,          // SYSTEM PARAMETER -
  conv2 = 0,            // SYSTEM PARAMETER -
  errorCount = 0,       // SYSTEM PARAMETER -
  errorCountLimit = 5,  //  USER PARAMETER - Maximum number of errors
  batteryPercent = 0,   // SYSTEM PARAMETER -

  avgCountVS = 6,  //  CALIB PARAMETER - Voltage Sensor Average Sampling Count (Recommended: 3)
  avgCountCS = 6,  //  CALIB PARAMETER - Current Sensor Average Sampling Count (Recommended: 4)
  avgCountTS = 500,
    errorTimeLimit = 1000,  //  USER PARAMETER - Time interval for reseting error counter (milliseconds)
  ERR = 0,
    phaseA_trim = 0,
    PPWM = 0,
    PWM = 0;  // SYSTEM PARAMETER



bool
  prev_change = 0,
  buck_mode = 0,
  prev_buck_mode = 0,
  bypassEnable = 0,  // SYSTEM PARAMETER -
  WIFI = 0,

  DEBUG = 0,  // This stops the controller from running normal loop, and follows commands from the command line.

  MPPT_Mode = 1,             //   USER PARAMETER - Enable MPPT algorithm, when disabled charger uses CC-CV algorithm
  output_Mode = 1,           //   USER PARAMETER - 0 = PSU MODE, 1 = Charger Mode
  disableFlashAutoLoad = 0,  //   USER PARAMETER - Forces the MPPT to not use flash saved settings, enabling this "1" defaults to programmed firmware settings.
  enablePPWM = 1,            //   USER PARAMETER - Enables Predictive PWM, this accelerates regulation speed (only applicable for battery charging application)
  enableWiFi = 1,            //   USER PARAMETER - Enable WiFi Connection
  enableFan = 1,             //   USER PARAMETER - Enable Cooling Fan
  enableBluetooth = 1,       //   USER PARAMETER - Enable Bluetooth Connection
  enableLCD = 1,             //   USER PARAMETER - Enable LCD display
  enableLCDBacklight = 1,    //   USER PARAMETER - Enable LCD display's backlight
  chargingPause = 0,         // SYSTEM PARAMETER -
  REC = 0,                   // SYSTEM PARAMETER -

  overrideFan = 0,  //   USER PARAMETER - Fan always on
  enableDynamicCooling = 0,
  BNC = 0,  // SYSTEM PARAMETER -
  FLV = 0,  // SYSTEM PARAMETER -
  IUV = 0,  // SYSTEM PARAMETER -
  IOV = 0,  // SYSTEM PARAMETER -
  IOC = 0,  // SYSTEM PARAMETER -
  OUV = 0,  // SYSTEM PARAMETER -
  OOV = 0,  // SYSTEM PARAMETER -
  OOC = 0,  // SYSTEM PARAMETER -
  OTE = 0,  // SYSTEM PARAMETER -
  fanStatus = 0;

double
  temp_in = 0,
  temp_out = 0,
  temperatureFan = 45,
  output_volt = 0,
  vin = 0,
  vout = 0,
  cin = 0,
  cout = 0,
  c_a = 0,
  c_b = 0,
  prev_pwr = 0,
  prev_duty = 0,
  duty = 0,
  in_pwr = 0,
  powerInput = 0.0000,        // SYSTEM PARAMETER - Input power (solar power) in Watts
  powerInputPrev = 0.0000,    // SYSTEM PARAMETER - Previously stored input power variable for MPPT algorithm (Watts)
  powerOutput = 0.0000,       // SYSTEM PARAMETER - Output power (battery or charing power in Watts)
  energySavings = 0.0000,     // SYSTEM PARAMETER - Energy savings in fiat currency (Peso, USD, Euros or etc...)
  voltageInput = 0.0000,      // SYSTEM PARAMETER - Input voltage (solar voltage)
  voltageInputPrev = 0.0000,  // SYSTEM PARAMETER - Previously stored input voltage variable for MPPT algorithm
  voltageOutput = 0.0000,     // SYSTEM PARAMETER - Input voltage (battery voltage)
  currentInput = 0.0000,      // SYSTEM PARAMETER - Output power (battery or charing voltage)
  currentOutput = 0.0000,
  Wh = 0.0000,   // SYSTEM PARAMETER - Stores the accumulated energy harvested (Watt-Hours)
  kWh = 0.0000,  // SYSTEM PARAMETER - Stores the accumulated energy harvested (Kiliowatt-Hours)
  MWh = 0.0000,  // SYSTEM PARAMETER - Stores the accumulated energy harvested (Megawatt-Hours)
  loopT = 0.0000,
  mppt = 0;

float
  voltageBatteryMax = 14.6000,  //   USER PARAMETER - Maximum Battery Charging Voltage (Output V)
  temperatureMax = 85,
  voltageBatteryMin = 5.000,    //   USER PARAMETER - Minimum Battery Charging Voltage (Output V)
  loopTime = 0.0000,
  voltageDropout = 1.0000,  //  CALIB PARAMETER - Buck regulator's dropout voltage (DOV is present due to Max Duty Cycle Limit)
  currentCharging = 30.0000,
  PPWM_margin = 190.0000,         //  CALIB PARAMETER - Minimum Operating Duty Cycle for Predictive PWM (%)
  PWM_MaxDC = 10.0000,            //   USER PARAMETER - Maximum Charging Current (A - Output)
  outputDeviation = 0.0000,       // SYSTEM PARAMETER - Output Voltage Deviation (%)
  vInSystemMin = 8.000,           //  CALIB PARAMETER -
  voltageBatteryThresh = 1.0000,  //  CALIB PARAMETER - Power cuts-off when this voltage is reached (Output V)
  currentOutAbsolute = 50.0000,
  currentInAbsolute = 51.0000,  //  CALIB PARAMETER - Maximum Input Current The System Can Handle (A - Input)
  daysRunning = 0.0000,
  efficiencyRate = 1.0000,   //  CALIB PARAMETER - Theroretical Buck Efficiency (% decimal)
  electricalPrice = 9.5000;  //   USER PARAMETER - Input electrical price per kWh (Dollar/kWh,Euro/kWh,Peso/kWh)

unsigned long
  currentErrorMillis = 0,     //SYSTEM PARAMETER -
  currentButtonMillis = 0,    //SYSTEM PARAMETER -
  currentSerialMillis = 0,    //SYSTEM PARAMETER -
  currentRoutineMillis = 0,   //SYSTEM PARAMETER -
  currentLCDMillis = 0,       //SYSTEM PARAMETER -
  currentLCDBackLMillis = 0,  //SYSTEM PARAMETER -
  currentWiFiMillis = 0,      //SYSTEM PARAMETER -
  currentMenuSetMillis = 0,   //SblynkYSTEM PARAMETER -
  prevButtonMillis = 0,       //SYSTEM PARAMETER -
  prevSerialMillis = 0,       //SYSTEM PARAMETER -
  prevRoutineMillis = 0,      //SYSTEM PARAMETER -
  prevErrorMillis = 0,        //SYSTEM PARAMETER -
  prevWiFiMillis = 0,         //SYSTEM PARAMETER -
  prevLCDMillis = 0,          //SYSTEM PARAMETER -
  prevLCDBackLMillis = 0,     //SYSTEM PARAMETER -
  prevMqttMillis = 0,
  timeOn = 0,          //SYSTEM PARAMETER -
  loopTimeStart = 0,   //SYSTEM PARAMETER - Used for the loop cycle stop watch, records the loop start time
  loopTimeEnd = 0,     //SYSTEM PARAMETER - Used for the loop cycle stop watch, records the loop end time
  secondsElapsed = 0;  //SYSTEM PARAMETER -


char line[128];  //this is a buffer used for Serial Commands
typedef CommandParser<> MyCommandParser;
MyCommandParser parser;

char
#ifdef blynk
  auth[] = BLYNK_AUTH_TOKEN,  //   USER PARAMETER - Input Blynk Authentication Token (From email after registration)
#endif
  ssid[] = "garage",  //   USER PARAMETER - Enter Your WiFi SSID
  pass[] = "paddlepop";



void coreTwo(void *pvParameters) {
#ifdef blynk
  setupWiFi();
  timer.setInterval(1000L, Wireless_Telemetry);  //TAB#7 - WiFi Initialization
  //================= CORE0: LOOP (DUAL CORE MODE) ======================//
  while (1) {
    timer.run();  //TAB#7 - Wireless telemetry (WiFi & Bluetooth)
  }
#endif

#ifdef MQTT
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.begin(ssid, pass);
  init_mqtt();
  while (1) {
    if (prevMqttMillis + 1000 < millis()) {
      prevMqttMillis = millis();
      if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        init_mqtt();
      } else {
        mqtt_publish();
      }
    }
  }

#endif
}


#define DISP_SDA 34
#define DISP_SCL 33

#include "U8g2lib.h"


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE, DISP_SCL, DISP_SDA);  // [full framebuffer, size = 1024 bytes]


GEMItem menuItemVoltMax("Max Batt volts:", voltageBatteryMax);
GEMItem menuItemVoltMin("Min Batt volts:", voltageBatteryMin);
GEMItem menuItemCurLim("Current Limit:", currentCharging);
GEMItem menuItemMode("MPPT mode enable:", output_Mode);
GEMItem menuItemPause("Enable Converter:", chargingPause);
GEMItem menuItemWIFI("Enable WIFI:", enableWiFi);
GEMPage menuPageMain("settings");
void dash();  // Forward declaration
GEMItem menuItemButton("dashboard", dash);

GEM_u8g2 menu(u8g2, GEM_POINTER_ROW, GEM_ITEMS_COUNT_AUTO);


void setup() {
  MCPWM_SetUP();
  Serial.begin(57600);
  analogReadResolution(12);
  u8g2.begin();
  setup_display();
  xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 0, &Core2, 0);
  menu.init();
  setupMenu();
  menu.drawMenu();
  Wire1.begin(SDA_PIN, SCL_PIN);
  ads1015.begin(72, &Wire1);
  ads1015_B.begin(73, &Wire1);
  // register all debug functions for serial parser
  parser.registerCommand("DEBUG", "d", &enter_debug);
  parser.registerCommand("FORCEFAN", "d", &toggle_fan);
  parser.registerCommand("NORM", "d", &leave_debug);
  parser.registerCommand("PHASEA", "d", &phaseA);
  parser.registerCommand("DUALPHASE", "d", &dualphase);
  pinMode(FANPIN, OUTPUT);
  Serial.println("setup MCPWM!");
}

void loop() {
  if (Serial.available() > 0) {
      size_t lineLength = Serial.readBytesUntil('\n', line, 127);
      line[lineLength] = '\0';
      char response[MyCommandParser::MAX_RESPONSE_SIZE];
      parser.processCommand(line, response);
      Serial.println(response);
    }
  if (DEBUG) {
    read_sensors();
    if (menu.readyForKey()) {
      menu.registerKeyPress(pinCheck());
    }
  } else {
    read_sensors();
    // delay(100);
    Device_Protection();
    Charging_Algorithm();
    System_Processes();
    if (menu.readyForKey()) {
      menu.registerKeyPress(pinCheck());
    }
    // update_display();
    // Serial.println((String) "loop time: " + loopTime );
    // Serial.println(temp_in);
    // Serial.println(temp_out);
    update_count++;
    if (update_count > 10) {
      update_count = 0;
      if (c_a > c_b + 0.2) {
        phaseA_trim--;
      } else if (c_a < c_b - 0.2) {
        phaseA_trim++;
      }
    }
    // Serial.println((String)"PLoss: " + (powerInput - powerOutput));
  }
}