#ifndef LANGUAGE_ALL_H
#define LANGUAGE_ALL_H

extern unsigned char lang_selected;
extern const char** MSG_ALL[];
#define WELCOME_MSG MSG_ALL[lang_selected][0]
#define MSG_SD_INSERTED MSG_ALL[lang_selected][1]
#define MSG_SD_REMOVED MSG_ALL[lang_selected][2]
#define MSG_MAIN MSG_ALL[lang_selected][3]
#define MSG_AUTOSTART MSG_ALL[lang_selected][4]
#define MSG_DISABLE_STEPPERS MSG_ALL[lang_selected][5]
#define MSG_AUTO_HOME MSG_ALL[lang_selected][6]
#define MSG_SET_HOME_OFFSETS MSG_ALL[lang_selected][7]
#define MSG_SET_ORIGIN MSG_ALL[lang_selected][8]
#define MSG_PREHEAT_PLA MSG_ALL[lang_selected][9]
#define MSG_PREHEAT_PLA0 MSG_ALL[lang_selected][10]
#define MSG_PREHEAT_PLA1 MSG_ALL[lang_selected][11]
#define MSG_PREHEAT_PLA2 MSG_ALL[lang_selected][12]
#define MSG_PREHEAT_PLA012 MSG_ALL[lang_selected][13]
#define MSG_PREHEAT_PLA_BEDONLY MSG_ALL[lang_selected][14]
#define MSG_PREHEAT_PLA_SETTINGS MSG_ALL[lang_selected][15]
#define MSG_PREHEAT_ABS MSG_ALL[lang_selected][16]
#define MSG_PREHEAT_ABS0 MSG_ALL[lang_selected][17]
#define MSG_PREHEAT_ABS1 MSG_ALL[lang_selected][18]
#define MSG_PREHEAT_ABS2 MSG_ALL[lang_selected][19]
#define MSG_PREHEAT_ABS012 MSG_ALL[lang_selected][20]
#define MSG_PREHEAT_ABS_BEDONLY MSG_ALL[lang_selected][21]
#define MSG_PREHEAT_ABS_SETTINGS MSG_ALL[lang_selected][22]
#define MSG_COOLDOWN MSG_ALL[lang_selected][23]
#define MSG_SWITCH_PS_ON MSG_ALL[lang_selected][24]
#define MSG_SWITCH_PS_OFF MSG_ALL[lang_selected][25]
#define MSG_EXTRUDE MSG_ALL[lang_selected][26]
#define MSG_RETRACT MSG_ALL[lang_selected][27]
#define MSG_MOVE_AXIS MSG_ALL[lang_selected][28]
#define MSG_MOVE_X MSG_ALL[lang_selected][29]
#define MSG_MOVE_Y MSG_ALL[lang_selected][30]
#define MSG_MOVE_Z MSG_ALL[lang_selected][31]
#define MSG_MOVE_E MSG_ALL[lang_selected][32]
#define MSG_MOVE_E1 MSG_ALL[lang_selected][33]
#define MSG_MOVE_E2 MSG_ALL[lang_selected][34]
#define MSG_MOVE_01MM MSG_ALL[lang_selected][35]
#define MSG_MOVE_1MM MSG_ALL[lang_selected][36]
#define MSG_MOVE_10MM MSG_ALL[lang_selected][37]
#define MSG_SPEED MSG_ALL[lang_selected][38]
#define MSG_NOZZLE MSG_ALL[lang_selected][39]
#define MSG_NOZZLE1 MSG_ALL[lang_selected][40]
#define MSG_NOZZLE2 MSG_ALL[lang_selected][41]
#define MSG_BED MSG_ALL[lang_selected][42]
#define MSG_FAN_SPEED MSG_ALL[lang_selected][43]
#define MSG_FLOW MSG_ALL[lang_selected][44]
#define MSG_FLOW0 MSG_ALL[lang_selected][45]
#define MSG_FLOW1 MSG_ALL[lang_selected][46]
#define MSG_FLOW2 MSG_ALL[lang_selected][47]
#define MSG_CONTROL MSG_ALL[lang_selected][48]
#define MSG_MIN MSG_ALL[lang_selected][49]
#define MSG_MAX MSG_ALL[lang_selected][50]
#define MSG_FACTOR MSG_ALL[lang_selected][51]
#define MSG_AUTOTEMP MSG_ALL[lang_selected][52]
#define MSG_ON MSG_ALL[lang_selected][53]
#define MSG_OFF MSG_ALL[lang_selected][54]
#define MSG_PID_P MSG_ALL[lang_selected][55]
#define MSG_PID_I MSG_ALL[lang_selected][56]
#define MSG_PID_D MSG_ALL[lang_selected][57]
#define MSG_PID_C MSG_ALL[lang_selected][58]
#define MSG_ACC MSG_ALL[lang_selected][59]
#define MSG_VXY_JERK MSG_ALL[lang_selected][60]
#define MSG_VZ_JERK MSG_ALL[lang_selected][61]
#define MSG_VE_JERK MSG_ALL[lang_selected][62]
#define MSG_VMAX MSG_ALL[lang_selected][63]
#define MSG_X MSG_ALL[lang_selected][64]
#define MSG_Y MSG_ALL[lang_selected][65]
#define MSG_Z MSG_ALL[lang_selected][66]
#define MSG_E MSG_ALL[lang_selected][67]
#define MSG_VMIN MSG_ALL[lang_selected][68]
#define MSG_VTRAV_MIN MSG_ALL[lang_selected][69]
#define MSG_AMAX MSG_ALL[lang_selected][70]
#define MSG_A_RETRACT MSG_ALL[lang_selected][71]
#define MSG_XSTEPS MSG_ALL[lang_selected][72]
#define MSG_YSTEPS MSG_ALL[lang_selected][73]
#define MSG_ZSTEPS MSG_ALL[lang_selected][74]
#define MSG_ESTEPS MSG_ALL[lang_selected][75]
#define MSG_TEMPERATURE MSG_ALL[lang_selected][76]
#define MSG_MOTION MSG_ALL[lang_selected][77]
#define MSG_VOLUMETRIC MSG_ALL[lang_selected][78]
#define MSG_VOLUMETRIC_ENABLED MSG_ALL[lang_selected][79]
#define MSG_FILAMENT_SIZE_EXTRUDER_0 MSG_ALL[lang_selected][80]
#define MSG_FILAMENT_SIZE_EXTRUDER_1 MSG_ALL[lang_selected][81]
#define MSG_FILAMENT_SIZE_EXTRUDER_2 MSG_ALL[lang_selected][82]
#define MSG_CONTRAST MSG_ALL[lang_selected][83]
#define MSG_STORE_EPROM MSG_ALL[lang_selected][84]
#define MSG_LOAD_EPROM MSG_ALL[lang_selected][85]
#define MSG_RESTORE_FAILSAFE MSG_ALL[lang_selected][86]
#define MSG_REFRESH MSG_ALL[lang_selected][87]
#define MSG_WATCH MSG_ALL[lang_selected][88]
#define MSG_PREPARE MSG_ALL[lang_selected][89]
#define MSG_TUNE MSG_ALL[lang_selected][90]
#define MSG_PAUSE_PRINT MSG_ALL[lang_selected][91]
#define MSG_RESUME_PRINT MSG_ALL[lang_selected][92]
#define MSG_STOP_PRINT MSG_ALL[lang_selected][93]
#define MSG_CARD_MENU MSG_ALL[lang_selected][94]
#define MSG_NO_CARD MSG_ALL[lang_selected][95]
#define MSG_DWELL MSG_ALL[lang_selected][96]
#define MSG_USERWAIT MSG_ALL[lang_selected][97]
#define MSG_RESUMING MSG_ALL[lang_selected][98]
#define MSG_PRINT_ABORTED MSG_ALL[lang_selected][99]
#define MSG_NO_MOVE MSG_ALL[lang_selected][100]
#define MSG_KILLED MSG_ALL[lang_selected][101]
#define MSG_STOPPED MSG_ALL[lang_selected][102]
#define MSG_CONTROL_RETRACT MSG_ALL[lang_selected][103]
#define MSG_CONTROL_RETRACT_SWAP MSG_ALL[lang_selected][104]
#define MSG_CONTROL_RETRACTF MSG_ALL[lang_selected][105]
#define MSG_CONTROL_RETRACT_ZLIFT MSG_ALL[lang_selected][106]
#define MSG_CONTROL_RETRACT_RECOVER MSG_ALL[lang_selected][107]
#define MSG_CONTROL_RETRACT_RECOVER_SWAP MSG_ALL[lang_selected][108]
#define MSG_CONTROL_RETRACT_RECOVERF MSG_ALL[lang_selected][109]
#define MSG_AUTORETRACT MSG_ALL[lang_selected][110]
#define MSG_FILAMENTCHANGE MSG_ALL[lang_selected][111]
#define MSG_INIT_SDCARD MSG_ALL[lang_selected][112]
#define MSG_CNG_SDCARD MSG_ALL[lang_selected][113]
#define MSG_ZPROBE_OUT MSG_ALL[lang_selected][114]
#define MSG_POSITION_UNKNOWN MSG_ALL[lang_selected][115]
#define MSG_ZPROBE_ZOFFSET MSG_ALL[lang_selected][116]
#define MSG_BABYSTEP_X MSG_ALL[lang_selected][117]
#define MSG_BABYSTEP_Y MSG_ALL[lang_selected][118]
#define MSG_BABYSTEP_Z MSG_ALL[lang_selected][119]
#define MSG_ENDSTOP_ABORT MSG_ALL[lang_selected][120]
#define MSG_ADJUSTZ MSG_ALL[lang_selected][121]
#define MSG_HOMEYZ MSG_ALL[lang_selected][122]
#define MSG_SETTINGS MSG_ALL[lang_selected][123]
#define MSG_PREHEAT MSG_ALL[lang_selected][124]
#define MSG_UNLOAD_FILAMENT MSG_ALL[lang_selected][125]
#define MSG_LOAD_FILAMENT MSG_ALL[lang_selected][126]
#define MSG_RECTRACT MSG_ALL[lang_selected][127]
#define MSG_ERROR MSG_ALL[lang_selected][128]
#define MSG_PREHEAT_NOZZLE MSG_ALL[lang_selected][129]
#define MSG_SUPPORT MSG_ALL[lang_selected][130]
#define MSG_CORRECTLY MSG_ALL[lang_selected][131]
#define MSG_YES MSG_ALL[lang_selected][132]
#define MSG_NO MSG_ALL[lang_selected][133]
#define MSG_NOT_LOADED MSG_ALL[lang_selected][134]
#define MSG_NOT_COLOR MSG_ALL[lang_selected][135]
#define MSG_LOADING_FILAMENT MSG_ALL[lang_selected][136]
#define MSG_PLEASE_WAIT MSG_ALL[lang_selected][137]
#define MSG_LOADING_COLOR MSG_ALL[lang_selected][138]
#define MSG_CHANGE_SUCCESS MSG_ALL[lang_selected][139]
#define MSG_PRESS MSG_ALL[lang_selected][140]
#define MSG_INSERT_FILAMENT MSG_ALL[lang_selected][141]
#define MSG_CHANGING_FILAMENT MSG_ALL[lang_selected][142]
#define MSG_SILENT_MODE_ON MSG_ALL[lang_selected][143]
#define MSG_SILENT_MODE_OFF MSG_ALL[lang_selected][144]
#define MSG_REBOOT MSG_ALL[lang_selected][145]
#define MSG_TAKE_EFFECT MSG_ALL[lang_selected][146]
#define MSG_Enqueing MSG_ALL[lang_selected][147]
#define MSG_POWERUP MSG_ALL[lang_selected][148]
#define MSG_EXTERNAL_RESET MSG_ALL[lang_selected][149]
#define MSG_BROWNOUT_RESET MSG_ALL[lang_selected][150]
#define MSG_WATCHDOG_RESET MSG_ALL[lang_selected][151]
#define MSG_SOFTWARE_RESET MSG_ALL[lang_selected][152]
#define MSG_AUTHOR MSG_ALL[lang_selected][153]
#define MSG_CONFIGURATION_VER MSG_ALL[lang_selected][154]
#define MSG_FREE_MEMORY MSG_ALL[lang_selected][155]
#define MSG_PLANNER_BUFFER_BYTES MSG_ALL[lang_selected][156]
#define MSG_OK MSG_ALL[lang_selected][157]
#define MSG_FILE_SAVED MSG_ALL[lang_selected][158]
#define MSG_ERR_LINE_NO MSG_ALL[lang_selected][159]
#define MSG_ERR_CHECKSUM_MISMATCH MSG_ALL[lang_selected][160]
#define MSG_ERR_NO_CHECKSUM MSG_ALL[lang_selected][161]
#define MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM MSG_ALL[lang_selected][162]
#define MSG_FILE_PRINTED MSG_ALL[lang_selected][163]
#define MSG_BEGIN_FILE_LIST MSG_ALL[lang_selected][164]
#define MSG_END_FILE_LIST MSG_ALL[lang_selected][165]
#define MSG_M104_INVALID_EXTRUDER MSG_ALL[lang_selected][166]
#define MSG_M105_INVALID_EXTRUDER MSG_ALL[lang_selected][167]
#define MSG_M200_INVALID_EXTRUDER MSG_ALL[lang_selected][168]
#define MSG_M218_INVALID_EXTRUDER MSG_ALL[lang_selected][169]
#define MSG_M221_INVALID_EXTRUDER MSG_ALL[lang_selected][170]
#define MSG_ERR_NO_THERMISTORS MSG_ALL[lang_selected][171]
#define MSG_M109_INVALID_EXTRUDER MSG_ALL[lang_selected][172]
#define MSG_HEATING MSG_ALL[lang_selected][173]
#define MSG_HEATING_COMPLETE MSG_ALL[lang_selected][174]
#define MSG_BED_HEATING MSG_ALL[lang_selected][175]
#define MSG_BED_DONE MSG_ALL[lang_selected][176]
#define MSG_M115_REPORT MSG_ALL[lang_selected][177]
#define MSG_COUNT_X MSG_ALL[lang_selected][178]
#define MSG_ERR_KILLED MSG_ALL[lang_selected][179]
#define MSG_ERR_STOPPED MSG_ALL[lang_selected][180]
#define MSG_RESEND MSG_ALL[lang_selected][181]
#define MSG_UNKNOWN_COMMAND MSG_ALL[lang_selected][182]
#define MSG_ACTIVE_EXTRUDER MSG_ALL[lang_selected][183]
#define MSG_INVALID_EXTRUDER MSG_ALL[lang_selected][184]
#define MSG_X_MIN MSG_ALL[lang_selected][185]
#define MSG_X_MAX MSG_ALL[lang_selected][186]
#define MSG_Y_MIN MSG_ALL[lang_selected][187]
#define MSG_Y_MAX MSG_ALL[lang_selected][188]
#define MSG_Z_MIN MSG_ALL[lang_selected][189]
#define MSG_Z_MAX MSG_ALL[lang_selected][190]
#define MSG_M119_REPORT MSG_ALL[lang_selected][191]
#define MSG_ENDSTOP_HIT MSG_ALL[lang_selected][192]
#define MSG_ENDSTOP_OPEN MSG_ALL[lang_selected][193]
#define MSG_HOTEND_OFFSET MSG_ALL[lang_selected][194]
#define MSG_SD_CANT_OPEN_SUBDIR MSG_ALL[lang_selected][195]
#define MSG_SD_INIT_FAIL MSG_ALL[lang_selected][196]
#define MSG_SD_VOL_INIT_FAIL MSG_ALL[lang_selected][197]
#define MSG_SD_OPENROOT_FAIL MSG_ALL[lang_selected][198]
#define MSG_SD_CARD_OK MSG_ALL[lang_selected][199]
#define MSG_SD_WORKDIR_FAIL MSG_ALL[lang_selected][200]
#define MSG_SD_OPEN_FILE_FAIL MSG_ALL[lang_selected][201]
#define MSG_SD_FILE_OPENED MSG_ALL[lang_selected][202]
#define MSG_SD_SIZE MSG_ALL[lang_selected][203]
#define MSG_SD_FILE_SELECTED MSG_ALL[lang_selected][204]
#define MSG_SD_WRITE_TO_FILE MSG_ALL[lang_selected][205]
#define MSG_SD_PRINTING_BYTE MSG_ALL[lang_selected][206]
#define MSG_SD_NOT_PRINTING MSG_ALL[lang_selected][207]
#define MSG_SD_ERR_WRITE_TO_FILE MSG_ALL[lang_selected][208]
#define MSG_SD_CANT_ENTER_SUBDIR MSG_ALL[lang_selected][209]
#define MSG_STEPPER_TOO_HIGH MSG_ALL[lang_selected][210]
#define MSG_ENDSTOPS_HIT MSG_ALL[lang_selected][211]
#define MSG_ERR_COLD_EXTRUDE_STOP MSG_ALL[lang_selected][212]
#define MSG_ERR_LONG_EXTRUDE_STOP MSG_ALL[lang_selected][213]
#define MSG_BABYSTEPPING_X MSG_ALL[lang_selected][214]
#define MSG_BABYSTEPPING_Y MSG_ALL[lang_selected][215]
#define MSG_BABYSTEPPING_Z MSG_ALL[lang_selected][216]
#define MSG_SERIAL_ERROR_MENU_STRUCTURE MSG_ALL[lang_selected][217]
#define MSG_LANGUAGE_NAME MSG_ALL[lang_selected][218]
#define MSG_LANGUAGE_SELECT MSG_ALL[lang_selected][219]
#define MSG_PRUSA3D MSG_ALL[lang_selected][220]
#define MSG_PRUSA3D_FORUM MSG_ALL[lang_selected][221]
#define MSG_PRUSA3D_HOWTO MSG_ALL[lang_selected][222]
#define LANGUAGE_NAME 218 
#define LANGUAGE_SELECT 219 
#define LANG_NUM 5 
char* CAT2(const char *s1,const char *s2);
char* CAT4(const char *s1,const char *s2,const char *s3,const char *s4);



#endif //LANGUAGE_ALL.H