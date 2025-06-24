#define DISP_SDA 34
#define DISP_SCL 33

#define BUTTON_SELECT_PIN 5 // pin for SELECT button
#define BUTTON_UP_PIN 6 // pin for UP button 
#define BUTTON_DOWN_PIN 13 // pin for DOWN button
#define BUTTON_BACK_PIN 21 // pin for returning to menu from sub-m

#include "U8g2lib.h"

unsigned long nextMillis = 0;
byte framesCount = 5;
byte currentFrame = framesCount;


// GEMItem menuItemVoltMax("Max Batt volts:", voltageBatteryMax);
// GEMItem menuItemVoltMin("Min Batt volts:", voltageBatteryMin);
// GEMItem menuItemCurLim("Current Limit:", PWM_MaxDC);
// GEMItem menuItemMode("MPPT mode enable:", output_Mode);
// GEMItem menuItemWIFI("Enable WIFI:", enableWiFi);
// GEMPage menuPageMain("settings");
// void dash(); // Forward declaration
// GEMItem menuItemButton("dashboard", dash);

// GEM_u8g2 menu(u8g2, GEM_POINTER_ROW, GEM_ITEMS_COUNT_AUTO);


void setupMenu() {
  // Add menu items to menu page
  menuPageMain.addMenuItem(menuItemVoltMax);
  menuPageMain.addMenuItem(menuItemVoltMin);
  menuPageMain.addMenuItem(menuItemCurLim);
  menuPageMain.addMenuItem(menuItemMode);
  menuPageMain.addMenuItem(menuItemWIFI);
  menuPageMain.addMenuItem(menuItemPause);
  menuPageMain.addMenuItem(menuItemButton);
  // menuPageMain.addMenuItem(menuItemBool);
  // menuPageMain.addMenuItem(menuItemBool);

  // menuPageMain.addMenuItem(menuItemButton);

  // Add menu page to menu and set it as current
  menu.setMenuPageCurrent(menuPageMain);
}


void setup_display() {
  // u8g2.setColorIndex(1);  // set the color to white
  // u8g2.setI2CAddress(0x78);
  // u8g2.begin();
  // u8g2.setBitmapMode(1);

  // define pins for buttons
  // INPUT_PULLUP means the button is HIGH when not pressed, and LOW when pressed
  // since it´s connected between some pin and GND
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP); // down button


  // pinMode(DEMO_PIN, INPUT_PULLUP);
  
}

void dash(){
  menu.context.loop = dashloop;
  menu.context.enter = dashContextEnter;
  menu.context.exit = dashContextExit;
  menu.context.allowExit = false; // Setting to false will require manual exit from the loop
  menu.context.enter();
}
void dashContextEnter() {
  // Clear sreen
  u8g2.clear();

  // Draw initial frame for the case of manual navigation ("Manual" tempo preset)
  // if (interval == 0) {
  // drawFrame(true);
  // }
  // Serial.println("Partying hard is in progress!");
}

void update_display(){
      u8g2.firstPage();
  do {
    /* all graphics commands have to appear within the loop body. */    
    u8g2.setFont(u8g_font_9x18B);
    String buf = ((String)powerInput + "W");
    u8g2.drawStr((64-5 - 9*3), 19, buf.c_str());
    u8g2.setFont(u8g_font_6x10);   
    buf = ((String)"in/out V: " + vin + " " + vout); 
    u8g2.drawStr(5, 15 + 15, buf.c_str()); 
    buf = ((String)"phase A: " + c_a +" "+ c_b );
    u8g2.drawStr(5, 15 + 30, buf.c_str()); 
    buf =  ((String)"in/out A: " + cin + " " + cout);
    u8g2.drawStr(5, 15 + 45, buf.c_str());
  } while ( u8g2.nextPage() );
}

void dashloop(){
  byte key = pinCheck();
  if (key == GEM_KEY_CANCEL) {
    // Exit animation routine if GEM_KEY_CANCEL key was pressed
    menu.context.exit();
  } else {
    u8g2.firstPage();
    vin = vin + 0.1;

    // u8g2.nextPage();
    // u8g2.sendBuffer(); 
    u8g2.firstPage();
  do {
    /* all graphics commands have to appear within the loop body. */    
    u8g2.setFont(u8g_font_9x18B);
    String buf = ((String)powerInput + "W");
    u8g2.drawStr((64+15 - strlen(buf.c_str())*9), 7, buf.c_str());
    u8g2.setFont(u8g_font_6x10);
    if(temp_in > temp_out){
      buf = ((String)"T:" + temp_in + "°");   
    }else{
      buf = ((String)"T:" + temp_out + "°");   
    }
    u8g2.drawStr(110 - strlen(buf.c_str()), 7, buf.c_str());
    buf = ((String)"in/out V: " + vin + " " + vout); 
    u8g2.drawStr(1, 5 + 15, buf.c_str()); 
    buf = ((String)"phase A: " + c_a +" "+ c_b );
    u8g2.drawStr(1, 5 + 30, buf.c_str()); 
    buf =  ((String)"in/out A: " + cin + " " + cout);
    u8g2.drawStr(1, 5 + 45, buf.c_str());
  } while ( u8g2.nextPage() );

  }
}

void dashContextExit() {
  // Reset variables
  
  currentFrame = framesCount;
  // Draw menu back on screen and clear context
  
  menu.reInit();
  menu.drawMenu();
  menu.clearContext();
}

void initButtons() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
}

uint8_t pinCheck() {
  if (nextMillis > millis()){
    return 0;
  }
  nextMillis = millis() + 200;
  int up = digitalRead(BUTTON_UP_PIN);
  int down = digitalRead(BUTTON_DOWN_PIN);
  int sel = digitalRead(BUTTON_SELECT_PIN);
  int back = digitalRead(BUTTON_BACK_PIN);

  if(sel == 0){
    while(sel == 0){
      sel = digitalRead(BUTTON_SELECT_PIN);
      down = digitalRead(BUTTON_DOWN_PIN);
      up = digitalRead(BUTTON_UP_PIN);
      if(down == 0){
        return U8X8_MSG_GPIO_MENU_NEXT;
      }else if(up == 0){
        return U8X8_MSG_GPIO_MENU_PREV;
      }
    }
    return U8X8_MSG_GPIO_MENU_SELECT;
  }
  if(back == 0){
    return U8X8_MSG_GPIO_MENU_HOME;
  }
    if(up == 0){
    return U8X8_MSG_GPIO_MENU_UP;
  }
  if(down == 0){
    return U8X8_MSG_GPIO_MENU_DOWN;
  }else{
    // Serial.println("No button press detected");
    return 0;
  }
}
