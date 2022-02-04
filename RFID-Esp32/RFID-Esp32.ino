#include <SPI.h>
#include <MFRC522.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
/* change ssid and password according to yours WiFi*/
const char* ssid     = "Your WIFI";
const char* password = "Your PASSWORD";
/*
 * This is the IP address of your PC
 * [Wins: use ipconfig command, Linux: use ifconfig command]
*/

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Create an Event Source on /events
AsyncEventSource events("/events");

const char* host = "192.168.0.236";
const int port = 8052;

#define SS_PIN 5
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

String content = " Blank";

void getRFIDReadings(){
  content;
}

String processor(const String& var){
  getRFIDReadings();
  //Serial.println(var);
  if(var == "CONTENT"){
    return String(content);
  }
}
//Create A webpage to view rfid card number
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>RFID Web Sever</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-gamepad"></i> CardNumber</p><p><span class="reading"><span id="rfid">%CONTENT%</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('content', function(e) {
  console.log("content", e.data);
  document.getElementById("rfid").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

void setup()
{
    Serial.begin(115200);
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
    Serial.print("Connecting to ");
    Serial.println(ssid);
    /* connect to your WiFi */
    WiFi.begin(ssid, password);
    /* wait until ESP32 connect to WiFi*/
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected with IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
    });

    events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
    });
    server.addHandler(&events);
    server.begin();
}
void loop()
{
    ws.cleanupClients();
    String currentUID = "";
        /* Use WiFiClient class to create TCP connections */
    WiFiClient client;
    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        return;
    }
    
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
        return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
        return;
    }

    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
        currentUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        currentUID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    currentUID.toUpperCase();
    currentUID.remove(0,1);
    currentUID.remove(2,1);
    currentUID.remove(4,1);
    currentUID.remove(6,1);
    Serial.println("UID: " + currentUID);
    client.print(currentUID);
    events.send("ping",NULL,millis());
    events.send(String(currentUID).c_str(),"content",millis());
    client.stop();
    delay(3000);
    
    /* This will send the data to the server */
}
