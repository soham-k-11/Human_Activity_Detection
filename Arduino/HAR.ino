#include<LSM6DS3.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

#define MAX_SIZE 60
#define MAX_SAMPLE_SIZE 300

char ssid[] = "Shripad Shrivallabh";
char pass[] = "Swamisamartha@102";

IPAddress serverIP(192,168,29,216); 
int port = 5000; 
const char* pred_route = "/predict"; 
const char* Init_route = "/Init";

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverIP, port);

void setup()
{
  Serial.begin(9600);
  while(!Serial);

  if (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("WiFi failed");
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status()); 
    while (true);
  }
  Serial.println("Wifi Connected");

  while (!IMU.begin()) 
  {
  Serial.println("IMU not detected. Retrying...");
  delay(1000);
  }
  Serial.println("IMU Started");

  client.beginRequest();
  client.get(Init_route);
  client.endRequest();
  delay(10);
}

void loop()
{
  for(int chunk=0;chunk<(MAX_SAMPLE_SIZE/MAX_SIZE);chunk++)
  {
    int i=0;
    String payload="[";
    Serial.println("Collecting values...");
    while(i<MAX_SIZE)
    {
      payload += "[";
      float x,y,z;
      IMU.readAcceleration(x, y, z);
      payload = payload + String(x) + "," + String(y) + "," +String(z) + ",";
      IMU.readGyroscope(x, y, z);
      payload = payload + String(x) + "," + String(y) + "," +String(z);
      payload += "]";
      if (i < MAX_SIZE-1) payload += ",";
      i++;
    }    
    payload += "]";

    client.beginRequest();
    client.post(route);
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", payload.length());
    client.beginBody();
    client.print(payload);
    client.endRequest();
    delay(10);
  } 
  String response = client.responseBody();
  Serial.print("Output: ");
  Serial.println(response);
  delay(200); 
}
