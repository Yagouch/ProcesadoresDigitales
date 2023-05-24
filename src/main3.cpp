#include<Arduino.h>

#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h> // Lectura de fichero canciones.txt


// I/O SPI 
#define SD_CS         19
#define SPI_MOSI      23
#define SPI_MISO      5
#define SPI_SCK       18

// I/O I2S
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

const char* ssid = "MOVISTAR_D84E";
const char* password = ";9o2a3ei5RY#!:";

AsyncWebServer server(80);

Audio audio;
File archivo; // Para lectura de fichero

const int numSongs = 4;
const int numGenre = 2;
const char* songFiles[numSongs][numGenre];
String GenreNames[numGenre];
int SongIndex = 0;
int GenreIndex = 0;


String URLconverter(const String& linea,const String& Genre){
String resultado = "/" + Genre + "/" + linea + ".mp3";
return resultado;
}


void setup(){
Serial.begin(115200);
// ----------WEB----------

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><style>";

    html += "body {font-family: Helvetica, Arial; background-color: #f2f2f2;}";
    html += ".album-list {display: flex;justify-content: center;}";
    html += ".album {width: 300px;margin: 10px;padding: 10px;background-color: #f2f2f2;border-radius: 5px;}";
    html += ".album img {width: 100%;height: auto;border-radius: 5px;}";
    html += ".album h2 {margin-top: 10px;font-size: 18px;text-align: center;}";
    html += ".album p {margin-top: 5px;font-size: 14px;text-align: center;}";
    html += ".album ul {margin-top: 10px;padding-left: 20px;}";
    html += ".album li {margin-bottom: 5px;}";
    html += ".player {text-align: center;padding: 20px;}";
    /* Estilos de los botones */
    html += ".btn {display: inline-block;padding: 10px 20px;font-size: 16px;border: none;border-radius: 4px;color: #fff;background-color: #4CAF50;cursor: pointer;transition: background-color 0.3s;}";
    html += ".btn:hover {background-color: #45a049;}";
    /* Estilos de botones específicos */
    html += ".play-pause {background-color: #2196F3;}";
    html += ".previous-song {background-color: #f44336;}";
    html += ".next-song {background-color: #f44336;}";
    html += ".previous-album {background-color: #FF9800;}";
    html += ".next-album {background-color: #FF9800;}";

    html += "</style></head><body>";

    html += "<h1>Control de musica</h1>";
    html += "<h4><strong>Se recomienda pausar antes de pasar de cancion</strong></h4>";

    /* Generar los botones de control en la página */
    html += "<div class=\"player\">";
    html += "<button class=\"btn play-pause\" onclick=\"toggleMusic()\">Toggle Music </button>";
    html += "<button class=\"btn next-song\" onclick=\"nextMusic()\">Next Music</button>";
    html += "<button class=\"btn previous-song\" onclick=\"previousMusic()\">Previous Music</button>";
    html += "<button class=\"btn next-album\" onclick=\"nextGenre()\">Next Genre</button>";
    html += "<button class=\"btn previous-album\" onclick=\"previousGenre()\">Previous Genre</button></div>";

    /* Generar la lista de canciones en la página*/
    html += "<div class=\"album-list\">";
    html += "<div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/919JyJJiTtL._SL1500_.jpg\" alt=\"rock\">";
    html += "<h2>Rock</h2><ul>";
    html += "<li>" + String(songFiles[0][0]) + "</li><li>" + String(songFiles[1][0]) + "</li><li>" + String(songFiles[2][0]) + "</li><li>" + String(songFiles[3][0]) + "</li></ul>";
    html += "</div><div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/718LH2M0eML._SL1500_.jpg\" alt=\"pop\">";
    html += "<h2>Pop</h2><ul>";
    html += "<li>" + String(songFiles[0][1]) + "</li><li>" + String(songFiles[1][1]) + "</li><li>" + String(songFiles[2][1]) + "</li><li>" + String(songFiles[3][1]) + "</li></ul>";
    html += "</div></div>";
  
    html += "<script>function toggleMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/toggle', true); xhr.send();}";
    html += "function nextMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/next', true); xhr.send();}";
    html += "function previousMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/previous', true); xhr.send();}";
    html += "function nextGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gnext', true); xhr.send();}";
    html += "function previousGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gprevious', true); xhr.send();}</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    request->send(200);
  });
  server.on("/next", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    SongIndex++;
    if (SongIndex>=numSongs) SongIndex=0;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });
  server.on("/previous", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    SongIndex--;
    if (SongIndex<0) SongIndex=numSongs-1;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });
  server.on("/Gnext", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    GenreIndex++; SongIndex=0;
    if (GenreIndex>=numGenre) GenreIndex=0;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });
  server.on("/Gprevious", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    GenreIndex--; SongIndex=0;
    if (GenreIndex<0) GenreIndex=numGenre-1;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });

  server.begin();

pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(4); // 0...21

//--------pruebas--------
archivo = SD.open("/canciones.txt");
String linea;
String prov;
String Genero;

if (archivo){
//while(archivo.available()){
  for(int i=0;i<numGenre;i++){

    Genero = archivo.readStringUntil('.');
    Genero.trim();
    GenreNames[i] = Genero.substring(1);
    for(int j=0;j<numSongs;j++){
      linea = archivo.readStringUntil('.');
      linea.trim();
      prov = URLconverter(linea,GenreNames[i]);
      char* temp = new char[prov.length() + 1];
      strcpy(temp, prov.c_str());
      songFiles[j][i]=temp;
    }

  }
//}
archivo.close();
} else {
  Serial.println("Error al abrir el archivo");
}

      Serial.println(songFiles[0][0]);
      Serial.println(songFiles[0][1]);
/*
for (int i = 0; i < numSongs; i++) {
  for (int j = 0; j < numGenre; j++) {
    Serial.print(songFiles[i][j]);
    Serial.print(" ");
  }
  Serial.println("");
}
*/

//--------pruebas--------

audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);


}

void loop(){

audio.loop();

}
