#include "FS.h"

void setup() {
  delay(1000);
  Serial.begin(19200);
  SPIFFS.begin();
  //SPIFFS.format();
  // put your setup code here, to run once:

  bool bWrite = false;
  char buff[100];
  size_t buffSize = 100;
  File f = SPIFFS.open("/data.txt","r");
  if (!f) {
    Serial.println("File open failed.");
  } else {
    Serial.println("File open successful."); 
    f.readBytes(buff, buffSize);
    Serial.println(buff);
  }

  Serial.println("Last character in buff is:");
  Serial.println(buff[100]);

  FSInfo fs_info;
  SPIFFS.info(fs_info);

  Serial.println("File data:");
  Serial.println(fs_info.totalBytes);
  Serial.println(fs_info.usedBytes);
  Serial.println(fs_info.blockSize);
  Serial.println(fs_info.pageSize);
  Serial.println(fs_info.maxOpenFiles);
  Serial.println(fs_info.maxPathLength);

  if (bWrite) {
    for (int i = 0; i < 1000; i++) {
      f.println(1234);
    }
  } else {
  }
  f.close();
}

void loop() {
 
  // put your main code here, to run repeatedly:

}
