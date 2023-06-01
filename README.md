# Proyecto - Reproductor MP3
## Yago Carballo Barroso y Ramon Llobet Duch

Nuestro proyecto se basa en un reproductor de audio MP3 con varias funcionalidades que se van a detallar a continuación.
Así pues, como ya hemos dicho, podremos **REPRODUCIR AUDIO** (utilizamos la biblioteca ```Audio.h``` para reproducir archivos de audio en la placa ESP32).
Podremos almacenar archivos de música en formato MP3 en una targeta SD y reproducirlos utilizando la función ```playMusic()```. La función utiliza la biblioteca ```SD.h``` para acceder a los archivos en la tarjeta SD y la biblioteca ```FS.h``` para el manejo del sistema de archivos.
Además, también podremos ***CONTROLAR LA REPRODUCCIÓN***. El programa utiliza ```botones``` conectados a pines específicos para controlar la reproducción de audio. 
Utilizamos ```tres botones```:
-  El botón ```BUTTON_pin```. Es el botón principal utilizado para pausar y reanudar la reproducción de audio (Play/Pause).
-  El botón ```BUTTON_volumeUP_pin```. Es el botón utilizado para aumentar el volumen de reproducción.
-  El botón ```BUTTON_volumeDOWN_pin```. Es el botón utilizado para disminuir el volumen de reproducción.
De esta manera podremos ***AJUSTAR EL VOLUMEN DE AUDIO***.
También podremos utilizar un ***SERVIDOR WEB Y WIFI***. El programa utiliza la biblioteca ```WiFi.h``` para establecer una conexión WiFi y crear un servidor web utilizando la biblioteca ```ESPAsyncWebServer.h```. Una vez que la conexión WiFi está establecida, podemos acceder al servidor web desde cualquier dispositivo en la misma red. El servidor web muestra una ***PÁGINA WEB CON BOTONES*** para controlar la reproducción de audio y ajustar el volumen. Ésta página web proporciona una interfaz visual para mostrar los géneros de música y las canciones disponibles. Esto permite al usuario seleccionar una canción y controlar la reproducción a través de los botones y acciones definidas en otras rutas del servidor web. Así pues también podremos hacer un ***CAMBIO DE GÉNERO DE MÚSICA A TRAVÉS DE LA PÁGINA WEB***. El programa incluye la función ```changeGenre()``` que se activa cuando se presiona el botón ```Change Genre``` en la página web. 
A diferencia del reproductor de audio implementado físicamente, la página web creada tiene dos botones más que permiten ***AVANZAR O RETROCEDER UNA CANCIÓN (EN LA PÁGINA WEB)*** i tres botones más que permiten realizar un  ***FILTRO PASO-BAJO,PASO-ALTO Y PASO-BANDA (EN LA PÁGINA WEB)***.
Finalmente, también hemos utilizado una ***PANTALLA OLED I2C (modelo SSD1306)*** que nos permitirá ***VISUALIZAR EL VOLUMEN ACTUAL, NOMBRE DE LA CANCIÓN, ESTADO DE REPRODUCCIÓN (PAUSADO/REPRODUCIENDO) Y DURACIÓN DE LA CANCIÓN (TIEMPO DE REPRODUCCIÓN Y TIEMPO TOTAL DE LA CANCIÓN)***. Hemos utilizado la biblioteca ```Adafruit_SSD1306.h``` para controlar la pantalla OLED.


Para poder realizar todas estas acciones hemos utilizado la placa ```ESP32``` y los protocolos  ```I2C``` (inter integrated circuits), ```SPI``` (Serial Peripheral Interface), y ```I2S``` (Integrated Interchip Sound).
- El ```protocolo I2C``` lo utilizamos para la comunicación con la pantalla OLED. Podremos identificar los dispositivos I2C disponibles en el bus y sus direcciones correspondientes.
- El ```protocolo SPI``` lo utilizamos para la comunicación con la tarjeta SD mediante la biblioteca ´´´SPI.h´´´ y ´´´SD.h´´´.
- El ```protocolo I2S``` lo utilizamos para  la reproducción de audio mediante la biblioteca ```Audio.h```.


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
### *4.0*
En esta versión del proyecto se crean botones para poder pausar/reanudar las canciones, subir/bajar el voluemen de audio y pasar a la siguiente/anterior canción.

