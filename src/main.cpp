#include <Arduino.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <vector>
#include <Ticker.h>
#include <Adafruit_SSD1306.h>

using namespace std;

// Ancho y alto de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I/O SPI 
#define SD_CS         19
#define SPI_MOSI      23
#define SPI_MISO      5
#define SPI_SCK       18

// I/O I2S
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

const char* ssid = "MOVISTAR_D84E"; // WiFi casa: "MOVISTAR_D84E" -- Wifi móvil: "realme"
const char* password = ";9o2a3ei5RY#!:"; // WiFi casa: ";9o2a3ei5RY#!:" -- Wifi móvil: "123456kk"

AsyncWebServer server(80);

extern String html;

Audio audio;
File archivo;

vector<const char*> songsRock;
vector<const char*> songsPop;
vector<const char*>* songFiles[2] = {&songsRock, &songsPop};

//Solo para guardar los nombre de las canciones
vector<String> NsongsRock;
vector<String> NsongsPop;

//Variables para el control de la reproducción
vector<String> GenreNames;
int SongIndex = 0;
int GenreIndex = 0;
int Volume = 6;
//Ticker para el cambio de canción automático
Ticker ticker;

//Función para convertir el nombre de la canción en una URL para reproducir
String URLconverter(const String& linea,const String& Genre){
String resultado = "/" + Genre + "/" + linea + ".mp3";
return resultado;
}

// función lambda del ticker
auto tick = []() {
  if(audio.getAudioCurrentTime()>=audio.getAudioFileDuration() && audio.getAudioFileDuration()!=0){ //Cuando pones radios la duración es 0
    Serial.println(audio.getAudioCurrentTime());
    Serial.println(audio.getAudioFileDuration());
    SongIndex++;
    if (SongIndex>=(*songFiles[GenreIndex]).size()) SongIndex=0;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
  };
};

//Constantes de las webradios
const char urlRadio1 [51] = "http://25553.live.streamtheworld.com/CADENASER.mp3";
const char urlRadio2 [59] = "https://nodo02-cloud01.streaming-pro.com:8005/flaixbac.mp3";
const char urlRadio3 [47] = "http://21633.live.streamtheworld.com/LOS40.mp3";
const char urlRadio4 [55] = "http://25553.live.streamtheworld.com/LOS40_CLASSIC.mp3";
const char urlRadio5 [49] = "https://25633.live.streamtheworld.com/RAC105.mp3";

//Objeto de la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void Init_vector_rock();
void Init_vector_pop();
void Init_protocols();
void Init_WebServer();
void Server_handle();

void setup(){
Serial.begin(115200);
delay(1000);

//Inicialización de I2S, SPI y I2C
Init_protocols();

//Establecer un ticker que me ejecute la función lambda cada 5 segundos
ticker.attach(5, tick);

//Rock
Init_vector_rock();

//Pop
Init_vector_pop();

// ----------<WEB>----------

Init_WebServer();

Server_handle();

server.begin();

// ---------</WEB>----------


audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);

}

void loop(){
audio.loop();
}

void Init_vector_rock(){

archivo = SD.open("/rock.txt");
GenreNames.push_back("rock");
String linea;
String prov;

if (archivo){
  while (archivo.available()){
    linea = archivo.readStringUntil('.'); linea.trim();
    NsongsRock.push_back(linea);
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
return;
}

void Init_vector_pop(){
archivo = SD.open("/pop.txt");
GenreNames.push_back("pop");
String linea;
String prov;

if (archivo){
  while (archivo.available()){
    linea = archivo.readStringUntil('.'); linea.trim();
    NsongsPop.push_back(linea);
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
return;
}

void Init_protocols(){
// Iniciar pantalla OLED en la dirección 0x3C
Serial.println("Iniciando pantalla OLED");
if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("No se encuentra la pantalla OLED");
    while (true);
}

//---------I2S y SPI----------
pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
SD.begin(SD_CS);
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(Volume); // 0...21
return;
}

void Init_WebServer(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

}

void Server_handle(){

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    html += "<div class=\"album-list\">";
    html += "<div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/919JyJJiTtL._SL1500_.jpg\" alt=\"rock\">";
    html += "<h2>Rock</h2><ul>";
    html += "<li>" + NsongsRock[0] + "</li><li>" + NsongsRock[1] + "</li><li>" + NsongsRock[2] + "</li><li>" + NsongsRock[3] + "</li></ul>";
    html += "</div><div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/718LH2M0eML._SL1500_.jpg\" alt=\"pop\">";
    html += "<h2>Pop</h2><ul>";
    html += "<li>" + NsongsPop[0] + "</li><li>" + NsongsPop[1] + "</li><li>" + NsongsPop[2] + "</li><li>" + NsongsPop[3] + "</li></ul>";
    html += "</div></div></body></html>";
    request->send(200, "text/html", html);
});

server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    request->send(200);
  });
  server.on("/next", HTTP_GET, [](AsyncWebServerRequest *request){//if (SongIndex>=4) SongIndex=0;
    audio.pauseResume();
    SongIndex++;
    if (SongIndex>=(*songFiles[GenreIndex]).size()) SongIndex=0;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200);
  });
  server.on("/previous", HTTP_GET, [](AsyncWebServerRequest *request){ //if (SongIndex<0) SongIndex=4;
    audio.pauseResume();
    SongIndex--;
    if (SongIndex<0) SongIndex=(*songFiles[GenreIndex]).size()-1;
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
  server.on("/lpf", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.setTone(3,-20,-20);
    request->send(200);
  });
  server.on("/hpf", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.setTone(-20,-20,3);
    request->send(200);
  });
  server.on("/bpf", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.setTone(-20, 3,-20);
    request->send(200);
  });
  server.on("/normal", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.setTone(0,0,0);
    request->send(200);
  });
  
  server.on("/radio1", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    audio.connecttohost(urlRadio1);
    request->send(200);
  });
  server.on("/radio2", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    audio.connecttohost(urlRadio2);
    request->send(200);
  });
  server.on("/radio3", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    audio.connecttohost(urlRadio3);
    request->send(200);
  });
  server.on("/radio4", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    audio.connecttohost(urlRadio4);
    request->send(200);
  });
  server.on("/radio5", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    audio.connecttohost(urlRadio5);
    request->send(200);
  });

}