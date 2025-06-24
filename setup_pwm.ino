#define GPIO_ABUCK_HI_OUT 2   //High side solar PHASE1
#define GPIO_ABUCK_LO_OUT 1   //Low Side solar PHASE1
#define GPIO_BBUCK_HI_OUT 4  //High side solar PHASE2
#define GPIO_BBUCK_LO_OUT 3  //Low Side solar PHASE2

#define GPIO_ABOOST_HI_OUT 12 //High side bat PHASE1
#define GPIO_ABOOST_LO_OUT 11 //Low Side bat PHASE1
#define GPIO_BBOOST_HI_OUT 10 //High side bat PHASE2
#define GPIO_BBOOST_LO_OUT 9  //Low Side bat PHASE2





// #define GPIO_SCOPE_Trigger_Sig 38   //For testing - trigger scope to monitor signals at start up, PWM changes and disabling

int frequency = 160000000;
// float PWM = 0;

//float Rise_delay = 200-9f;

bool pwm0 = false;
bool pwm1 = false;
bool buckEnable= false;


//************************************************************
void MCPWM_SetUP(){

//we set up the MCPWM to create complimentary PWM with a set dead time on rising edges...

mcpwm_group_set_resolution(MCPWM_UNIT_0, 160000000);
mcpwm_timer_set_resolution(MCPWM_UNIT_0, MCPWM_TIMER_0, 160000000);
mcpwm_timer_set_resolution(MCPWM_UNIT_0, MCPWM_TIMER_1, 160000000);  //Found necessary to apply this to all timers
mcpwm_group_set_resolution(MCPWM_UNIT_1, 160000000);
mcpwm_timer_set_resolution(MCPWM_UNIT_1, MCPWM_TIMER_0, 160000000);
mcpwm_timer_set_resolution(MCPWM_UNIT_1, MCPWM_TIMER_1, 160000000);

//Config structure for timers....
 mcpwm_config_t pwm_config;
    pwm_config.frequency = 40000;    //frequency = 40KHz
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
   
   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);  
   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config); 

   mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);  
   mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config); 

   delay(20); 
   //To sync times and apply a phase shift, Enabling sync output of another timer by invoking mcpwm_set_timer_sync_output()
   mcpwm_sync_config_t sync_config;
   sync_config.sync_sig = MCPWM_SELECT_TIMER0_SYNC;
   sync_config.timer_val = 500;
   sync_config.count_direction = MCPWM_TIMER_DIRECTION_UP;
  
   //and selecting desired event to generate sync output from:-
   mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_SWSYNC_SOURCE_TEZ);   //'MCPWM_SWSYNC_SOURCE_TEZ' = the sync signal is generated when Timer0 counts to zero
   mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_1, &sync_config);
  // do the same for boost section of 
  mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_SWSYNC_SOURCE_TEZ);   //'MCPWM_SWSYNC_SOURCE_TEZ' = the sync signal is generated when Timer0 counts to zero
  mcpwm_sync_configure(MCPWM_UNIT_1, MCPWM_TIMER_1, &sync_config);

  mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 70, 70); 
  mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 70, 70);
  mcpwm_deadtime_enable(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 70, 70);
  mcpwm_deadtime_enable(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 70, 70);

  // mcpwm_deadtime_enable(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_BYPASS_FED, 1000, 1000);
   
  //  mcpwm_deadtime_enable(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_ACTIVE_HIGH_MODE,50,50);
  //  mcpwm_deadtime_enable(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_ACTIVE_HIGH_MODE,50,50);
  //  mcpwm_deadtime_enable(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_ACTIVE_HIGH_MODE,50,50);
  //  mcpwm_deadtime_enable(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_ACTIVE_HIGH_MODE,50,50);

   //PWM starts operating using the set PWM Duty value...
   mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_A,0);
   mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_A,0);
   mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_A,0);
   mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_A,0);

 delay (20);
 //mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);  
 //vTaskDelay(pdMS_TO_TICKS(10));
 delay(20);
// mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);   //gives a clean cycle start to PWM generator, no part duty waveforms
 
 delay (17);
 mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0A,GPIO_ABUCK_HI_OUT);   // PWM generation commences following init commands
 mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0B,GPIO_ABUCK_LO_OUT);   // Phase 1 GPIOs

 mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM1A,GPIO_BBUCK_HI_OUT);   // PWM generation commences following init commands
 mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM1B,GPIO_BBUCK_LO_OUT);   // Phase 2 GPIOs

// now do same for output side fets
 mcpwm_gpio_init(MCPWM_UNIT_1,MCPWM0A,GPIO_ABOOST_HI_OUT);   // PWM generation commences following init commands
 mcpwm_gpio_init(MCPWM_UNIT_1,MCPWM0B,GPIO_ABOOST_LO_OUT);   // Phase 1 GPIOs

 mcpwm_gpio_init(MCPWM_UNIT_1,MCPWM1A,GPIO_BBOOST_HI_OUT);   // PWM generation commences following init commands
 mcpwm_gpio_init(MCPWM_UNIT_1,MCPWM1B,GPIO_BBOOST_LO_OUT);   // Phase 2 GPIOs

  // digitalWrite(GPIO_SCOPE_Trigger_Sig, HIGH);     //Toggle a pin..for scope diagnostic purposes
  buckEnable=1; 
}

//***************************************************************
void buck_Disable(void){
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_A,0);
        phaseA_trim = 0;
  
}

//****************************************************************
void Set_buck_PWM(float duty){
    // if(duty>4){      
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_B,duty+(phaseA_trim/100.0));
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_A,duty+(phaseA_trim/100.0));
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_B,duty);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_A,duty);

        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_B,100);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_A,100);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_B,100);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_A,100);
    // }
    // else{
    //   buck_Disable();
    // }
}

void Set_singlephase_buck_PWM(float duty){
    // if(duty>4){      
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_B,duty);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_A,duty);

        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_B,100);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_A,100);
    // }
    // else{
    //   buck_Disable();
    // }
}


void Set_boost_PWM(float duty){
    // if(duty>4){      
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_B,100);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_A,100);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_B,100);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_A,100);

        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_B,duty);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_A,duty);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_B,duty);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_A,duty);


    // }
    // else{
    //   buck_Disable();
    // }
}

void Set_singlephase_boost_PWM(float duty){
    // if(duty>4){      
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_B,100);
        mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_1,MCPWM_GEN_A,100);

        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_B,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_0,MCPWM_GEN_A,0);
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_B,duty-(phaseA_trim/100.0));
        mcpwm_set_duty(MCPWM_UNIT_1,MCPWM_TIMER_1,MCPWM_GEN_A,duty-(phaseA_trim/100.0));
    // }
    // else{
    //   buck_Disable();
    // }
}


void Set_PWM(float duty){

  if(duty > 100){
    buck_mode = 0;
    if (prev_buck_mode != buck_mode){
      prev_buck_mode = buck_mode;
      phaseA_trim = 0;
    }
    Set_boost_PWM(200-duty);
  }
  else if(duty == 100){
    buck_mode = 1;
    Set_buck_PWM(float(99.9));
  }
  else{
    buck_mode = 1;
    Set_buck_PWM(duty);
  }
}

void Set_PWMA(float duty){

  if(duty > 100){
    buck_mode = 0;
    if (prev_buck_mode != buck_mode){
      prev_buck_mode = buck_mode;
      phaseA_trim = 0;
    }
    Set_singlephase_boost_PWM(200-duty);
  }
  else if(duty == 100){
    buck_mode = 1;
    Set_singlephase_buck_PWM(float(99.9));
  }
  else{
    buck_mode = 1;
    Set_singlephase_buck_PWM(duty);
  }
}

void Set_PWMB(float duty){

  if(duty > 100){
    buck_mode = 0;
    if (prev_buck_mode != buck_mode){
      prev_buck_mode = buck_mode;
      phaseA_trim = 0;
    }
    Set_boost_PWM(200-duty);
  }
  else if(duty == 100){
    buck_mode = 1;
    Set_buck_PWM(float(99.9));
  }
  else{
    buck_mode = 1;
    Set_buck_PWM(duty);
  }
}
//**********************************************************************

