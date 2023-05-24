#include<Arduino.h>
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

// I/O I2S
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

const char* ssid = "MOVISTAR_D84E";
const char* password = ";9o2a3ei5RY#!:";

AsyncWebServer server(80);

Audio audio;
File archivo;

vector<const char*> songsRock;
vector<const char*> songsPop;
vector<const char*>* songFiles[2] = {&songsRock, &songsPop};

vector<String> GenreNames;
int SongIndex = 0;
int GenreIndex = 0;
int Volume = 4;

String URLconverter(const String& linea,const String& Genre){
String resultado = "/" + Genre + "/" + linea + ".mp3";
return resultado;
}

void setup(){
Serial.begin(115200);
delay(1000);


//---------I2S y SPI----------
pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(Volume); // 0...21

/*
// Liberar la memoria asignada a los elementos individuales de songFiles
for (auto& row : songFiles) {
  for (auto& element : row) {
    delete element; // o delete element; si se usó new en lugar de new[]
  }
  row.clear();
}
// Eliminar los elementos del vector songFiles
songFiles.clear();
songFiles.shrink_to_fit();
*/
/*
if (archivo){
  while (archivo.available()){
    if (linea[0]=='*'){
        GenreNames.push_back(linea.substring(1));
        songFiles.push_back(vector<const char*>());
        linea = archivo.readStringUntil('.'); linea.trim();
    } else {
        prov = URLconverter(linea,GenreNames[GenreNames.size()-1]);
        char* temp = new char[prov.length() + 1];
        strcpy(temp, prov.c_str());
        songFiles[songFiles.size()-1].push_back(temp);
        linea = archivo.readStringUntil('.'); linea.trim();
    }

  }
  archivo.close();
} else {
  Serial.println("Error al abrir el archivo");
}
*/
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

size_t size;

// ----------<WEB>----------

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
    html += "<div><button onclick=\"soundUp()\">Sound Up</button>";
    html += "<button onclick=\"soundDown()\">Sound Down</button></div>";

    // Generar la lista de canciones en la página
    html += "<div class=\"album-list\">";
    html += "<div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/919JyJJiTtL._SL1500_.jpg\" alt=\"rock\">";
    html += "<h2>Rock</h2><ul>";
    html += "<li>" + String(songsRock[0]) + "</li><li>" + String(songsRock[1]) + "</li><li>" + String(songsRock[2]) + "</li><li>" + String(songsRock[3]) + "</li></ul>";
    html += "</div><div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/718LH2M0eML._SL1500_.jpg\" alt=\"pop\">";
    html += "<h2>Pop</h2><ul>";
    html += "<li>" + String(songsPop[0]) + "</li><li>" + String(songsPop[1]) + "</li><li>" + String(songsPop[2]) + "</li><li>" + String(songsPop[3]) + "</li></ul>";
    html += "</div></div>";
    
  
    html += "<script>function toggleMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/toggle', true); xhr.send();}";
    html += "function nextMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/next', true); xhr.send();}";
    html += "function previousMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/previous', true); xhr.send();}";
    html += "function nextGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gnext', true); xhr.send();}";
    html += "function previousGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gprevious', true); xhr.send();}";
    html += "function soundUp() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/soundUp', true); xhr.send();}";
    html += "function soundDown() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/soundDown', true); xhr.send();}</script>";
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
    if (SongIndex>=4) SongIndex=0;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200);
  });
  server.on("/previous", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    SongIndex--;
    if (SongIndex<0) SongIndex=3;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200);
  });
  server.on("/Gnext", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    GenreIndex++; SongIndex=0;
    if (GenreIndex>=2) GenreIndex=0;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200);
  });
  server.on("/Gprevious", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    GenreIndex--; SongIndex=0;
    if (GenreIndex<0) GenreIndex=1;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200);
  });
  server.on("/soundUp", HTTP_GET, [](AsyncWebServerRequest *request){
    Volume+=2;
    audio.setVolume(Volume);
    request->send(200);
  });
  server.on("/soundDown", HTTP_GET, [](AsyncWebServerRequest *request){
    Volume-=2;
    audio.setVolume(Volume);
    request->send(200);
  });

server.begin();
// ---------</WEB>----------


audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);

}

void loop(){
audio.loop();
}