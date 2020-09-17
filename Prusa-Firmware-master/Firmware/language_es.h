/**
 * Spanish
 *
 * LCD Menu Messages
 * Please note these are limited to 17 characters!
 *
 */
#ifndef LANGUAGE_ES_H
#define LANGUAGE_ES_H

#define WELCOME_MSG                         MACHINE_NAME " lista"
#define MSG_SD_INSERTED                     "Tarjeta colocada"
#define MSG_SD_REMOVED                      "Tarjeta retirada"
#define MSG_MAIN                            "Menu principal"
#define MSG_DISABLE_STEPPERS                "Apagar motores"
#define MSG_AUTO_HOME                       "Llevar al origen"
#define MSG_COOLDOWN                        "Enfriar"
#define MSG_MOVE_AXIS                       "Mover ejes"
#define MSG_MOVE_X                          "Mover X"
#define MSG_MOVE_Y                          "Mover Y"
#define MSG_MOVE_Z                          "Mover Z"
#define MSG_MOVE_E                          "Extrusor"
#define MSG_SPEED                           "Velocidad"
#define MSG_NOZZLE                          "Fusor"
#define MSG_BED                             "Base"
#define MSG_FAN_SPEED                       "Ventilador"
#define MSG_FLOW                            "Flujo"
#define MSG_TEMPERATURE                     "Temperatura"
#define MSG_WATCH                           "Monitorizar"
#define MSG_TUNE                            "Ajustar"
#define MSG_PAUSE_PRINT                     "Pausar impresion"
#define MSG_RESUME_PRINT                    "Reanudar impres."
#define MSG_STOP_PRINT                      "Detener impresion"
#define MSG_CARD_MENU                       "Menu de SD"
#define MSG_NO_CARD                         "No hay tarjeta SD"
#define MSG_DWELL                           "Reposo..."
#define MSG_USERWAIT                        "Esperando ordenes"
#define MSG_RESUMING                        "Resumiendo impre."
#define MSG_PRINT_ABORTED                   "Print aborted"
#define MSG_NO_MOVE                         "Sin movimiento"
#define MSG_KILLED                          "PARADA DE EMERG."
#define MSG_STOPPED                         "PARADA"
#define MSG_FILAMENTCHANGE                  "Cambiar filamento"
#define MSG_BABYSTEP_Z                      "Micropaso Z"
#define MSG_ADJUSTZ							"Auto Micropaso Z?"
#define MSG_HOMEYZ                          "Graduar Z"
#define MSG_SETTINGS                         "Ajuste"
#define MSG_PREHEAT                         "Precalentar"
#define MSG_UNLOAD_FILAMENT                 "Sacar filamento"
#define MSG_LOAD_FILAMENT                 "Introducir filamento"
#define MSG_ERROR                        "ERROR:"
#define MSG_PREHEAT_NOZZLE                       "Precal. extrusor!"
#define MSG_SUPPORT "Support"
#define MSG_CORRECTLY			"Cambiado correc.?"
#define MSG_YES					"Si"
#define MSG_NO					"No"
#define MSG_NOT_LOADED 			"Fil. no cargado"
#define MSG_NOT_COLOR 			"Color no claro"
#define MSG_LOADING_FILAMENT	"Cargando fil."
#define MSG_PLEASE_WAIT			"Espera"
#define MSG_LOADING_COLOR		"Cargando color"
#define MSG_CHANGE_SUCCESS		"Cambiar bien!"
#define MSG_PRESS				"Y pulse el mando"
#define MSG_INSERT_FILAMENT		"Inserta filamento"
#define MSG_CHANGING_FILAMENT	"Cambiando fil.!"
#define MSG_SILENT_MODE_ON					"Modo     [silencio]"
#define MSG_SILENT_MODE_OFF					"Modo [mas fuerza]" 
#define MSG_REBOOT							"Reiniciar la imp."
#define MSG_TAKE_EFFECT						"para tomar efecto"											
#define MSG_HEATING                         "Calentando..."
#define MSG_HEATING_COMPLETE                "Calentando listo."
#define MSG_BED_HEATING                     "Base Calentando"
#define MSG_BED_DONE                        "Base listo."
#define MSG_LANGUAGE_NAME					"Espanol"
#define MSG_LANGUAGE_SELECT					"Cambia la lengua "
#define MSG_PRUSA3D					"prusa3d.com"
#define MSG_PRUSA3D_FORUM					"forum.prusa3d.com"
#define MSG_PRUSA3D_HOWTO					"howto.prusa3d.com"


// Do not translate those!

