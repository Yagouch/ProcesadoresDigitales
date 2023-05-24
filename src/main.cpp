#include <Arduino.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <vector>
#include <Ticker.h>

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

//Solo para guardar los nombre de las canciones
vector<String> NsongsRock;
vector<String> NsongsPop;
//

vector<String> GenreNames;
int SongIndex = 0;
int GenreIndex = 0;
int Volume = 2;
Ticker ticker;
bool ToggleStateUp=false;

String URLconverter(const String& linea,const String& Genre){
String resultado = "/" + Genre + "/" + linea + ".mp3";
return resultado;
}

// función lambda
auto tick = []() {
  if (!audio.isRunning() && !ToggleStateUp) {
    // Si no hay canción reproduciéndose, reproducir la siguiente
    Serial.println("Cambio de canción automático");
    SongIndex++;
    if (SongIndex >= songFiles[GenreIndex]->size()) {
      SongIndex = 0;
    }
    audio.connecttoSD(songFiles[GenreIndex]->at(SongIndex));
  }
};

//Radios

const char urlRadio1 [51] = "http://25553.live.streamtheworld.com/CADENASER.mp3";
const char urlRadio2 [59] = "https://nodo02-cloud01.streaming-pro.com:8005/flaixbac.mp3";
const char urlRadio3 [47] = "http://21633.live.streamtheworld.com/LOS40.mp3";
const char urlRadio4 [55] = "http://25553.live.streamtheworld.com/LOS40_CLASSIC.mp3";
const char urlRadio5 [49] = "https://25633.live.streamtheworld.com/RAC105.mp3";


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

//Establecer un ticker que me ejecute la función lambda cada 2 segundos
ticker.attach(5, tick);

