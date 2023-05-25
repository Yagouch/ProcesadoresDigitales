# Proyecto - Reproductor MP3
## Yago Carballo Barroso y Ramon Llobet Duch
### **Versiones**
### *1.0*
Primera versión del proyecto, creando una página web con un botón que permita pausar y reanudar la canción.

```ino
#include<Arduino.h>

#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

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
    String html = "<html><body>";
    html += "<h1>Control de música</h1>";
    html += "<button onclick=\"toggleMusic()\">Toggle Music</button>";
    html += "<script>function toggleMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/toggle', true); xhr.send();}</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    request->send(200);
  });

  server.begin();

pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(10); // 0...21

audio.connecttoFS(SD, "002.mp3");

}

void loop(){
audio.loop();
}
```
### *2.0*
Segunda versión del proyecto. He añadido una matriz unidimensional de canciones, y dos botones para mover un index sobre esa matriz, y poder alternar entre canciones.
```ino
#include<Arduino.h>

#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

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

const int numSongs = 4;
const char* songFiles[numSongs] = {"/rock/ACDC-You_Shook_Me_All_Night_Long.mp3","/rock/JimiHendrix-All_Along_The_Watchtower.mp3","/rock/Kansas-Carry_On_Wayward_Son.mp3","/rock/TheBeatles-A_Day_In_The_Life.mp3"};
int SongIndex = 0;

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
    String html = "<html><body>";
    html += "<h1>Control de musica</h1>";
    html += "<h4><strong>Se recomienda pausar antes de pasar de cancion</strong></h4>";
    html += "<button onclick=\"toggleMusic()\">Toggle Music</button>";
    html += "<button onclick=\"nextMusic()\">Next Music</button>";
    html += "<button onclick=\"previousMusic()\">Previous Music</button>";
    
    // Generar la lista de canciones en la página
    html += "<ul>";
    for (int i = 0; i < numSongs; i++) {
      html += "<li>";
      html += songFiles[i];
      html += "</li>";
    }
    html += "</ul>";

    html += "<script>function toggleMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/toggle', true); xhr.send();}";
    html += "function nextMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/next', true); xhr.send();}";
    html += "function previousMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/previous', true); xhr.send();}</script>";
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
    audio.connecttoFS(SD, songFiles[SongIndex]);
    request->send(200);
  });
  server.on("/previous", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    SongIndex--;
    if (SongIndex<0) SongIndex=numSongs-1;
    audio.connecttoFS(SD, songFiles[SongIndex]);
    request->send(200);
  });

  server.begin();

pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(10); // 0...21

audio.connecttoFS(SD, songFiles[SongIndex]);

}

void loop(){
audio.loop();
}
```
### *3.0*
En esta versión se ha añadido una matriz de dos dimensiones, las columnas son los álbumes y las filas las canciones de cada álbum, se han añadido dos botones adicionales a la página para pasar de álbum y se ha mejorado visualmente con CSS, aparte de añadir el contenido de la matriz.

Así pues, ahora podremos controlar la reproducción de audio de diferentes canciones y géneros. Utilizamos una tarjeta SD para almacenar los archivos de música que se comunica con un módulo de audio a través de las interfaces I2S y SPI. Además, muestra una página web que permite controlar la reproducción de audio y muestra una lista de canciones organizadas por género.

```ino
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
//se crea una instancia del objeto AsyncWebServer para manejar las solicitudes HTTP en el puerto 80

Audio audio;
File archivo; // Para lectura de fichero

const int numSongs = 4;
const int numGenre = 2;
const char* songFiles[numSongs][numGenre];//matriz de punteros a char que almacena los nombres de archivo de las canciones para cada género.
String GenreNames[numGenre];
int SongIndex = 0;
int GenreIndex = 0;


String URLconverter(const String& linea,const String& Genre){
//se utiliza para convertir una línea de texto en una URL válida para acceder a un archivo de canción
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
  // Configuración del servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  // Manejador de la ruta "/"
  // Genera y envía la página HTML con los controles y la lista de canciones
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
  // Manejador de la ruta "/toggle"
  // Pausa o reanuda la reproducción de audio
    audio.pauseResume();
    request->send(200);
  });
  server.on("/next", HTTP_GET, [](AsyncWebServerRequest *request){
  // Manejador de la ruta "/next"
  // Cambia a la siguiente canción
    audio.pauseResume();
    SongIndex++;
    if (SongIndex>=numSongs) SongIndex=0;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });
  server.on("/previous", HTTP_GET, [](AsyncWebServerRequest *request){
  // Manejador de la ruta "/previous"
  // Cambia a la canción anterior
    audio.pauseResume();
    SongIndex--;
    if (SongIndex<0) SongIndex=numSongs-1;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });
  server.on("/Gnext", HTTP_GET, [](AsyncWebServerRequest *request){
  // Manejador de la ruta "/Gnext"
  // Cambia al siguiente género y reinicia la lista de canciones
    audio.pauseResume();
    GenreIndex++; SongIndex=0;
    if (GenreIndex>=numGenre) GenreIndex=0;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });
  server.on("/Gprevious", HTTP_GET, [](AsyncWebServerRequest *request){
  // Manejador de la ruta "/Gprevious"
  // Cambia al género anterior y reinicia la lista de canciones
    audio.pauseResume();
    GenreIndex--; SongIndex=0;
    if (GenreIndex<0) GenreIndex=numGenre-1;
    audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);
    request->send(200);
  });

server.begin();
// Configuración de SPI y SD
//---------I2S y SPI----------
pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);
// Configuración de I2S y audio
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(4); // Volumen de reproducción. 0...21 

// Lectura del archivo "canciones.txt
archivo = SD.open("/canciones.txt");
String linea;
String prov;
String Genero;

if (archivo){
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
  archivo.close();
} else {
  Serial.println("Error al abrir el archivo");
}
// Conexión a la primera canción y género
audio.connecttoFS(SD, songFiles[SongIndex][GenreIndex]);

}

void loop(){
audio.loop();
}
```
