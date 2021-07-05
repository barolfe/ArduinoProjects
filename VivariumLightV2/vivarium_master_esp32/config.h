//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

// Solar emulation settings
boolean solar_emulation = true;
Solar solar = Solar(0, 0); // lat, lon -- in radians

// Light fading settings
unsigned int slow_interval = 20;
unsigned int warm_level_cur = 0;
unsigned int cool_level_cur = 0;
unsigned int warm_level_set = 400;  // These are used as max values when in solar mode
unsigned int cool_level_set = 950; // These are used as max values when in solar mode
unsigned int warm_level_cur_solar = 0; // Current calculated solar values -- these are set in setSolarValues()
unsigned int cool_level_cur_solar = 0; // Current calculated solar values -- these are set in setSolarValues()
unsigned long time_last = 0;

// Thunderstorm settings
bool b_in_thunderstorm = false;
bool b_first_thunder = true; 
unsigned long t_thunder_start = 0;
unsigned long t_last_thunder_check = 0;
unsigned int thunder_duration = 60*20; // thunder storm duration, in seconds
float thunder_frac = 0.85; // fraction of current light level to dim to during thunderstrom
 // lightning values -- sequences are on-time, off-time, ... -- must be even, always end with an off time, must have trailing zero
 // C++ will pad with zeros
unsigned int lightning_timings[4][16] = { {50, 50, 100, 40, 30, 30}, {50, 100, 100, 30, 80, 20, 100, 100, 50}, {100, 20, 100, 50}, {40, 40, 40, 100, 150, 30, 100, 50, 70, 50, 80, 10} };  
      
// General light/timer settings
boolean timer_mode = true;
boolean timer_mode_stored = timer_mode;
boolean manual_on = false;
unsigned int hour1 = 7; // simple for now
unsigned int hour2 = 12 + 7;
unsigned long time_last_checked = 0;
unsigned long time_last_update = 0;


// Fan settings
boolean fan_timer = true;
boolean fan_off = false;
boolean b_fans_only_day = true;
unsigned int fan_level_max = 1024;
unsigned int fan_level_set = 0; 
unsigned int fan_duration = 4; // minutes, duration fan is on each time it is triggered on
unsigned int fan_interval  = 10; // minutes, how often the fan is turned on
unsigned int fan_level_min = 0; // Don't PWM plain old DC fans, it doesn't work right

// EEPROM settings
bool b_clear_eeprom = false; // set to true to CLEAR EEPROM (initialization, for example)

// Display settings
bool b_IP_time_flip_switch = true; // acts like a flip switch to show IP/time (you don't need to change it)
