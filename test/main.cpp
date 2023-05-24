#include <Arduino.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>

#include <vector>
using namespace std;

// I/O SPI 
#define SD_CS         19
#define SPI_MOSI      23
#define SPI_MISO      5
#define SPI_SCK       18

File archivo;
vector<const char*> songsRock;
vector<const char*> songsPop;

vector<String> GenreNames;
int SongIndex = 0;
int GenreIndex = 0;

String URLconverter(const String& linea,const String& Genre){
String resultado = "/" + Genre + "/" + linea + ".mp3";
return resultado;
}

void setup(){

Serial.begin(115200);
delay(1000);

pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);

// Rock
archivo = SD.open("/rock.txt");
GenreNames.push_back("rock");
String linea;
String prov;
String Genero;

if (archivo){
  while (archivo.available()){
    linea = archivo.readStringUntil('.'); linea.trim();
    prov = URLconverter(linea,GenreNames[GenreNames.size()-1]);
    char* temp = new char[prov.length() + 1];
    strcpy(temp, prov.c_str());
    songsRock.push_back(temp);
  }
  songsRock.pop_back(); // El archivo tiene una linea en blanco de más al final
  archivo.close();
} else {
  Serial.println("Error al abrir el archivo rock.txt");
}
//Pop
archivo = SD.open("/pop.txt");
GenreNames.push_back("pop");

if (archivo){
  while (archivo.available()){
    linea = archivo.readStringUntil('.'); linea.trim();
    prov = URLconverter(linea,GenreNames[GenreNames.size()-1]);
    char* temp = new char[prov.length() + 1];
    strcpy(temp, prov.c_str());
    songsPop.push_back(temp);
  }
  songsPop.pop_back();
  archivo.close();
} else {
  Serial.println("Error al abrir el archivo pop.txt");
}

vector<const char*>* songFiles[2] = {&songsRock, &songsPop};

 for (int i = 0; i < 2; i++) {
        Serial.print("Songs in category ");
        Serial.print(i);
        Serial.println(": ");
        for (const auto& song : *songFiles[i]) {
            Serial.println(song);
        }
        Serial.println();
    }
}


void loop(){

}
