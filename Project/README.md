# Proyecto - Reproductor MP3 y Radio
## Yago Carballo Barroso y Ramon Llobet Duch

**Para ver el video recomendamos ir a la presentación en canva o al link de Youtube**

Youtube:
https://youtu.be/p_rmnvjsVH4

Canva: https://www.canva.com/design/DAFkrZTKVUo/OY1Vx892KiAGcPGYppTrtw/edit?utm_content=DAFkrZTKVUo&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton

Nuestro proyecto se basa en un reproductor de audio MP3 y radio con varias funcionalidades.

Podremos ***REPRODUCIR AUDIO*** desde una SD utilizando la biblioteca ```Audio.h``` para reproducir archivos de audio en la placa ESP32.

Podremos almacenar archivos de música en formato MP3 en una targeta SD y reproducirlos utilizando la función ```audio.connecttoFS(SD,...);```. La función utiliza la biblioteca ```SD.h``` para acceder a los archivos en la tarjeta SD y la biblioteca ```FS.h``` para el manejo del sistema de archivos.

Contaremos con ```tres botones``` conectados a pines específicos:
-  El botón ```BUTTON_pin```. Es el botón principal utilizado para pausar y reanudar la reproducción de audio (audio.pauseResume()).
-  El botón ```BUTTON_volumeUP_pin```. Es el botón utilizado para aumentar el volumen de reproducción.
-  El botón ```BUTTON_volumeDOWN_pin```. Es el botón utilizado para disminuir el volumen de reproducción.

Además del control físico por botones contaremos con una ***PÁGINA WEB***. El programa utiliza la biblioteca ```WiFi.h``` para establecer una conexión WiFi y crear un servidor web utilizando la biblioteca ```ESPAsyncWebServer.h```. Una vez que la conexión WiFi está establecida, podemos acceder al servidor web desde cualquier dispositivo en la misma red. El servidor web muestra una ***PÁGINA WEB CON BOTONES*** para controlar la reproducción de audio, ajustar el volumen, aplicar filtros y seleccionar emisoras de radio. Ésta página web proporciona una interfaz visual para mostrar los géneros de música y las canciones disponibles. Esto permite al usuario seleccionar una canción y controlar la reproducción a través de los botones y acciones definidas en otras rutas del servidor web. Así pues también podremos hacer un cambio de género y música a través de la página web. El programa incluye la función ```changeGenre()``` que se activa cuando se presiona el botón de siguiente de la segunda fila de botones en la página web.

A diferencia del reproductor de audio implementado físicamente, la página web creada tiene dos botones más que permiten ***AVANZAR O RETROCEDER A LA SIGUIENTE CANCIÓN*** y tres botones más que permiten realizar un  ***FILTRO PASO-BAJO,PASO-ALTO Y PASO-BANDA***.

Finalmente, también hemos utilizado una ***PANTALLA OLED I2C*** que nos permitirá visualizar el volumen actual, nombre de la canción que se está reproduciendo, estado de reproducción (Pausado/Reproduciendo) y duración de la canción (Tiempo de reproducción/Tiempo total de la canción). Hemos utilizado la biblioteca ```Adafruit_SSD1306.h``` para controlar la pantalla OLED.

Para poder realizar todas estas acciones hemos utilizado la placa ```ESP32``` y los protocolos  ```I2C``` (inter integrated circuits), ```SPI``` (Serial Peripheral Interface), y ```I2S``` (Integrated Interchip Sound).
- El ```protocolo I2C``` lo utilizamos para la comunicación con la pantalla OLED. Podremos identificar los dispositivos I2C disponibles en el bus y sus direcciones correspondientes.
- El ```protocolo SPI``` lo utilizamos para la comunicación con la tarjeta SD mediante la biblioteca ```SPI.h``` y ```SD.h```.
- El ```protocolo I2S``` lo utilizamos para  la reproducción de audio mediante la biblioteca ```Audio.h```.


### **RESULTADO FINAL (PÁGINA WEB E IMPLEMENTACIÓN FÍSICA DEL REPRODUCTOR)**



![IMG-3506](https://github.com/Yagouch/Proyecto/assets/126197495/256d6314-b9db-4f4a-a2a5-395a6e9cf0d5)

![IMG-3507](https://github.com/Yagouch/Proyecto/assets/126197495/bdc51460-28ac-45d3-9ff1-d88e39ec035d)

![IMG-3504](https://github.com/Yagouch/Proyecto/assets/126197495/7f7cbbd4-9f69-4d93-b2f2-faa65c781853)


