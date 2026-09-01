#include <DHT.h>

#define SOIL1 A0
#define SOIL2 A1
#define RAIN1 A2
#define DHTPIN 2
#define DHTTYPE DHT11

#define AIN1 5
#define AIN2 6
#define AIN3 7
#define AIN4 8
#define ENA 11
#define ENB 12

DHT dht(DHTPIN, DHTTYPE);

void setup() {
 Serial.begin(9600);
 dht.begin();

 pinMode(AIN1, OUTPUT);
 pinMode(AIN2, OUTPUT);
 pinMode(AIN3, OUTPUT);
 pinMode(AIN4, OUTPUT);
 pinMode(ENA, OUTPUT);
 pinMode(ENB, OUTPUT);
 
 analogWrite(ENA, 255);
 analogWrite(ENB, 255);
}

void loop() {
 float tempAvg, humAvg;
 calculateTempHumAverage(tempAvg, humAvg);

 int rainPrediction = calculateRainAverage();
 int rain = analogRead(RAIN1);
 int rainLevel = getRainLevel(rain);

 int combinedPrediction = matchPredictions(tempAvg, humAvg, rainPrediction);

 int soil1 = analogRead(SOIL1);
 int soil2 = analogRead(SOIL2);
 int soil1Percent = map(soil1, 500, 215, 0, 100);
 int soil2Percent = map(soil2, 500, 215, 0, 100);

 int soilLevel1 = getSoilLevel(soil1Percent);
 int soilLevel2 = getSoilLevel(soil2Percent);

 int tempLevel = getTemperatureLevel(tempAvg);
 int humLevel = getHumidityLevel(humAvg);

 int predictionLevel = getPredictionLevel(tempAvg, humAvg);
 String predictionStatus = getPredictionStatus(predictionLevel);

 String dataToSend = "{";
 dataToSend += "\"soil1\":" + String(soil1Percent) + ",";
 dataToSend += "\"soil2\":" + String(soil2Percent) + ",";
 dataToSend += "\"temp\":" + String(tempAvg, 2) + ",";
 dataToSend += "\"hum\":" + String(humAvg, 2) + ",";
 dataToSend += "\"rain\":\"";
 dataToSend += (combinedPrediction == 1 ? "Tidak Berpotensi Hujan" : "Berpotensi Hujan");
 dataToSend += "\"}";
 dataToSend += "}";

 Serial.println(dataToSend);
  
 String message = "Soil Moisture 1: " + String(soil1Percent) + "% - Status: " + getSoilStatus(soilLevel1) +
          "\nSoil Moisture 2: " + String(soil2Percent) + "% - Status: " + getSoilStatus(soilLevel2) +
          "\nDHT - Humidity Average: " + String(humAvg) + "%" +
          "\nDHT - Temp Average: " + String(tempAvg) + "°C" +
          "\nPrediction: " + String(predictionLevel) + " " + "Valid: " + String(rainPrediction) +
          "\nFinal Prediction: " + String(combinedPrediction == 1 ? "Tidak Berpotensi Hujan" : "Berpotensi Hujan") + "\n";

 Serial.println(message);

 controlPumpWithLogic(soilLevel1, soilLevel2, combinedPrediction);

 delay(21600000);
}

void calculateTempHumAverage(float &tempAvg, float &humAvg) {
 long tempSum = 0;
 long humSum = 0;
 const int duration = 900;

 Serial.println("Calculating Prediction...\n");

 for (int i = 0; i < duration; i++) {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  tempSum += temp;
  humSum += hum;

  delay(1000);
 }

 tempAvg = tempSum / duration;
 humAvg = humSum / duration;
}

int calculateRainAverage() {
 long sum = 0;
 const int duration = 900; 
 const float tolerance = 85.0; 
 int rainValues[duration]; 

 for (int i = 0; i < duration; i++) {
  int rainValue = analogRead(RAIN1);
  rainValues[i] = rainValue;
  sum += rainValue;

  delay(1000); 
 }

 int average = sum / duration; 

 bool isStable = evaluateStableRain(rainValues, duration, average);
 bool isRising = evaluateTrend(rainValues, duration, tolerance);

 if (isStable || isRising) {
  return 1; 
 } else {
  return 0; 
 }
}

bool evaluateStableRain(int rainValues[], int duration, int average) {
 int stableCount = 0;
 for (int i = 0; i < duration; i++) {
  if (abs(rainValues[i] - average) <= (0.15 * average)) {
   stableCount++;
  }
 }
 float stablePercentage = (float)stableCount / duration * 100.0;
 if (stablePercentage >= 85.0 && average < 700) {
  return false;
 }
 if (stablePercentage >= 85.0 && average > 700) {
  return true;
 }
}

