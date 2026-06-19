#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define WIFI_SSID "kirannn"
#define WIFI_PASSWORD "hi123456"
#define API_KEY "AIzaSyDyarIz3njiC3IUH3i-5cf3fIfwQ-a3-wM"// Firebase Credentials
#define DATABASE_URL "https://solarpumpcontroller-default-rtdb.firebaseio.com/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
#define PUMP_LED     2  //pin config
#define CHARGE_LED   4
#define ALARM_LED    5
#define BUZZER_LED   18

unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(PUMP_LED, OUTPUT);
  pinMode(CHARGE_LED, OUTPUT);
  pinMode(ALARM_LED, OUTPUT);
  pinMode(BUZZER_LED, OUTPUT);

  digitalWrite(PUMP_LED, LOW);
  digitalWrite(CHARGE_LED, LOW);
  digitalWrite(ALARM_LED, LOW);
  digitalWrite(BUZZER_LED, LOW);

  Serial.println("\nConnecting WiFi...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Firebase SignUp Success");
  }
  else
  {
    Serial.printf("Firebase SignUp Failed: %s\n",
                  config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Waiting for Firebase...");

  while (!Firebase.ready())
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nFirebase Ready");
}

void loop()
{
  
  if (millis() - previousMillis > 5000)   // Upload data every 5 seconds
  {
    previousMillis = millis();    

    float solarVoltage = random(180, 250) / 10.0;
    float batteryVoltage = random(110, 140) / 10.0;
    float pumpCurrent = random(0, 50) / 10.0;
    int tankLevel = random(0, 100);

    Firebase.RTDB.setFloat(&fbdo, "/data/solarVoltage", solarVoltage);
    Firebase.RTDB.setFloat(&fbdo, "/data/batteryVoltage", batteryVoltage);
    Firebase.RTDB.setFloat(&fbdo, "/data/pumpCurrent", pumpCurrent);
    Firebase.RTDB.setInt(&fbdo, "/data/tankLevel", tankLevel);

    Firebase.RTDB.setInt(&fbdo, "/data/pumpStatus",
                         digitalRead(PUMP_LED));

    Firebase.RTDB.setInt(&fbdo, "/data/chargingStatus",
                         digitalRead(CHARGE_LED));

    Firebase.RTDB.setInt(&fbdo, "/data/dryRunStatus", 0);

    Firebase.RTDB.setString(&fbdo,
                            "/data/systemStatus",
                            "Healthy");

    Serial.println("Data Uploaded");
  }

  
  if (Firebase.RTDB.getInt(&fbdo, "/commands/pumpCommand"))   // Pump Command
  {
    int value = fbdo.intData();

    digitalWrite(PUMP_LED, value);

    Serial.print("Pump Command = ");
    Serial.println(value);
  }

 
  if (Firebase.RTDB.getInt(&fbdo,
                           "/commands/chargingRelayCommand"))    // Charging Relay Command
  {
    int value = fbdo.intData();

    digitalWrite(CHARGE_LED, value);

    Serial.print("Charging Command = ");
    Serial.println(value);
  }

 
  if (Firebase.RTDB.getInt(&fbdo,
                           "/commands/alarmCommand"))    // Alarm Command
  {
    int value = fbdo.intData();

    digitalWrite(ALARM_LED, value);

    Serial.print("Alarm Command = ");
    Serial.println(value);
  }

  
  if (Firebase.RTDB.getInt(&fbdo,   
                           "/commands/buzzerCommand"))     // Buzzer Command
  {
    int value = fbdo.intData();

    digitalWrite(BUZZER_LED, value);

    Serial.print("Buzzer Command = ");
    Serial.println(value);
  }

  delay(500);
}