// Rock
archivo = SD.open("/rock.txt");
GenreNames.push_back("rock");
String linea;
String prov;
String Genero;

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
//Pop
archivo = SD.open("/pop.txt");
GenreNames.push_back("pop");

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

    html += "body {font-family: Helvetica, Arial; background-color: #a1a1a1 ;}";
    html += ".album-list {display: flex;justify-content: center;}";
    html += ".album {width: 300px;margin: 10px;padding: 10px;background-color: #f2f2f2;border-radius: 5px;}";
    html += ".album img {width: 100%;height: auto;border-radius: 5px;}";
    html += ".album h2 {margin-top: 10px;font-size: 18px;text-align: center;}";
    html += ".album p {margin-top: 5px;font-size: 14px;text-align: center;}";
    html += ".album ul {margin-top: 10px;padding-left: 20px;}";
    html += ".album li {margin-bottom: 5px;}";
    
    //CSS para las radios 
    html += ".radio-list {display: flex;justify-content: center;}";
    html += ".radio {width: 300px;display: flex;flex-direction: column;align-items: center;margin: 10px;padding: 10px;background-color: #cdcdcd;border-radius: 5px;}";
    html += ".radio h2 {margin-top: 10px;font-size: 18px;text-align: center;}";
    html += ".radio button {width: 200px;margin: 10px;padding: 10px;background-color: #f2f2f2;border-radius: 5px;}";
    html += ".radio img {width: 100%;height: auto;border-radius: 5px;}";

    html += ".player {text-align: center;padding: 20px; display: flex;justify-content: center;}";
    /* Estilos de los botones */
    html += ".btn {background-color: transparent;margin-right: 10px;}";
    /* Estilos de botones específicos */
    html += ".small-image {width: 100px;height: auto;}";
    html += "</style></head><body>";

    html += "<h1>Control de musica</h1>";
    html += "<h4><strong>Se recomienda pausar antes de pasar de cancion</strong></h4>";

    /* Generar los botones de control en la página */
    html += "<div class=\"player\">";
    html += "<button class=\"btn\" onclick=\"previousMusic()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/151/151863.png?w=826&t=st=1684963918~exp=1684964518~hmac=84ca50f9a950adce7038fac7a0f4efddb4b1a4a059be569f07a5ad3d89ad7067\" alt=\"Previous_song\"></button>";
    html += "<button class=\"btn\" onclick=\"toggleMusic()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/27/27185.png?w=826&t=st=1684874834~exp=1684875434~hmac=2a3b0fb58f01eddd3535f420daf82ae9367147abdb2fb23b69ec60e0861c6c9e\" alt=\"Play_Resume\"></button>";
    html += "<button class=\"btn\" onclick=\"nextMusic()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/64/64595.png?w=826&t=st=1684963914~exp=1684964514~hmac=a3329e1afa1b7f15d0c20d1fefaae556405ecfaf030c658146ffeea51ca209d4\" alt=\"Next_song\"></button></div>";
    html += "<div class=\"player\"><button class=\"btn\" onclick=\"previousGenre()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/151/151847.png?w=826&t=st=1684963958~exp=1684964558~hmac=3c67f6af0990811e2e36036ca9006b3ac5a93d8be46f82ae587e8d6c340a3c60\" alt=\"Previous_Genre\"></button>";
    html += "<button class=\"btn\" onclick=\"nextGenre()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/254/254422.png?w=826&t=st=1684963952~exp=1684964552~hmac=dc0b709f1e2cde45b048135ab45e130d5a55412b52ccfe99e0690df44be4b376\" alt=\"next_Genre\"></button></div>";
    html += "<div class=\"player\"><button class=\"btn\" onclick=\"soundDown()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/711/711251.png?w=826&t=st=1684964926~exp=1684965526~hmac=d993bbaa36b98a4a63a24da585687507d3bd947862d3f06043dd27f5ff036e43\" alt=\"Sound_down\"></button>";
    html += "<button class=\"btn\" onclick=\"soundUp()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/37/37420.png?w=826&t=st=1684964909~exp=1684965509~hmac=99ec7931b5f80b90246a7efaa0bfc188cd57a921dcb24f144cd4ddf9e58e68a0\" alt=\"Sound_up\"></button>";
    html += "<button class=\"btn\" onclick=\"lpf()\">LPF</button>";
    html += "<button class=\"btn\" onclick=\"hpf()\">HPF</button>";
    html += "<button class=\"btn\" onclick=\"bpf()\">BPF</button>";
    html += "<button class=\"btn\" onclick=\"normal()\">Restaurar</button></div>"; //"html += "<button class=\"btn\" onclick=\"default()\">Restaurar</button>

    // Generar la lista de canciones en la página
    html += "<div class=\"album-list\">";
    html += "<div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/919JyJJiTtL._SL1500_.jpg\" alt=\"rock\">";
    html += "<h2>Rock</h2><ul>";
    html += "<li>" + NsongsRock[0] + "</li><li>" + NsongsRock[1] + "</li><li>" + NsongsRock[2] + "</li><li>" + NsongsRock[3] + "</li></ul>";
    html += "</div><div class=\"album\">";
    html += "<img src=\"https://m.media-amazon.com/images/I/718LH2M0eML._SL1500_.jpg\" alt=\"pop\">";
    html += "<h2>Pop</h2><ul>";
    html += "<li>" + NsongsPop[0] + "</li><li>" + NsongsPop[1] + "</li><li>" + NsongsPop[2] + "</li><li>" + NsongsPop[3] + "</li></ul>";
    html += "</div></div>";

    //WebRadios
    html += "<div class=\"radio-list\">";
    html += "<div class=\"radio\">";
    html += "<img src=\"https://static.mytuner.mobi/media/tvos_radios/e6xdJAvSZu.png\" alt=\"CadenaSER\">";
    html += "<h2>Cadena SER</h2>";
    html += "<button onclick=\"radio1()\">Sintonizar</button></div>";
    // 2
    html += "<div class=\"radio\">";
    html += "<img src=\"https://yt3.googleusercontent.com/ytc/AGIKgqOnFV2ZgbxrIPCXBr9D_-v9PhV-biiR7anNKEC3zw=s900-c-k-c0x00ffffff-no-rj\" alt=\"flaixbac\">";
    html += "<h2>Flaixbac</h2>";
    html += "<button onclick=\"radio2()\">Sintonizar</button></div>";
    // 3
    html += "<div class=\"radio\">";
    html += "<img src=\"https://i1.sndcdn.com/avatars-000023165070-5cl2d7-t500x500.jpg\" alt=\"40Principales\">";
    html += "<h2>Los 40 Principales</h2>";
    html += "<button onclick=\"radio3()\">Sintonizar</button></div>";
    // 4
    html += "<div class=\"radio\">";
    html += "<img src=\"https://static.mytuner.mobi/media/tvos_radios/WwgbG6g5tt.png\" alt=\"40Classic\">";
    html += "<h2>Los 40 Classic</h2>";
    html += "<button onclick=\"radio4()\">Sintonizar</button></div>";
    // 5
    html += "<div class=\"radio\">";
    html += "<img src=\"https://www.rac105.cat/assets/uploads/2017/11/logo-rac105.png\" alt=\"kissfm\">";
    html += "<h2>RAC 105</h2>";
    html += "<button onclick=\"radio5()\">Sintonizar</button></div>";
    
    html += "</div>";
    
    /*Funciones de Javascript*/
    // Botones control de música
    html += "<script>function toggleMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/toggle', true); xhr.send();};";
    html += "function nextMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/next', true); xhr.send();};";
    html += "function previousMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/previous', true); xhr.send();};";
    html += "function nextGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gnext', true); xhr.send();};";
    html += "function previousGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gprevious', true); xhr.send();};";
    html += "function soundUp() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/soundUp', true); xhr.send();};";
    html += "function soundDown() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/soundDown', true); xhr.send();};";
    //filtros
    html += "function lpf() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/lpf', true); xhr.send();};";
    html += "function hpf() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/hpf', true); xhr.send();};";
    html += "function bpf() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/bpf', true); xhr.send();};";
    html += "function normal() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/normal', true); xhr.send();};";
    // Botones control de radio
    html += "function radio1() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio1', true); xhr.send();};";
    html += "function radio2() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio2', true); xhr.send();};";
    html += "function radio3() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio3', true); xhr.send();};";
    html += "function radio4() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio4', true); xhr.send();};";
    html += "function radio5() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio5', true); xhr.send();};";
    html += "</script></body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    audio.pauseResume();
    if(audio.isRunning()){
      ToggleStateUp=false;
    }
    else{
      ToggleStateUp=true;
    }
    
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

server.begin();
// ---------</WEB>----------


audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);

}

void loop(){
audio.loop();
}