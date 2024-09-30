#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <cmath>
#include <cstring>
#include <chrono>
#include <regex>
using namespace std;

struct Registro{
    char track_name[110]; // PK
    char artists_name[135]; 
    int artist_count;
    int year;
    int month;
    int in_spotify_playlist;
    long streams;
    char key[2]; 
    char mode[5];
    int danceability;
    char cover_url[70];

    void showData() const {
        std::cout << "Track: " << track_name << ", Artist: " << artists_name
                  << ", Artist Count: " << artist_count
                  << ", Released Year: " << year
                  << ", Released Month: " << month
                  << ", Playlists: " << in_spotify_playlist
                  << ", Streams: " << streams
                  << ", Key: " << key
                  << ", Mode: " << mode
                  << ", Danceability: " << danceability
                  << ", Cover URL: " << cover_url << std::endl;
    }
};


class SequentialFile{
private:
    string file_name; // archivo data
    string aux_name; // archivo auxiliar
    int size; // tamaño ordenado
    int size_k; // tamaño aux actual
    int k; // tamaño auxiliar

public:

    SequentialFile(string file_name, string aux_name){
        this->file_name=file_name;
        this->aux_name=aux_name;
        this->size=0;
        this->k=0;
        this->size_k=0;
    }

    vector<Registro> sort_vector_by_codigo(vector<Registro> &registros){ // ordenar by PK
        char track_name[110]; // key
        map<string,int> mp; // map (key, index)
        vector<Registro> vector; // resultado
        
        for (int i=0; i<registros.size(); i++){ // almacenar ordenado by track_name
            mp.insert(make_pair(registros[i].track_name, i));
        }
        
        for (auto elem: mp){ // almacenar ordenado en vector
            vector.push_back(registros[elem.second]);
        }

        return vector;
    }


    void insertALL(vector<Registro> registros){ // inserta todos los registros de un vector, ordenado por codigo, archivo vacio
        // ofstream file(this->file_name, ios::app | ios::binary);
        ofstream file(this->file_name, ios::binary); // nuevo
        ofstream aux(this->aux_name, ios::binary); // nuevo
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        // tamaño de la parte ordenada
        this->size = registros.size(); // n

        // insert ordenado
        vector<Registro> res;
        res = sort_vector_by_codigo(registros); // implementar ordenamiento del vector registros por char[9] (codigo), vector<Registro>

        file.seekp(0, ios::beg);

        // size_k
        this->k = floor(log(size));

        for (Registro reg: res){ // insertar registros ordenado
            file.write((char*)&reg, sizeof(Registro));
        }

        file.close();
        aux.close();
    }
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    void add(Registro registro){ // usar espacio auxiliar para insert
        ofstream aux(this->aux_name, ios::in | ios::out | ios::binary);
        if (!aux.is_open()) throw ("No se pudo abrir el archivo");

        // int read = 0, write = 0;

        aux.seekp(0, ios::end);
        aux.write(reinterpret_cast<char*>(&registro), sizeof(Registro));
        // write++;

        this->size_k++; // tamaño actual del espacio auxiliar
        aux.close();

        // si supera k = log size        
        // si aux llegó a su límite, reconstrucción
        if (k < size_k){
            rebuild();
        }

        // cout << "lecturas: " << read << endl;
        // cout << "escrituras: " << write << endl;

        aux.close();
    }

    Registro search(string key){ // track_name
        ifstream file(this->file_name, ios::binary);
        ifstream aux(this->aux_name, ios::binary);
        if (!file.is_open()) throw ("No se pudo abrir el archivo");
        if (!aux.is_open()) throw ("No se pudo abrir el archivo");
        
        // Calcular el tamaño del archivo principal
        file.seekg(0, ios::end);
        int size = file.tellg() / sizeof(Registro);
        // int read = 0, write = 0;

        file.seekg(0, ios::beg);

        int l = 0;
        int u = size - 1;
        int m;

        while (u >= l) {
            Registro reg;
            m = l + (u - l) / 2;

            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));
            // read++;