#define MSG_Enqueing                        "enqueing \""
#define MSG_POWERUP                         "PowerUp"
#define MSG_EXTERNAL_RESET                  " External Reset"
#define MSG_BROWNOUT_RESET                  " Brown out Reset"
#define MSG_WATCHDOG_RESET                  " Watchdog Reset"
#define MSG_SOFTWARE_RESET                  " Software Reset"
#define MSG_AUTHOR                          " | Author: "
#define MSG_CONFIGURATION_VER               " Last Updated: "
#define MSG_FREE_MEMORY                     " Free Memory: "
#define MSG_PLANNER_BUFFER_BYTES            "  PlannerBufferBytes: "
#define MSG_OK                              "ok"
#define MSG_FILE_SAVED                      "Done saving file."
#define MSG_ERR_LINE_NO                     "Line Number is not Last Line Number+1, Last Line: "
#define MSG_ERR_CHECKSUM_MISMATCH           "checksum mismatch, Last Line: "
#define MSG_ERR_NO_CHECKSUM                 "No Checksum with line number, Last Line: "
#define MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM "No Line Number with checksum, Last Line: "
#define MSG_FILE_PRINTED                    "Done printing file"
#define MSG_BEGIN_FILE_LIST                 "Begin file list"
#define MSG_END_FILE_LIST                   "End file list"
#define MSG_M104_INVALID_EXTRUDER           "M104 Invalid extruder "
#define MSG_M105_INVALID_EXTRUDER           "M105 Invalid extruder "
#define MSG_M200_INVALID_EXTRUDER           "M200 Invalid extruder "
#define MSG_M218_INVALID_EXTRUDER           "M218 Invalid extruder "
#define MSG_M221_INVALID_EXTRUDER           "M221 Invalid extruder "
#define MSG_ERR_NO_THERMISTORS              "No thermistors - no temperature"
#define MSG_M109_INVALID_EXTRUDER           "M109 Invalid extruder "
#define MSG_M115_REPORT                     "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:" FIRMWARE_URL " PROTOCOL_VERSION:" PROTOCOL_VERSION " MACHINE_TYPE:" MACHINE_NAME " EXTRUDER_COUNT:" STRINGIFY(EXTRUDERS) " UUID:" MACHINE_UUID "\n"
#define MSG_COUNT_X                         " Count X: "
#define MSG_ERR_KILLED                      "Printer halted. kill() called!"
#define MSG_ERR_STOPPED                     "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)"
#define MSG_RESEND                          "Resend: "
#define MSG_UNKNOWN_COMMAND                 "Unknown command: \""
#define MSG_ACTIVE_EXTRUDER                 "Active Extruder: "
#define MSG_INVALID_EXTRUDER                "Invalid extruder"
#define MSG_X_MIN                           "x_min: "
#define MSG_X_MAX                           "x_max: "
#define MSG_Y_MIN                           "y_min: "
#define MSG_Y_MAX                           "y_max: "
#define MSG_Z_MIN                           "z_min: "
#define MSG_Z_MAX                           "z_max: "
#define MSG_M119_REPORT                     "Reporting endstop status"
#define MSG_ENDSTOP_HIT                     "TRIGGERED"
#define MSG_ENDSTOP_OPEN                    "open"
#define MSG_HOTEND_OFFSET                   "Hotend offsets:"
#define MSG_SD_CANT_OPEN_SUBDIR             "Cannot open subdir"
#define MSG_SD_INIT_FAIL                    "SD init fail"
#define MSG_SD_VOL_INIT_FAIL                "volume.init failed"
#define MSG_SD_OPENROOT_FAIL                "openRoot failed"
#define MSG_SD_CARD_OK                      "SD card ok"
#define MSG_SD_WORKDIR_FAIL                 "workDir open failed"
#define MSG_SD_OPEN_FILE_FAIL               "open failed, File: "
#define MSG_SD_FILE_OPENED                  "File opened: "
#define MSG_SD_SIZE                         " Size: "
#define MSG_SD_FILE_SELECTED                "File selected"
#define MSG_SD_WRITE_TO_FILE                "Writing to file: "
#define MSG_SD_PRINTING_BYTE                "SD printing byte "
#define MSG_SD_NOT_PRINTING                 "Not SD printing"
#define MSG_SD_ERR_WRITE_TO_FILE            "error writing to file"
#define MSG_SD_CANT_ENTER_SUBDIR            "Cannot enter subdir: "
#define MSG_STEPPER_TOO_HIGH                "Steprate too high: "
#define MSG_ENDSTOPS_HIT                    "endstops hit: "
#define MSG_ERR_COLD_EXTRUDE_STOP           " cold extrusion prevented"
#define MSG_ERR_LONG_EXTRUDE_STOP           " too long extrusion prevented"
#define MSG_BABYSTEPPING_X                  "Babystepping X"
#define MSG_BABYSTEPPING_Y                  "Babystepping Y"
#define MSG_BABYSTEPPING_Z                  "Adjusting Z"
#define MSG_SERIAL_ERROR_MENU_STRUCTURE     "Error in menu structure"
#define MSG_SET_HOME_OFFSETS                "Set home offsets"
#define MSG_SET_ORIGIN                      "Set origin"
#define MSG_PREHEAT_PLA                     "Preheat PLA"
#define MSG_PREHEAT_PLA0                    "Preheat PLA 1"
#define MSG_PREHEAT_PLA1                    "Preheat PLA 2"
#define MSG_PREHEAT_PLA2                    "Preheat PLA 3"
#define MSG_PREHEAT_PLA012                  "Preheat PLA All"
#define MSG_PREHEAT_PLA_BEDONLY             "Preheat PLA Bed"
#define MSG_PREHEAT_PLA_SETTINGS            "Preheat PLA conf"
#define MSG_PREHEAT_ABS                     "Preheat ABS"
#define MSG_PREHEAT_ABS0                    "Preheat ABS 1"
#define MSG_PREHEAT_ABS1                    "Preheat ABS 2"
#define MSG_PREHEAT_ABS2                    "Preheat ABS 3"
#define MSG_PREHEAT_ABS012                  "Preheat ABS All"
#define MSG_PREHEAT_ABS_BEDONLY             "Preheat ABS Bed"
#define MSG_PREHEAT_ABS_SETTINGS            "Preheat ABS conf"
#define MSG_SWITCH_PS_ON                    "Switch power on"
#define MSG_SWITCH_PS_OFF                   "Switch power off"
#define MSG_AUTOTEMP                        "Autotemp"
#define MSG_ON                              "On "
#define MSG_OFF                             "Off"
#define MSG_PID_P                           "PID-P"
#define MSG_PID_I                           "PID-I"
#define MSG_PID_D                           "PID-D"
#define MSG_PID_C                           "PID-C"
#define MSG_ACC                             "Accel"
#define MSG_VXY_JERK                        "Vxy-jerk"
#define MSG_VZ_JERK                         "Vz-jerk"
#define MSG_VE_JERK                         "Ve-jerk"
#define MSG_VMAX                            "Vmax "
#define MSG_X                               "x"
#define MSG_Y                               "y"
#define MSG_Z                               "z"
#define MSG_E                               "e"
#define MSG_VMIN                            "Vmin"
#define MSG_VTRAV_MIN                       "VTrav min"
#define MSG_AMAX                            "Amax "
#define MSG_A_RETRACT                       "A-retract"
#define MSG_XSTEPS                          "Xsteps/mm"
#define MSG_YSTEPS                          "Ysteps/mm"
#define MSG_ZSTEPS                          "Zsteps/mm"
#define MSG_ESTEPS                          "Esteps/mm"
#define MSG_RETRACT                         "Retract"
#define MSG_EXTRUDE                         "Extrude"
#define MSG_AUTOSTART                       "Autostart"
#define MSG_MOVE_E1                         "Extruder2"
#define MSG_MOVE_E2                         "Extruder3"
#define MSG_MOVE_01MM                       "Move 0.1mm"
#define MSG_MOVE_1MM                        "Move 1mm"
#define MSG_MOVE_10MM                       "Move 10mm"
#define MSG_NOZZLE1                         "Nozzle2"
#define MSG_NOZZLE2                         "Nozzle3"
#define MSG_FLOW0                           "Flow 0"
#define MSG_FLOW1                           "Flow 1"
#define MSG_FLOW2                           "Flow 2"
#define MSG_CONTROL                         "Control"
#define MSG_MIN                             " \002 Min"
#define MSG_MAX                             " \002 Max"
#define MSG_FACTOR                          " \002 Fact"
#define MSG_MOTION                          "Motion"
#define MSG_VOLUMETRIC                      "Filament"
#define MSG_VOLUMETRIC_ENABLED		        "E in mm3"
#define MSG_FILAMENT_SIZE_EXTRUDER_0        "Fil. Dia. 1"
#define MSG_FILAMENT_SIZE_EXTRUDER_1        "Fil. Dia. 2"
#define MSG_FILAMENT_SIZE_EXTRUDER_2        "Fil. Dia. 3"
#define MSG_CONTRAST                        "LCD contrast"
#define MSG_STORE_EPROM                     "Store memory"
#define MSG_LOAD_EPROM                      "Load memory"
#define MSG_RESTORE_FAILSAFE                "Restore failsafe"
#define MSG_REFRESH                         "Refresh"
#define MSG_PREPARE                         "Prepare"
#define MSG_CONTROL_RETRACT                 "Retract mm"
#define MSG_CONTROL_RETRACT_SWAP            "Swap Re.mm"
#define MSG_CONTROL_RETRACTF                "Retract  V"
#define MSG_CONTROL_RETRACT_ZLIFT           "Hop mm"
#define MSG_CONTROL_RETRACT_RECOVER         "UnRet +mm"
#define MSG_CONTROL_RETRACT_RECOVER_SWAP    "S UnRet+mm"
#define MSG_CONTROL_RETRACT_RECOVERF        "UnRet  V"
#define MSG_AUTORETRACT                     "AutoRetr."
#define MSG_INIT_SDCARD                     "Init. SD card"
#define MSG_CNG_SDCARD                      "Change SD card"
#define MSG_ZPROBE_OUT                      "Z probe out. bed"
#define MSG_POSITION_UNKNOWN                "Home X/Y before Z"
#define MSG_ZPROBE_ZOFFSET                  "Z Offset"
#define MSG_BABYSTEP_X                      "Babystep X"
#define MSG_BABYSTEP_Y                      "Babystep Y"
#define MSG_ENDSTOP_ABORT                   "Endstop abort"
#define MSG_RECTRACT                        "Rectract"

#endif // LANGUAGE_EN_H
