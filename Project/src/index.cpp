 #include<Arduino.h>
 
 String html ="<html><head><title>Reproductor MP3</title>"
    "<style> body {font-family: Helvetica, Arial; background-color: #a1a1a1 ;}"
    ".album-list {display: flex;justify-content: center;}"
    ".album {width: 300px;margin: 10px;padding: 10px;background-color: #f2f2f2;border-radius: 5px;}"
    ".album img {width: 100%;height: auto;border-radius: 5px;}"
    ".album h2 {margin-top: 10px;font-size: 18px;text-align: center;}"
    ".album p {margin-top: 5px;font-size: 14px;text-align: center;}"
    ".album ul {margin-top: 10px;padding-left: 20px;}"
    ".album li {margin-bottom: 5px;}"

    ".radio-list {display: flex;justify-content: center;}"
    ".radio {width: 300px;display: flex;flex-direction: column;align-items: center;margin: 10px;padding: 10px;background-color: #cdcdcd;border-radius: 5px;}"
    ".radio h2 {margin-top: 10px;font-size: 18px;text-align: center;}"
    ".radio button {width: 200px;margin: 10px;padding: 10px;background-color: #f2f2f2;border-radius: 5px;}"
    ".radio img {width: 100%;height: auto;border-radius: 5px;}"

    ".player {text-align: center;padding: 20px; display: flex;justify-content: center;}"
    /* Estilos de los botones */
    ".btn {background-color: transparent;margin-right: 10px;}"
    /* Estilos de botones específicos */
    ".small-image {width: 100px;height: auto;}"
    "</style></head><body>"

    "<h1>Control de musica</h1>"
    "<h4><strong>Se recomienda pausar antes de pasar de cancion</strong></h4>"

    /* Generar los botones de control en la página */
    "<div class=\"player\">"
    "<button class=\"btn\" onclick=\"previousMusic()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/151/151863.png?w=826&t=st=1684963918~exp=1684964518~hmac=84ca50f9a950adce7038fac7a0f4efddb4b1a4a059be569f07a5ad3d89ad7067\" alt=\"Previous_song\"></button>"
    "<button class=\"btn\" onclick=\"toggleMusic()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/27/27185.png?w=826&t=st=1684874834~exp=1684875434~hmac=2a3b0fb58f01eddd3535f420daf82ae9367147abdb2fb23b69ec60e0861c6c9e\" alt=\"Play_Resume\"></button>"
    "<button class=\"btn\" onclick=\"nextMusic()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/64/64595.png?w=826&t=st=1684963914~exp=1684964514~hmac=a3329e1afa1b7f15d0c20d1fefaae556405ecfaf030c658146ffeea51ca209d4\" alt=\"Next_song\"></button></div>"
    "<div class=\"player\"><button class=\"btn\" onclick=\"previousGenre()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/151/151847.png?w=826&t=st=1684963958~exp=1684964558~hmac=3c67f6af0990811e2e36036ca9006b3ac5a93d8be46f82ae587e8d6c340a3c60\" alt=\"Previous_Genre\"></button>"
    "<button class=\"btn\" onclick=\"nextGenre()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/254/254422.png?w=826&t=st=1684963952~exp=1684964552~hmac=dc0b709f1e2cde45b048135ab45e130d5a55412b52ccfe99e0690df44be4b376\" alt=\"next_Genre\"></button></div>"
    "<div class=\"player\"><button class=\"btn\" onclick=\"soundDown()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/711/711251.png?w=826&t=st=1684964926~exp=1684965526~hmac=d993bbaa36b98a4a63a24da585687507d3bd947862d3f06043dd27f5ff036e43\" alt=\"Sound_down\"></button>"
    "<button class=\"btn\" onclick=\"soundUp()\"><img class=\"small-image\" src=\"https://cdn-icons-png.flaticon.com/512/37/37420.png?w=826&t=st=1684964909~exp=1684965509~hmac=99ec7931b5f80b90246a7efaa0bfc188cd57a921dcb24f144cd4ddf9e58e68a0\" alt=\"Sound_up\"></button>"
    "<button class=\"btn\" onclick=\"lpf()\">LPF</button>"
    "<button class=\"btn\" onclick=\"hpf()\">HPF</button>"
    "<button class=\"btn\" onclick=\"bpf()\">BPF</button>"
    "<button class=\"btn\" onclick=\"normal()\">Restaurar</button></div>" //""<button class=\"btn\" onclick=\"default()\">Restaurar</button>

    //WebRadios
    "<div class=\"radio-list\">"
    "<div class=\"radio\">"
    "<img src=\"https://static.mytuner.mobi/media/tvos_radios/e6xdJAvSZu.png\" alt=\"CadenaSER\">"
    "<h2>Cadena SER</h2>"
    "<button onclick=\"radio1()\">Sintonizar</button></div>"
    // 2
    "<div class=\"radio\">"
    "<img src=\"https://yt3.googleusercontent.com/ytc/AGIKgqOnFV2ZgbxrIPCXBr9D_-v9PhV-biiR7anNKEC3zw=s900-c-k-c0x00ffffff-no-rj\" alt=\"flaixbac\">"
    "<h2>Flaixbac</h2>"
    "<button onclick=\"radio2()\">Sintonizar</button></div>"
    // 3
    "<div class=\"radio\">"
    "<img src=\"https://i1.sndcdn.com/avatars-000023165070-5cl2d7-t500x500.jpg\" alt=\"40Principales\">"
    "<h2>Los 40 Principales</h2>"
    "<button onclick=\"radio3()\">Sintonizar</button></div>"
    // 4
    "<div class=\"radio\">"
    "<img src=\"https://static.mytuner.mobi/media/tvos_radios/WwgbG6g5tt.png\" alt=\"40Classic\">"
    "<h2>Los 40 Classic</h2>"
    "<button onclick=\"radio4()\">Sintonizar</button></div>"
    // 5
    "<div class=\"radio\">"
    "<img src=\"https://www.rac105.cat/assets/uploads/2017/11/logo-rac105.png\" alt=\"kissfm\">"
    "<h2>RAC 105</h2>"
    "<button onclick=\"radio5()\">Sintonizar</button></div>"
    
    "</div>"
    
    /*Funciones de Javascript*/
    // Botones control de música
    "<script>function toggleMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/toggle', true); xhr.send();};"
    "function nextMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/next', true); xhr.send();};"
    "function previousMusic() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/previous', true); xhr.send();};"
    "function nextGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gnext', true); xhr.send();};"
    "function previousGenre() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/Gprevious', true); xhr.send();};"
    "function soundUp() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/soundUp', true); xhr.send();};"
    "function soundDown() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/soundDown', true); xhr.send();};"
    //filtros
    "function lpf() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/lpf', true); xhr.send();};"
    "function hpf() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/hpf', true); xhr.send();};"
    "function bpf() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/bpf', true); xhr.send();};"
    "function normal() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/normal', true); xhr.send();};"
    // Botones control de radio
    "function radio1() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio1', true); xhr.send();};"
    "function radio2() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio2', true); xhr.send();};"
    "function radio3() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio3', true); xhr.send();};"
    "function radio4() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio4', true); xhr.send();};"
    "function radio5() {var xhr = new XMLHttpRequest(); xhr.open('GET', '/radio5', true); xhr.send();};"
    "</script>";