            if (string(reg.track_name) < key) {
                l = m + 1;
            }
            else if (string(reg.track_name) > key) {
                u = m - 1;
            }
            else { // Encontrado
                file.close();
                aux.close();
                // cout << "lecturas: " << read << endl;
                // cout << "escrituras: " << write << endl;
                return reg;
            }
        }

        // Buscar en el archivo auxiliar si no se encontró en el archivo principal
        for (int i = 0; i < size_k; i++) {
            Registro reg;
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
            // read++;
            if (std::string(reg.track_name) == key) {
                file.close();
                aux.close();
                // cout << "lecturas: " << read << endl;
                // cout << "escrituras: " << write << endl;
                return reg;
            }
        }

        file.close();
        aux.close();
        throw ("No encontrado");
    }

    vector<Registro> rangeSearch(string begin, string end){ // busqueda entre rango de track_name
        ifstream file(this->file_name, ios::binary);
        ifstream aux(this->aux_name, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        if(!aux.is_open()) throw ("No se pudo abrir el archivo");

        bool b=false;
        vector<Registro> vec; // vector de registos
        Registro reg;  

        file.seekg(0, ios::end);
        int size = file.tellg() / sizeof(Registro);

        // int read = 0, write = 0;

        // track_name[110]

        file.seekg(0, ios::beg);

        int l = 0;
        int u = size - 1;
        int m;
        
        while (u >= l) {
            Registro reg;
            m = l + (u - l) / 2;

            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));
            // read++;
            
            if (reg.track_name < begin) {
                l = m + 1;
            }
            else if (reg.track_name > begin) {
                u = m - 1;
            }
            else { // Encontrado
                break;
            }
        }
        
        // recorrer mientras el codigo sea < end
        file.seekg(m * sizeof(Registro), ios::beg); 
        file.read((char*)&reg, sizeof(Registro)); // leer registro
        // read++;
        while (reg.track_name<=end && m<this->size){
            if (reg.track_name>=begin && reg.track_name<=end){
                vec.push_back(reg);
            }
            m++; // incrementar pos
            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));
            // read++;
        }
        



        // espacio auxiliar
        for (int i=0; i<size_k; i++){
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read((char*)&reg, sizeof(Registro));
            // read++;
            if (reg.track_name>=begin && reg.track_name<=end){
                vec.push_back(reg);
            }
        }

        sort_vector_by_codigo(vec); // ordenar la salida?

        file.close();
        aux.close();

        // cout << "lecturas: " << read << endl;
        // cout << "escrituras: " << write << endl;

        return vec;
    }

    void eliminar(string key){ // eliminar by key
        fstream file(this->file_name, ios::in | ios::out | ios::binary);
        fstream aux(this->aux_name, ios::in | ios::out | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        if(!aux.is_open()) throw ("No se pudo abrir el archivo");


        // buscar en el file
        bool b=false;
        // int read = 0, write = 0;

        // binarysearch para buscar key
        int l=0;
        int u=size-1;
        int m;
        Registro reg, reg2;
        while(u>=l){
            m = l + (u - l) / 2;

            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));
            // read++;

            if (string(reg.track_name) < key) {
                l = m + 1;
            }
            else if (string(reg.track_name) > key) {
                u = m - 1;
            }
            else {
                b = true;

                int j = m + 1; // Empezar desde la posición siguiente a donde se eliminó el registro

                while (j < this->size) {
                    // Leer el registro de la posición j
                    file.seekg(j * sizeof(Registro), ios::beg);
                    file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
                    // read++;

                    // Escribir el registro en la posición anterior (j-1)
                    file.seekp((j - 1) * sizeof(Registro), ios::beg);
                    file.write(reinterpret_cast<char*>(&reg), sizeof(Registro));
                    // write++;

                    j++;
                }

                this->size--;
                break;
            }
        }

        if (!b){ // si no encontró key, realizar una búsqueda lineal
            aux.seekg(0, ios::beg);
            for (int i=0; i<k; i++){
                aux.read((char*)&reg, sizeof(Registro)); // se puede implementar un buffer para traer la data del espacio auxiliar
                // read++;
                if (reg.track_name == key){
                    int j = m + 1;

                    while (j < this->size) {
                        file.seekg(j * sizeof(Registro), ios::beg);
                        file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
                        // read++;

                        file.seekp((j - 1) * sizeof(Registro), ios::beg);
                        file.write(reinterpret_cast<char*>(&reg), sizeof(Registro));
                        // write++;

                        j++;
                    }

                    this->size_k--;
                    break;
                }
            }
        }

        // cout << "lecturas: " << read << endl;
        // cout << "escrituras: " << write << endl;
        
        file.close();
        aux.close();
    }

    void rebuild(){
        fstream file(this->file_name, ios::in | ios::out | ios::binary);
        fstream aux(this->aux_name, ios::in | ios::out | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        if(!aux.is_open()) throw ("No se pudo abrir el archivo");

        Registro reg, aux_reg, reg2;
        string key;
        // int read = 0, write = 0;

        // insertar cada registro de aux a file   
        for (int i=0; i<size_k; i++){
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read((char*)&aux_reg, sizeof(Registro));
            // read++;
            // ir a la posición a insertar (binary search)
            file.seekg(0, ios::beg);

            int l = 0;
            int u = this->size - 1;
            int m;
            
            while (u >= l) {
                m = l + (u - l) / 2;

                file.seekg(m * sizeof(Registro), ios::beg);
                file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
                // read++;
                if (string(reg.track_name) > string(aux_reg.track_name)) {
                    u = m - 1;
                }
                else if (string(reg.track_name) < string(aux_reg.track_name)) {
                    l = m + 1;
                } 
                else { // Encontrado
                    break;
                }         
            }
            
            // mover los registros
            int j=this->size-1; // 
            while (j >= l) {
                file.seekg(j * sizeof(Registro), ios::beg);
                file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
                // read++;

                file.seekp((j + 1) * sizeof(Registro), ios::beg);
                file.write(reinterpret_cast<char*>(&reg), sizeof(Registro));
                // write++;

                j--;
            }

            // insertar el registro
            file.seekp(l * sizeof(Registro), ios::beg);
            file.write(reinterpret_cast<char*>(&aux_reg), sizeof(Registro));
            // write++;

            
            this->size++;
        }

        // cout << "lecturas: " << read << endl;
        // cout << "escrituras: " << write << endl;

        
        this->size_k = 0;
        this->k = floor(log(this->size));

        file.close();
        aux.close();

        ofstream aux_2(this->aux_name, ios::trunc | ios::binary);
        aux_2.close();
    }

};


