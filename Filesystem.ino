#include "Scheduler.h"
#include "Sensor.h"
#include "JSONFile.h"
#include "Metadata.h"
#include "Network.h"

void newConnectionCallback( uint32_t nodeId );


#define P Serial.print
#define Pln Serial.println
#define SEC 1000000

RTCScheduler scheduler;
JSONFile datafile;


void setup() {
  uint32_t seqNo;
  SensorArray sensors;
  FakeRandomSensor fake;
  Metadata metadata;

  
  Serial.begin(115200);

  //SPIFFS.begin();
  //SPIFFS.format();
  //test_loggr();
  //test_meta();  

  Pln("\n\n\n");

  network.setup();
  datafile.open("/data.json");
  return;

  scheduler.shouldSense();
  scheduler.shouldTX();

  metadata.open("metadata.bin");
  seqNo = metadata.incrementSeqNo();
  metadata.close();
  
  sensors.push("fake", fake.read());
  sensors.push("fake2", fake.read());

  String json = sensors.toJSON(seqNo, 0);
  Pln("JSON");
  P(json);

  JSONFile file;

  file.open("/data.json");
  file.append(json);

  while (file.hasNext()) {
    P("line: ");
    Pln(file.next());
  }
}

void loop() {
  network.update();

  if (!network.knowsServer()) {
    delay(10);
    return;
  }

  Pln("ok");

  while (datafile.hasNext()) {
    P("line: ");
    Pln(datafile.next());
  }

  delay(10000);

  if (datafile.hasNext()) {
    String data = datafile.next();

    P("send: "); Pln(data);
    network.sendData(data);
  }
  
  delay(100);
}



void newConnectionCallback( uint32_t nodeId ) {
  P("new connection"); Pln(nodeId);
  network.hello();
}


/**
void test_meta() {
  meta.open("/meta.bin");
  Serial.print("seq_no: "); Serial.println(meta.getSeqNo());
  Serial.print("server: "); Serial.println(meta.getServer());
  meta.incrementSeqNo();
  Serial.print("new seq_no: "); Serial.println(meta.getSeqNo());
  meta.close();
}

void test_loggr() {
  loggr.open("/logs.bin");
  loggr.setTemperature(80);
  loggr.setHumidity(70);
  loggr.append();
  loggr.close();

  loggr.open("/logs.bin");

  byte i = 0;
  
  while (loggr.next()) {
    Serial.print("######### "); Serial.print(i++); Serial.println(" #######");
    P("temp: "); P(loggr.getTemperature());
    P("\nhum: "); P(loggr.getHumidity());
    Pln("");
  }
}
*/

