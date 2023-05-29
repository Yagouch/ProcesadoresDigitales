#include <Arduino.h>
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

    attachInterrupt(digitalPinToInterrupt(BUTTON_pin), isr, RISING); // Asignar la interrupción al pin del botón  cuando se detecta un flanco de subida.
    attachInterrupt(digitalPinToInterrupt(BUTTON_volumeUP_pin), ISR_volumeUP, RISING);// Asigna la interrupción al pin del botón de aumento de volumen.
    attachInterrupt(digitalPinToInterrupt(BUTTON_volumeDOWN_pin), ISR_volumeDOWN, RISING);//Asigna la interrupción al pin del botón de disminución de volumen.

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