vector<Registro> leerCSV(const string& filename) {
    ifstream file(filename);
    vector<Registro> records;
    string line;

    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo.\n";
        return records;
    }
    getline(file, line); // omitir primera linea de datos.csv
    
    
    // for (auto i: line){
    //     cout << i << endl;
    // }
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Registro record;
        // Guardar info

        // track_name[110]
        // char artists_name[135]
        // artist_count
        // year
        // month
        // in_spotify_playlist
        // streams
        // key[2]
        // mode[5]
        // danceability
        // cover_url[70]

        getline(ss, token, ';');
        strncpy(record.track_name, token.c_str(), sizeof(record.track_name));
        record.track_name[sizeof(record.track_name) - 1] = '\0';


        getline(ss, token, ';');
        strncpy(record.artists_name, token.c_str(), sizeof(record.artists_name));
        record.artists_name[sizeof(record.artists_name) - 1] = '\0';  // Asegurar el null-termination
        
        getline(ss, token, ';');
        record.artist_count = stoi(token);

        getline(ss, token, ';');
        record.year = stoi(token);

        getline(ss, token, ';');
        record.month = stoi(token);

        getline(ss, token, ';');
        record.in_spotify_playlist = stoi(token);

        getline(ss, token, ';');
        record.streams = stol(token);

        getline(ss, token, ';');
        strncpy(record.key, token.c_str(), sizeof(record.key));
        record.key[sizeof(record.key) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ';');
        strncpy(record.mode, token.c_str(), sizeof(record.mode));
        record.mode[sizeof(record.mode) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ';');
        record.danceability = stoi(token);
        
        getline(ss, token, ';');
        strncpy(record.cover_url, token.c_str(), sizeof(record.cover_url));
        record.cover_url[sizeof(record.cover_url) - 1] = '\0';  // Asegurar el null-termination

        records.push_back(record); // añadir record
    }
    
    file.close();
    return records;
}

