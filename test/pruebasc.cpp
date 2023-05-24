#include <iostream>
#include <string>
#include <cstring>

#include <vector>

using namespace std;

string URLconverter(string linea, string Genre) {
    string resultado = "/" + Genre + "/" + linea + ".mp3";
    return resultado;
}

/*
int main() {
    const int numSongs = 4;
    const int numGenre = 2;
    const char* songFiles[numSongs][numGenre];
    string GenreNames = "Genre";
    string can = "cancion";

    string prov;

    for (int i = 0; i < numGenre; i++) {
        for (int j = 0; j < numSongs; j++) {
            prov = URLconverter(can, GenreNames);
            char* temp = new char[prov.length() + 1];
            strcpy(temp, prov.c_str());
            songFiles[j][i] = temp;
        }
    }

    // escritura
    for (int i = 0; i < numSongs; i++) {
        for (int j = 0; j < numGenre; j++) {
            cout << songFiles[i][j] << " ";
        }
        cout << endl;
    }

    // Liberar la memoria asignada
    for (int i = 0; i < numSongs; i++) {
        for (int j = 0; j < numGenre; j++) {
            delete[] songFiles[i][j];
        }
    }

    return 0;
}
*/
int main() {

vector<vector<const char*>> songFiles;
vector<string> GenreNames;
int SongIndex = 0;
int GenreIndex = 0;

string prov;
string linea;

//Rellenar la matriz de canciones

for (int i = 0; i < 2; i++) {
    songFiles.push_back(vector<const char*>());
    GenreNames.push_back("Genre");
    for (int j = 0; j < 4; j++) {
        prov = URLconverter("cancion", GenreNames[i]);
        char* temp = new char[prov.length() + 1];
        strcpy(temp, prov.c_str());
        songFiles[i].push_back(temp);
    }

}

//Escribir la matriz de canciones
for (int i = 0; i < songFiles.size(); i++) {
    for (int j = 0; j < songFiles[i].size(); j++) {
        cout << songFiles[i][j] << " ";
    }
    cout << endl;
}


}


