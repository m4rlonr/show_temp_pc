#include <DHT.h>         // Definição da biblioteca DHT
#include <TFT_eSPI.h>    // Definição da biblioteca referente ao display
#include <SPI.h>         // Definição da biblioteca referente ao display
#include <Preferences.h> // Definição da biblioteca usada para guardar dados

Preferences preferences;
float showmax;
float showmin;

// Definição de pino do DHT 11 e tipo de sensor
#define DHTPIN 17
#define DHTTYPE DHT11

// Definição dos botões utilizados
#define BUTTON1PIN 35
#define BUTTON2PIN 0

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

bool useFahrenheit = false;   // Default to Fahrenheit
bool showTemp = true;         // Default to Temp / Humidity
long lastDebounceButton1 = 0; // Holds Button1 last debounce
long lastDebounceButton2 = 0; // Holds Button2 last debounce
long debounceDelay = 200;     // 200ms between re-polling

// INTRPT Function to execute when Button 1 is Pushed
void IRAM_ATTR toggleButton1()
{
  if ((millis() - lastDebounceButton1) > debounceDelay)
  {
    if (useFahrenheit)
    {
      useFahrenheit = false;
    }
    else
    {
      useFahrenheit = true;
    }
    lastDebounceButton1 = millis();
  }
}

// INTRPT Function to execute when Button 2 is Pushed
void IRAM_ATTR toggleButton2()
{
  if ((millis() - lastDebounceButton2) > debounceDelay)
  {
    if (showTemp)
    {
      showTemp = false;
    }
    else
    {
      showTemp = true;
    }
    lastDebounceButton2 = millis();
  }
};

// What to display if showTemp = true
void showScrn1()
{
  float t = showmax;
  float h = showmin;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(0, 30);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.println("Max     Min");
  tft.drawLine(0, 35, 250, 35, TFT_BLUE);
  tft.setCursor(0, 60);
  tft.print(t);
  tft.print(F("c"));
  tft.setCursor(130, 60);
  tft.print(h);
  tft.print(F("c"));
  tft.drawLine(0, 75, 250, 75, TFT_RED);
  tft.setCursor(0, 100);
  tft.println("Atual");
  tft.print(dht.readTemperature());
  tft.print(F("c"));
};

// What to display if showTemp = false
void showScrn2()
{
  float t = dht.readTemperature(useFahrenheit);
  float h = dht.readHumidity();
  float hi = dht.computeHeatIndex(t, h, useFahrenheit);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(50, 30);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.println("Heat Index");
  tft.drawLine(0, 35, 250, 35, TFT_BLUE);
  tft.setFreeFont(&Orbitron_Light_32);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(60, 100);
  tft.print(hi);
  if (useFahrenheit)
  {
    tft.print(F("f"));
  }
  else
  {
    tft.print(F("c"));
  }
};

void preference(float newValue)
{
  preferences.begin("temperaturas", false);

  // Definindo variaveis CORIGGIRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRr
  float maxtemp = preferences.getFloat("maxtemp", 21);
  float mintemp = preferences.getFloat("mintemp", 20);

  // Trocando dado
  if (newValue < mintemp)
  {
    Serial.printf("entrou1");
    preferences.putFloat("mintemp", newValue);
  }
  else if (newValue > maxtemp)
  {
    Serial.printf("entrou2");
    preferences.putFloat("maxtemp", newValue);
  }

  // Para exibir os dados
  showmin = mintemp;
  showmax = maxtemp;

  // Imprimindo no serial print
  // Serial.printf("Valor maxima: %.1f\n", maxtemp);
  // Serial.printf("Valor minima: %.1f\n", mintemp);

  // preferences.clear(); //Limpa os dados
  // Close the Preferences
  preferences.end();
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON1PIN, INPUT);
  pinMode(BUTTON2PIN, INPUT);
  // attachInterrupt(BUTTON1PIN, toggleButton1, FALLING);
  // attachInterrupt(BUTTON2PIN, toggleButton2, FALLING);
  dht.begin();
  tft.begin();
  tft.setRotation(1); // Landscape

  // Restart ESP
  // ESP.restart();
}

void loop()
{
  float temp = dht.readTemperature();
  preference(temp);
  // Serial.printf("temperatura sendor: %.1f\n", temp);
  if (showTemp)
  {
    showScrn1();
  } // Temp Humidity
  else
  {
    showScrn2();
  }            // Heat Index
  delay(5000); // Required by this device to function properly
}