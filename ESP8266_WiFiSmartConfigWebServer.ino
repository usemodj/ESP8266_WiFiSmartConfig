#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server ( 80 );

void handleRoot();
void handleNotFound();
void drawGraph();

void setup() {
  
  Serial.begin(115200);
  delay(10);
  
  WiFi.mode(WIFI_STA);
  delay(500);

  WiFi.beginSmartConfig();
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(WiFi.smartConfigDone()){
      Serial.println("WiFi Smart Config Done.");
    }
  }
  
  Serial.println("");
  Serial.println("");
  
  WiFi.printDiag(Serial);
  
  // Start the server
  server.on ( "/", handleRoot );
  server.on ( "/test.svg", drawGraph );
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
 
  server.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.println ( "HTTP server started" );
  //Serial.printf("Ready! Open http://%s/ in your browser\n", getLocalIPString().c_str());


  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  server.handleClient();
  yield();
}

void handleRoot() {
  char temp[500];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char *html = R"##(<html>
  <head>
    <meta charset='utf-8'/>
    <meta http-equiv='refresh' content='5'/>
    <title>ESP8266 Demo</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }
    </style>
  </head>
  <body>
    <h1>Hello from ESP8266!</h1>
    <p><a href="/inline"> Inline </a></p>
    <p>Uptime: %02d:%02d:%02d</p>
    <img src="/test.svg"/>
  </body>
  </html>
  )##";
  
  snprintf ( temp, 500, html, 
    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}


