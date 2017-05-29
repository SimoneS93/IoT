
bool hello = false;
RTCScheduler scheduler;
WifiMeshNetwork network;
Metadata metadata;
Loggr loggr;
SensorArray sensors(loggr);


void setup() {
    if (scheduler.shouldSense()) {
        SensorArray sensors;
        Sensor temp = DHT11Sensor(10, 11);
        sensors.read("dht11", temp);
        loggr.append(sensors.toJSON(metadata.incrementSeqNo(), scheduler.now()));
        //sensors.sense(metadata.incrementSeqNo(), scheduler.now());
    }

    if (!scheduler.shouldTX()) {

        ESP.deepSleep(10 * SEC);
    }

    metadata.open("/meta.bin");
    loggr.open("/logs.bin");
    network.onAck(&onAck);
}

void loop() {
    // se siamo qui, vuol dire che dobbiamo trasmettere le rilevazioni
    network.update();
    network.hello();

    // aspetta di ricevere l'ack dal server, col suo indirizzo
    if (!network.knowsServer()) {
        delay(50);
        return;
    }

    // invia messaggio al server o segnala terminazione
    if (loggr.hasNext()) {
        network.sendToServer(loggr.nextSerialized());
    }
    else {
        network.bye();
    }

    // se tutti hanno finito di trasmettere, dormi
    if (network.isEmpty()) {
        loggr.close();
        ESP.deepSleep(10 * SEC);
    }

    delay(50);
}

/**
 * Alla ricezione di un ack, cancella definitivamente
 * la rilevazione corrispondente
 */
void onAck(uint32_t seqNo) {
    loggr.delete(seqNo);
}