bool evaluateTrend(int rainValues[], int duration, float tolerance) {
 int riseCount = 0;
 int decreaseCount = 0;

 for (int i = 1; i < duration; i++) {
  if (rainValues[i] > rainValues[i - 1]) {
   riseCount++; 
  } else if (rainValues[i] < rainValues[i - 1]) {
   decreaseCount++; 
  }
 }

 float risePercentage = (float)riseCount / (duration - 1) * 100.0;
 float decreasePercentage = (float)decreaseCount / (duration - 1) * 100.0;

 if (risePercentage >= tolerance) {
  return true; 
 } else if (decreasePercentage >= tolerance) {
  return false;
 }
}

int matchPredictions(float tempAvg, float humAvg, int rainPrediction) {
 int tempLevel = getTemperatureLevel(tempAvg);
 int humLevel = getHumidityLevel(humAvg);
 int predictionLevel = getPredictionLevel(tempAvg, humAvg);

 if (predictionLevel == 0 && rainPrediction == 1) {
  return 1;
 } 
 if (predictionLevel == 0 && rainPrediction == 0) {
  return 0;
 }
 if (predictionLevel == 1 && rainPrediction == 0) {
  return 1;
 }
 if (predictionLevel == 0 && rainPrediction == 1) {
  return 0;
 }
}

int getRainLevel(int rainValue) {
 if (rainValue > 600) return 1;
 if (rainValue > 300) return 2;
 return 3;
}

String getRainStatus(int rainLevel) {
 if (rainLevel == 1) return "Tidak Hujan";
 if (rainLevel == 2) return "Gerimis";
 return "Hujan";
}

int getSoilLevel(int soilPercent) {
 if (soilPercent < 41) return 1;
 if (soilPercent < 61) return 2;
 return 3;
}

String getSoilStatus(int soilLevel) {
 if (soilLevel == 1) return "Kering";
 if (soilLevel == 2) return "Normal";
 return "Basah";
}

int getHumidityLevel(float humAvg) {
 if (humAvg < 31) return 1;
 if (humAvg < 51) return 2;
 return 3;
}

int getTemperatureLevel(float tempAvg) {
 if (tempAvg < 31) return 1;
 if (tempAvg < 41) return 2;
 return 3;
}

int getPredictionLevel(int tempAvg, int humAvg) {
  if (tempAvg < 31 && humAvg < 31) return 1;  
  if (tempAvg < 31 && (humAvg > 30 && humAvg < 51)) return 1;
  if ((tempAvg > 30 && tempAvg < 41) && humAvg < 31) return 1; 
  if ((tempAvg > 30 && tempAvg < 41) && (humAvg > 30 && humAvg < 51)) return 1;
  if (tempAvg > 40 && humAvg < 31) return 1;         
  if (tempAvg > 40 && (humAvg > 30 && humAvg < 51)) return 1;  
  if (tempAvg > 40 && humAvg > 50) return 1;         
  return 0;
}

String getPredictionStatus(int predictionLevel) {
 if (predictionLevel == 1) return "Tidak akan hujan";
 return "Akan hujan";
}

void controlPumpWithLogic(int soilLevel1, int soilLevel2, int combinedPrediction) {
  if (soilLevel1 == 1 && soilLevel2 == 1 && combinedPrediction == 1) {
    turnOnPump1();
    turnOnPump2();
    delay(30000);
    turnOffPump();
  }
  if (soilLevel1 == 1 && soilLevel2 == 2 && combinedPrediction == 1) {
    turnOnPump1();
    turnOnPump2();
    delay(30000);
    turnOffPump();
  }
  if (soilLevel1 == 1 && soilLevel2 == 3 && combinedPrediction == 1) {
    turnOnPump1();
    delay(30000);
    turnOffPump();
  }
  if (soilLevel1 == 2 && soilLevel2 == 1 && combinedPrediction == 1) {
    turnOnPump1();
    turnOnPump2();
    delay(30000);
    turnOffPump();
  }
  if (soilLevel1 == 2 && soilLevel2 == 2 && combinedPrediction == 1) {
    turnOnPump1();
    turnOnPump2();
    delay(30000);
    turnOffPump();
  }
  if (soilLevel1 == 3 && soilLevel2 == 1 && combinedPrediction == 1) {
    turnOnPump2();
    delay(30000);
    turnOffPump();
  }
  else {
   turnOffPump();
  }
}

void turnOnPump1() {
 digitalWrite(AIN1, HIGH);
 digitalWrite(AIN2, LOW);
}

void turnOnPump2() {
 digitalWrite(AIN3, HIGH);
 digitalWrite(AIN4, LOW);
}

void turnOffPump() {
 digitalWrite(AIN1, LOW);
 digitalWrite(AIN2, LOW);
 digitalWrite(AIN3, LOW);
 digitalWrite(AIN4, LOW);
}