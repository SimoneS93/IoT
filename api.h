
#define ONCE(label) static bool __once_##label = false; if (!__once_##label && (__once_##label = true))



bool hello = false;
bool shouldSenseData = true;
RTCScheduler scheduler;
WifiMeshNetwork network;
Metadata metadata("/metadata.bin");
DirLogger logger("/data");
SensorArray sensors;
JSONSerializer serializer;


void setup() {
    if (scheduler.shouldSense()) {
        metadata.open();
        logger.open();
        
        Sensor dht = DHT11Sensor(10, 11);
        sensors.append("dht11", temp.read());
        serializer.append("dht11_temp", dht.readTemperature());
        serializer.append("dht11_hum", dht.readHumidity());
        serializer.mark(metadata.incrementSeqNo(), scheduler.now());
        
        logger.append(serializer.toString());
        logger.close();
        metadata.close();
    }

    if (!scheduler.shouldTX()) {
        ESP.deepSleep(10 * SEC);
    }

    logger.open();
    network.onDataAck(&onAck);
    network.onDataTimeout(&onDataTimeout);
}


void loop() {
    // se siamo qui, vuol dire che dobbiamo trasmettere le rilevazioni
    network.update();

    // invia HELLO solo una volta
    // non lo mettiamo nel setup() per essere sicuri che la rete stia "girando"
    ONCE(HELLO) {
      network.hello();
    }

    // in tutti i casi, non restare attivo per più di 2 minuti
    AFTER(SLEEP, 120 * 1000) {
      ESP.deepSleep(60 * SEC);
    }

    // aspetta di ricevere l'ack dal server, col suo indirizzo
    if (!network.knowsServer()) {
        delay(50);
        return;
    }

    // invia messaggio al server o segnala terminazione
    if (!shouldSendData) {
      return;
    }
    
    if (logger.hasNext()) {
        network.sendToServer(logger.next());
        shouldSendData = false;
    }
    else {
        loggr.close();
        network.bye();

        // se tutti hanno finito di trasmettere, dormi
        if (network.isEmpty()) {
            ESP.deepSleep(10 * SEC);
        }
    }

    delay(50);
}

/**
 * Alla ricezione di un ack, cancella definitivamente
 * la rilevazione corrispondente
 */
void onDataAck(uint32_t seqNo) {
    logger.pop();
    shouldSendData = true;
}

void onDataTimeout() {
  shouldSendData = true;
}

