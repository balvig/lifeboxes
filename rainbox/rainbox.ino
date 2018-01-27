// Libs
#include "RBD_Timer.h"
#include "WEMOS_Matrix_GFX.h"
#include "Net.h"
#include "Api.h"
#include "Sky.h"

// Configuration
const int UPDATE_INTERVAL = 1800000; // 30 minutes
const int REALTIME_LOOP_SLEEP = 25;
const size_t JSON_BUFFER = JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(3) + 80; // http://arduinojson.org/assistant/
const String API_ENDPOINT = "http://lifeboxes.herokuapp.com/weather";
//const String API_ENDPOINT = "http://b9bf9a06.ngrok.io/weather";


// Variables
MLED matrix(5);
RBD::Timer updateTimer;
Lifeboxes::Net net;
Lifeboxes::Api api(API_ENDPOINT, JSON_BUFFER);
Lifeboxes::Sky skies[] = { 1, 3, 5 };

// Main
void setup() {
  Serial.begin(115200);
  updateTimer.setTimeout(UPDATE_INTERVAL);
  updateTimer.restart();
  if(net.connect()) {
    updateState();
  }
}

void loop() {
  if (updateTimer.onRestart()) {
    updateState();
  //  conserveBattery();
  }
  animate();
}

void animate() {
  matrix.clear();

  for (auto &sky : skies) {
    for (auto &raindrop : sky.raindrops) {
      raindrop.update();
      matrix.drawPixel(raindrop.x, raindrop.y, raindrop.active);
    }
  }

  matrix.writeDisplay();
}

// Private
void updateState() {
//  if (net.connect()) {
    syncWithApi();
  //  net.disconnect();
 // }
//  else {
    //lcdMessage("Wifi connection error");
  //}
}

void syncWithApi() {
  JsonObject& json = api.fetchJson();

  syncSkies(json["led_values"]);
}

void syncSkies(JsonArray& data) {
  int dataSize = data.size();
  
  for (int i = 0; i < dataSize; i++) {
    int state = data[i];
    skies[i].setState(state);
  }
}


void conserveBattery() {
  if (needRealtimeUpdates()) {
    return;
  }
  else {
    delay(REALTIME_LOOP_SLEEP);
  }
}
    
boolean needRealtimeUpdates() {
  return true;
}