void executeSQL(SequentialFile& seq, const std::string& sql) {
    // Expresiones regulares para las consultas
    std::regex insert_regex(R"(INSERT INTO songs \((.*?)\) VALUES \((.*?)\);)");
    std::regex select_regex(R"(SELECT \* FROM songs;)");
    std::regex select_specific_regex(R"(SELECT \* FROM songs WHERE track_name = '(.*?)';)");
    std::regex select_range_regex(R"(SELECT \* FROM songs WHERE track_name BETWEEN '(.*?)' AND '(.*?)';)");
    std::regex delete_regex(R"(DELETE FROM songs WHERE track_name = '(.*?)';)");
    std::smatch match;

    // Verifica si la operación es de insertar
    if (std::regex_match(sql, match, insert_regex)) {
        // Procesar un comando INSERT INTO
        std::string values = match[2];
        std::istringstream val_stream(values);
        std::string val_name;

        // Crear un nuevo registro basado en el comando SQL
        Registro record;
        int column_index = 0;

        while (std::getline(val_stream, val_name, ',')) {
            val_name.erase(remove(val_name.begin(), val_name.end(), '\''), val_name.end());
            switch (column_index) {
                case 0: strncpy(record.track_name, val_name.c_str(), sizeof(record.track_name) - 1); break;
                case 1: strncpy(record.artists_name, val_name.c_str(), sizeof(record.artists_name)-1); break;
                case 2: record.artist_count = std::stoi(val_name); break;
                case 3: record.year = std::stoi(val_name); break;
                case 4: record.month = std::stoi(val_name); break;
                case 5: record.in_spotify_playlist = std::stoi(val_name); break;
                case 6: record.streams = std::stol(val_name); break;
                case 7: strncpy(record.key, val_name.c_str(), sizeof(record.key) - 1); break;
                case 8: strncpy(record.mode, val_name.c_str(), sizeof(record.mode) - 1); break;
                case 9: record.danceability = std::stoi(val_name); break;
                case 10: strncpy(record.cover_url, val_name.c_str(), sizeof(record.cover_url) - 1); break;
            }
            column_index++;
        }
        seq.add(record);
        std::cout << "Registro insertado.\n";

    // Para seleccionar todos los registros del archivo
    } else if (std::regex_match(sql, match, select_regex)) {
        std::cout << "Registros:\n";
        std::vector<Registro> registros;
        registros = seq.rangeSearch("A", "zzzzzzzzzzzzzzzzzzzz");
        for (Registro i : registros) {
            i.showData();
        }

    // Para búsqueda de un solo registro
    } else if (std::regex_match(sql, match, select_specific_regex)) {
        std::string track_name = match[1];
        Registro record = seq.search(track_name);
        record.showData();

    // Para eliminar un registro
    } else if (std::regex_match(sql, match, delete_regex)) {
        std::string track_name = match[1];
        seq.eliminar(track_name.c_str());
        std::cout << "Registro eliminado.\n";

    // Para búsqueda por rango
    } else if (std::regex_match(sql, match, select_range_regex)) {
        std::string start_value = match[1];  // Valor inicial del rango
        std::string end_value = match[2];    // Valor final del rango

        // Comprobar que los valores capturados son correctos
        std::cout << "Start value: " << start_value << "\n";
        std::cout << "End value: " << end_value << "\n";

        // Ejecutar búsqueda por rango
        std::vector<Registro> registros = seq.rangeSearch(start_value, end_value);

        // Mostrar los resultados
        std::cout << "Registros encontrados en el rango de '" << start_value << "' a '" << end_value << "':\n";
        for (Registro record : registros) {
            record.showData();
        }

        if (registros.empty()) {
            std::cout << "No se encontraron registros en el rango especificado.\n";
        }
    } else {
        std::cout << "Comando SQL no reconocido.\n";
    }
}

int main(){
    SequentialFile seq("data.dat", "aux.dat");
    Registro reg;
    vector<Registro> vec;

    seq.insertALL(leerCSV("data_songs.csv"));
   
   
   std::string sql;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, sql);
        if (sql == "exit") {
            break;
        }
        executeSQL(seq, sql);
    }
}

// SELECT * FROM songs;
// SELECT * FROM songs WHERE track_name = 'WHERE SHE GOES';
// SELECT * FROM songs WHERE track_name BETWEEN 'As It Was' AND 'Columbia';
// INSERT INTO songs (track_name, artists_name, artist_count, year, month, in_spotify_playlist, streams, key, mode, danceability, cover_url) VALUES ('Pollito con papas', 'nombre', '1', '2016', '6', '1000', '2000', 'A', 'Majo', '100', 'https://pollitoconpapas.com');
// SELECT * FROM songs WHERE track_name = 'Pollito con papas';
// DELETE FROM songs WHERE track_name = 'Pollito con papas';
