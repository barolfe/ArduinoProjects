/*
  config.h - config the keywords for Victron.DIRECT
*/

#define BMV_712
// MPPT 75 | 10
#ifdef BMV_712

  const byte buffsize = 32;
  const byte value_bytes = 33;
  const byte label_bytes = 9;
  const byte num_keywords = 14;

  char keywords[num_keywords][label_bytes] = {
  "PID",
  "V",
  "VS",
  "I",
  "P",
  "CE",
  "SOC",
  "TTG",
  "Alarm",
  "Relay",
  "AR",
  "BMW",
  "FW",
  "Checksum"
  };
  #define PID 0
  #define V 1
  #define VS 2  // Offically SER# but # does not play that well as macro
  #define I 3     // ScV
  #define P 4     // ScI
  #define CE 5   // PVV
  #define SOC 6   // PVI = PVV / VPV
  #define TTG 7    // ScS
  #define ALARM 8   // ScERR
  #define RELAY 9  // SLs
  #define AR 10   // SLI
  #define BMW 11
  #define FW 12
  #define CHECKSUM 13
#endif
