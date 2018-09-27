#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

int pinRandom1 = A0;
int pinRandom2 = A5;


// Update these with values suitable for your network.
byte mac[] = {  0xDA, 0xED, 0xBA, 0xFE, 0xFE, 0xED };

IPAddress SERVER(192, 168, 0, 10); 
IPAddress ip(192, 168, 0, 200); // Shield address

// Callback for messagess MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
        // Once connected, publish an announcement...
        //client.publish("/test","hello world");
        // ... and resubscribe
        //client.subscribe("/test");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(pinRandom1, INPUT);
  pinMode(pinRandom2, INPUT);
  
  Serial.begin(9600);

  client.setServer(SERVER, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}


void loop()
{
  if (!client.connected()) {
    reconnect();    
  }else{

    char json[256];

    // JSON_ARRAY_SIZE(n)
    // n*JSON_OBJECT_SIZE(2)
    // SIENDO n = CANTIDAD DE OBJETOS DENTRO DEL ARRAY  
    const size_t bufferSize = JSON_ARRAY_SIZE(2) + 2*JSON_OBJECT_SIZE(2);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    
    JsonArray& root = jsonBuffer.createArray();
    
    JsonObject& root_0 = root.createNestedObject();
    root_0["id"] = 1;
    root_0["valor"] = analogRead(pinRandom1);
    
    JsonObject& root_1 = root.createNestedObject();
    root_1["id"] = 2;
    root_1["valor"] = analogRead(pinRandom2);

    root.printTo(json, sizeof(json));   
        
    client.publish("/test",json);
    Serial.println("Valor enviado:");
    Serial.println(json);
}
  delay(500);
  client.loop();
}