```ino

#include <Arduino.h>//Incluye las definiciones básicas de Arduino para la placa ESP32.
#include "Audio.h"//Biblioteca para el manejo de reproducción de audio.
#include "SD.h"// Biblioteca para el manejo de la tarjeta SD.
#include "FS.h"//Biblioteca para el manejo del sistema de archivos.
#include <WiFi.h>//Biblioteca para el manejo de la conexión WiFi.
#include <ESPAsyncWebServer.h>//Biblioteca para crear un servidor web asíncrono.
#include <SPI.h>// Biblioteca para el manejo de la comunicación SPI.
#include <vector>//Para uso de vectores
#include <Ticker.h>//Para generar interrupciones periódicas
#include <Adafruit_SSD1306.h>//Biblioteca para el manejo de la pantalla OLED

using namespace std;

// Ancho y alto de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I/O SPI
//Pines utilizados para la comunicación SPI con la tarjeta SD.
#define SD_CS 19
#define SPI_MOSI 23
#define SPI_MISO 5
#define SPI_SCK 18

// I/O I2S
// Pines utilizados para la comunicación I2S para la reproducción de audio.
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

// Botones
#define BUTTON_pin 4
#define BUTTON_volumeUP_pin 2
#define BUTTON_volumeDOWN_pin 15

//BOTONES PARA SIGUIENTE/ANTERIOR CANCIÓN:
#define BUTTON_next_pin 14
#define BUTTON_previous_pin 13


const char *ssid = "MOVISTAR_D84E";      // WiFi casa: "MOVISTAR_D84E" -- Wifi móvil: "realme"
const char *password = ";9o2a3ei5RY#!:"; // WiFi casa: ";9o2a3ei5RY#!:" -- Wifi móvil: "123456kk"

AsyncWebServer server(80);// para gestionar el servidor web.

extern String html;

Audio audio;// para el manejo de la reproducción de audio.
File archivo;//para el acceso a archivos en la tarjeta SD.

//Vectores que almacenarán los nombres de archivos de canciones por género (rock y pop):
vector<const char *> songsRock;
vector<const char *> songsPop;
vector<const char *> *songFiles[2] = {&songsRock, &songsPop};
//para acceder a los vectores de canciones según el índice del género

// Solo para guardar los nombre de las canciones
vector<String> NsongsRock;
vector<String> NsongsPop;

// Variables para el control de la reproducción
vector<String> GenreNames;

//Para controlar la reproducción de canciones y géneros:
int SongIndex = 0;
int GenreIndex = 0;
int Volume = 6;//para almacenar el volumen de reproducción.

// Ticker para el cambio de canción automático
Ticker ticker;

// Boton
const unsigned long debounceDelay = 50;
volatile unsigned long lastDebounceTime = 0;

//PARA SIGUIENTE/ANTERIOR CANCIÓN:
volatile bool nextButtonPressed = false;
volatile bool previousButtonPressed = false;
volatile unsigned long lastNextDebounceTime = 0;
volatile unsigned long lastPreviousDebounceTime = 0;

//Las siguientes funciones  marcadas con el atributo IRAM_ATTR son rutinas de interrupción que se ejecutan cuando se produce
//un evento de interrupción en los pines configurados mediante attachInterrupt()

void IRAM_ATTR isr()
{
    //se realiza una verificación de tiempo de rebote utilizando millis() 
    //para evitar que la interrupción se active múltiples veces debido a fluctuaciones en la señal.
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        audio.pauseResume();//se realiza acción de pausar/reanudar.
        lastDebounceTime = millis(); // Actualizar el tiempo de debounce
    }
}
//Ajuste de volumen del audio.
void IRAM_ATTR ISR_volumeUP()
{
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        Volume+=2;//incrementamos el volumen en un factor de 2.
        if (Volume > 20) Volume = 20;
        audio.setVolume(Volume);
        lastDebounceTime = millis(); // Actualizar el tiempo de debounce
    }
}

void IRAM_ATTR ISR_volumeDOWN()
{
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        Volume-=2;
        if (Volume < 0) Volume = 0;
        audio.setVolume(Volume);
        lastDebounceTime = millis(); // Actualizar el tiempo de debounce
    }
}

//PARA SIGUIENTE/ANTERIOR CANCIÓN:
void IRAM_ATTR ISR_nextButton()
{
    if ((millis() - lastNextDebounceTime) > debounceDelay)
    {
        nextButtonPressed = true;
        lastNextDebounceTime = millis();
    }
}

void IRAM_ATTR ISR_previousButton()
{
    if ((millis() - lastPreviousDebounceTime) > debounceDelay)
    {
        previousButtonPressed = true;
        lastPreviousDebounceTime = millis();
    }
}


// Función para convertir el nombre de la canción en una URL para reproducir
String URLconverter(const String &linea, const String &Genre)
{
    String resultado = "/" + Genre + "/" + linea + ".mp3";
    return resultado;
}

// función lambda del ticker
//Se realiza una comprobación para determinar si la canción actual ha llegado al final. 
//Si es así, se incrementa el índice de la canción y se ajusta a cero si alcanza el límite máximo. 
//Luego, se establece una nueva conexión con el archivo de audio correspondiente.

auto tick = []()
{
    if (audio.getAudioCurrentTime() >= audio.getAudioFileDuration() && audio.getAudioFileDuration() != 0)
    { // Cuando pones radios la duración es 0
        Serial.println(audio.getAudioCurrentTime());
        Serial.println(audio.getAudioFileDuration());
        SongIndex++;
        if (SongIndex >= (*songFiles[GenreIndex]).size())
            SongIndex = 0;
        audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    };
};

// Constantes de las webradios
String LastRadio = "Radio";
const char urlRadio1[51] = "http://25553.live.streamtheworld.com/CADENASER.mp3";//direcciones URL de diferentes estaciones de radio en línea
const char urlRadio2[59] = "https://nodo02-cloud01.streaming-pro.com:8005/flaixbac.mp3";
const char urlRadio3[47] = "http://21633.live.streamtheworld.com/LOS40.mp3";
const char urlRadio4[55] = "http://25553.live.streamtheworld.com/LOS40_CLASSIC.mp3";
const char urlRadio5[49] = "https://25633.live.streamtheworld.com/RAC105.mp3";

// Objeto de la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void Init_vector_rock();
//inicializa un vector de canciones de género rock. Lee el archivo "rock.txt" y extrae los nombres de las canciones. 
//Luego, utiliza la función URLconverter para convertir cada nombre de canción en una URL válida 
//y los almacena en el vector songsRock.
void Init_vector_pop();//similar a la función anterior.
void Init_protocols();// inicializa la pantalla OLED, el protocolo I2S, el protocolo SPI y la tarjeta SD.
void Init_WebServer();//configura la conexión WiFi y muestra por la consola la dirección IP asignada al microcontrolador.
void Server_handle();//establece las distintas rutas y acciones que se manejarán en el servidor web.

void pantalla(void *parameter);//se encarga de actualizar la pantalla OLED con información relevante, como el volumen, el estado de reproducción y la duración de la canción actual.

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Inicialización de I2S, SPI y I2C
    Init_protocols();

    // Establecer un ticker que ejecute la función lambda cada 5 segundos
    ticker.attach(5, tick);

    // Rock
    Init_vector_rock();//para inicializar el vector de canciones de rock.

    // Pop
    Init_vector_pop();//para inicializar el vector de canciones de pop.

    // ----------<WEB>----------

    Init_WebServer();

    Server_handle();

    server.begin();

    // ---------</WEB>----------

    pinMode(BUTTON_pin, INPUT_PULLUP); // Configurar el pin del botón como entrada con resistencia pull-up interna
    pinMode(BUTTON_volumeUP_pin, INPUT_PULLUP);
    pinMode(BUTTON_volumeDOWN_pin, INPUT_PULLUP);
    
    //PARA SIGUIENTE/ANTERIOR CANCIÓN:
    pinMode(BUTTON_next_pin, INPUT_PULLUP);
    pinMode(BUTTON_previous_pin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTTON_pin), isr, RISING); // Asignar la interrupción al pin del botón  cuando se detecta un flanco de subida.
    attachInterrupt(digitalPinToInterrupt(BUTTON_volumeUP_pin), ISR_volumeUP, RISING);// Asigna la interrupción al pin del botón de aumento de volumen.
    attachInterrupt(digitalPinToInterrupt(BUTTON_volumeDOWN_pin), ISR_volumeDOWN, RISING);//Asigna la interrupción al pin del botón de disminución de volumen.
    
    //PARA SIGUIENTE/ANTERIOR CANCIÓN:
    attachInterrupt(digitalPinToInterrupt(BUTTON_next_pin), ISR_nextButton, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_previous_pin), ISR_previousButton, RISING);

    //Se crea una tarea en segundo plano para controlar la pantalla OLED:
    xTaskCreatePinnedToCore(
        pantalla,   /* Función que implementa la tarea. */
        "pantalla", /* Nombre de la tarea. */
        2000,       /* Tamaño de la pila de la tarea */
        NULL,       /* Parámetros de la tarea */
        1,          /* Prioridad de la tarea */
        NULL,       /* Referencia a la tarea */
        0);         /* Núcleo donde se ejecutará la tarea */

    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);// para conectar al sistema de archivos SD y cargar la primera canción del género actual.
}

void loop()
{
    audio.loop();//se encarga de controlar el reproductor de audio y mantenerlo en funcionamiento.
    //las funciones auxiliares (change_song/genre, toggle_playback, increase_volume...) se utilizan para realizar acciones específicas,
    //como cambiar la canción, cambiar el género, ajustar el volumen y actualizar la pantalla OLED.
    
    //PARA SIGUIENTE/ANTERIOR CANCIÓN:
    if (nextButtonPressed)
    {
        nextButtonPressed = false;
        SongIndex++;
        if (SongIndex >= (*songFiles[GenreIndex]).size())
            SongIndex = 0;
        audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    }

    if (previousButtonPressed)
    {
        previousButtonPressed = false;
        if (SongIndex == 0)
            SongIndex = (*songFiles[GenreIndex]).size() - 1;
        else
            SongIndex--;
        audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    }
    
}

void Init_vector_rock()//se encarga de inicializar los vectores de canciones para los géneros de rock
{

    //abre archivo de texto correspondiente, lee las canciones del archivo y las almacena en el vector songsRock.
    archivo = SD.open("/rock.txt");
    GenreNames.push_back("rock");//se guarda el nombre del género en el vector GenreNames
    String linea;
    String prov;

    if (archivo)
    {
        while (archivo.available())
        {
            linea = archivo.readStringUntil('.');
            linea.trim();
            NsongsRock.push_back(linea);
            prov = URLconverter(linea, GenreNames[GenreNames.size() - 1]);
            char *temp = new char[prov.length() + 1];
            strcpy(temp, prov.c_str());
            songsRock.push_back(temp);
        }
        songsRock.pop_back(); // El archivo tiene una linea en blanco de más al final
        archivo.close();
    }
    else
    {
        Serial.println("Error al abrir el archivo rock.txt");
    }
    return;
}
//Ahora para el género pop
void Init_vector_pop()
{
    archivo = SD.open("/pop.txt");
    GenreNames.push_back("pop");
    String linea;
    String prov;

    if (archivo)
    {
        while (archivo.available())
        {
            linea = archivo.readStringUntil('.');
            linea.trim();
            NsongsPop.push_back(linea);
            prov = URLconverter(linea, GenreNames[GenreNames.size() - 1]);
            char *temp = new char[prov.length() + 1];
            strcpy(temp, prov.c_str());
            songsPop.push_back(temp);
        }
        songsPop.pop_back();
        archivo.close();
    }
    else
    {
        Serial.println("Error al abrir el archivo pop.txt");
    }
    return;
}

void Init_protocols()
{
    // Iniciar pantalla OLED en la dirección 0x3C
    Serial.println("Iniciando pantalla OLED");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("No se encuentra la pantalla OLED");
        while (true)
            ;
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

void Init_WebServer()//para configurar y establecer la conexión con el servidor web. 
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }

    Serial.println("Conectado a la red WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void Server_handle()
{
    //se encarga de manejar las diferentes solicitudes HTTP recibidas por el servidor web.
    //la ruta "/" responde a una solicitud GET mostrando una página HTML que contiene información sobre los géneros de música (rock y pop) y las canciones disponibles. 
    //Otras rutas como "/toggle", "/next", "/previous", "/soundUp", "/soundDown" y otras, se encargan de realizar acciones específicas como pausar/reanudar la reproducción, cambiar de canción, ajustar el volumen, etc.

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
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
    request->send(200, "text/html", html); });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    request->send(200); });
    server.on("/next", HTTP_GET, [](AsyncWebServerRequest *request) { // if (SongIndex>=4) SongIndex=0;
        audio.pauseResume();
        SongIndex++;
        if (SongIndex >= (*songFiles[GenreIndex]).size())
            SongIndex = 0;
        audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
        request->send(200);
    });
    server.on("/previous", HTTP_GET, [](AsyncWebServerRequest *request) { // if (SongIndex<0) SongIndex=4;
        audio.pauseResume();
        SongIndex--;
        if (SongIndex < 0)
            SongIndex = (*songFiles[GenreIndex]).size() - 1;
        audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
        request->send(200);
    });
    server.on("/Gnext", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    GenreIndex++; SongIndex=0;
    if (GenreIndex>=2) GenreIndex=0;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200); });
    server.on("/Gprevious", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    GenreIndex--; SongIndex=0;
    if (GenreIndex<0) GenreIndex=1;
    audio.connecttoFS(SD, (*songFiles[GenreIndex])[SongIndex]);
    request->send(200); });
    server.on("/soundUp", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    Volume+=2;
    if (Volume > 21) Volume = 21;
    audio.setVolume(Volume);
    request->send(200); });
    server.on("/soundDown", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    Volume-=2;
    if(Volume < 0) Volume = 0;
    audio.setVolume(Volume);
    request->send(200); });
    server.on("/lpf", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.setTone(3,-20,-20);
    request->send(200); });
    server.on("/hpf", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.setTone(-20,-20,3);
    request->send(200); });
    server.on("/bpf", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.setTone(-20, 3,-20);
    request->send(200); });
    server.on("/normal", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.setTone(0,0,0);
    request->send(200); });

    server.on("/radio1", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    LastRadio="Cadena SER";
    audio.connecttohost(urlRadio1);
    request->send(200); });
    server.on("/radio2", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    LastRadio="Flaixbac";
    audio.connecttohost(urlRadio2);
    request->send(200); });
    server.on("/radio3", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    LastRadio="Los 40 Principales";
    audio.connecttohost(urlRadio3);
    request->send(200); });
    server.on("/radio4", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    LastRadio="Los 40 Classic";
    audio.connecttohost(urlRadio4);
    request->send(200); });
    server.on("/radio5", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    audio.pauseResume();
    LastRadio="rac 105";
    audio.connecttohost(urlRadio5);
    request->send(200); });
}

void pantalla(void *parameter)//para actualizar la pantalla OLED
{
    //Muestra el volumen actual, el género de música, el nombre de la canción y el artista. 
    //También muestra un indicador visual de reproducción (un triángulo lleno o rectángulos) dependiendo del estado de reproducción actual
    String temp_cancion;
    String temp_artista;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Audio");
    display.println("Player");
    display.display();
    vTaskDelay(2000);
    display.clearDisplay();
    display.display();
    while (1)
    {
        if (audio.getAudioFileDuration() == 0) // Si no hay canción en reproducción la duracion es 0
        {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print("Volumen:");
            display.println(Volume);
            display.drawLine(0, 12, display.width(), 10, SSD1306_WHITE);
            display.setCursor(95, 0);
            display.println("Radio");
            display.setCursor(28, 22);
            display.print("Reproduciendo:");
            display.setCursor((64 - ((LastRadio.length() / 2) * 6)), 34);
            display.print(LastRadio);
        }
        else
        {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print("Volumen:");
            display.println(Volume);
            display.drawLine(0, 12, display.width(), 10, SSD1306_WHITE);
            display.setCursor(95, 0);
            display.println("SD");
            display.setCursor(31, 50);
            display.print("0");
            display.print(audio.getAudioCurrentTime() / 60);
            display.print(":");
            if(audio.getAudioCurrentTime() % 60 < 10) display.print("0");
            display.print(audio.getAudioCurrentTime() % 60);
            display.print("/");
            display.print("0");
            display.print(audio.getAudioFileDuration() / 60);
            display.print(":");
            if(audio.getAudioFileDuration() % 60 < 10) display.print("0");
            display.print(audio.getAudioFileDuration() % 60);
            display.setCursor(0, 15);
            if (GenreIndex == 0)
            {

                int i = 0;
                bool trobat = false;
                while (i < NsongsRock[SongIndex].length() && !trobat)
                {
                    if (NsongsRock[SongIndex][i] == '-')
                    {
                        temp_cancion = NsongsRock[SongIndex];
                        temp_artista = temp_cancion.substring(0, i);
                        temp_cancion = temp_cancion.substring(i + 1);
                        trobat = true;
                    }
                    i++;
                }
            }
            else
            {

                int i = 0;
                bool trobat = false;
                while (i < NsongsPop[SongIndex].length() && !trobat)
                {
                    if (NsongsPop[SongIndex][i] == '-')
                    {
                        temp_cancion = NsongsPop[SongIndex];
                        temp_artista = temp_cancion.substring(0, i);
                        temp_cancion = temp_cancion.substring(i + 1);
                        trobat = true;
                 temp_cancion = temp_cancion.substring(i + 1);
                        trobat = true;
                    }
                    i++;


            display.setCursor(0, 35);
            display.print("de ");
            display.println(temp_artista);
        }
        if (audio.isRunning())
        {
            display.fillTriangle(112, 50, 112, 58, 122, 54, SSD1306_WHITE);
        }
        else
        {
            display.fillRect(112, 50, 4, 8, SSD1306_WHITE);
            display.fillRect(118, 50, 4, 8, SSD1306_WHITE);
        }

        display.display();

        vTaskDelay(1000);
    }
}   }
                    i++;
                }
            }

            for (int i = 0; i < temp_cancion.length(); i++)
            {
                if (temp_cancion[i] == '_')
                    temp_cancion[i] = ' ';
            }
            display.println(temp_cancion);
            display.setCursor(0, 35);
            display.print("de ");
            display.println(temp_artista);
        }
        if (audio.isRunning())
        {
            display.fillTriangle(112, 50, 112, 58, 122, 54, SSD1306_WHITE);
        }
        else
        {
            display.fillRect(112, 50, 4, 8, SSD1306_WHITE);
            display.fillRect(118, 50, 4, 8, SSD1306_WHITE);
        }

        display.display();

        vTaskDelay(1000);
    }
}

```




