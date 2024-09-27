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

        aux.seekp(0, ios::end);
        aux.write(reinterpret_cast<char*>(&registro), sizeof(Registro));

        this->size_k++; // tamaño actual del espacio auxiliar
        aux.close();

        // si supera k = log size        
        // si aux llegó a su límite, reconstrucción
        if (k < size_k){
            rebuild();
        }

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

        file.seekg(0, ios::beg);

        int l = 0;
        int u = size - 1;
        int m;

        while (u >= l) {
            Registro reg;
            m = l + (u - l) / 2;

            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));

            if (string(reg.track_name) < key) {
                l = m + 1;
            }
            else if (string(reg.track_name) > key) {
                u = m - 1;
            }
            else { // Encontrado
                file.close();
                aux.close();
                return reg;
            }
        }

        // Buscar en el archivo auxiliar si no se encontró en el archivo principal
        for (int i = 0; i < size_k; i++) {
            Registro reg;
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
            if (std::string(reg.track_name) == key) {
                file.close();
                aux.close();
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
        while (reg.track_name<=end && m<this->size){
            if (reg.track_name>=begin && reg.track_name<=end){
                vec.push_back(reg);
            }
            m++; // incrementar pos
            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));
        }
        



        // espacio auxiliar
        for (int i=0; i<size_k; i++){
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read((char*)&reg, sizeof(Registro));
            if (reg.track_name>=begin && reg.track_name<=end){
                vec.push_back(reg);
            }
        }

        sort_vector_by_codigo(vec); // ordenar la salida?

        file.close();
        aux.close();

        return vec;
    }

    void eliminar(string key){ // eliminar by key
        fstream file(this->file_name, ios::in | ios::out | ios::binary);
        fstream aux(this->aux_name, ios::in | ios::out | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        if(!aux.is_open()) throw ("No se pudo abrir el archivo");


        // buscar en el file
        bool b=false;

        // binarysearch para buscar key
        int l=0;
        int u=size-1;
        int m;
        Registro reg, reg2;
        while(u>=l){
            m = l + (u - l) / 2;

            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));

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

                    // Escribir el registro en la posición anterior (j-1)
                    file.seekp((j - 1) * sizeof(Registro), ios::beg);
                    file.write(reinterpret_cast<char*>(&reg), sizeof(Registro));

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
                if (reg.track_name == key){
                    int j = m + 1;

                    while (j < this->size) {
                        file.seekg(j * sizeof(Registro), ios::beg);
                        file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));

                        file.seekp((j - 1) * sizeof(Registro), ios::beg);
                        file.write(reinterpret_cast<char*>(&reg), sizeof(Registro));

                        j++;
                    }

                    this->size_k--;
                    break;
                }
            }
        }
        
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

        // insertar cada registro de aux a file   
        for (int i=0; i<size_k; i++){
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read((char*)&aux_reg, sizeof(Registro));
            // ir a la posición a insertar (binary search)
            file.seekg(0, ios::beg);

            int l = 0;
            int u = this->size - 1;
            int m;
            
            while (u >= l) {
                m = l + (u - l) / 2;

                file.seekg(m * sizeof(Registro), ios::beg);
                file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
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

                file.seekp((j + 1) * sizeof(Registro), ios::beg);
                file.write(reinterpret_cast<char*>(&reg), sizeof(Registro));

                j--;
            }

            // insertar el registro
            file.seekp(l * sizeof(Registro), ios::beg);
            file.write(reinterpret_cast<char*>(&aux_reg), sizeof(Registro));

            
            this->size++;
        }

        
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


void measureInsertionTime(SequentialFile& seq, const Registro& record) {
    auto start = std::chrono::high_resolution_clock::now();
    seq.add(record);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Tiempo de insercion: " << duration.count() << " ms" << std::endl;
}

void measureSearchTime(SequentialFile& seq, const char* track_name) {
    auto start = std::chrono::high_resolution_clock::now();
    seq.search(track_name);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Tiempo de busqueda: " << duration.count() << " ms" << std::endl;
}

void executeSQL(SequentialFile& seq, const std::string& sql) {
    //comando que reconoce
    std::regex insert_regex(R"(INSERT INTO songs \((.*?)\) VALUES \((.*?)\);)");
    std::regex select_regex(R"(SELECT \* FROM songs;)");
    std::regex select_specific_regex(R"(SELECT \* FROM songs WHERE track_name = '(.*?)';)");
    std::regex delete_regex(R"(DELETE FROM songs WHERE track_name = '(.*?)';)");
    std::smatch match;
    //verifica si la operacion es de insertar
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
                case 1: strncpy(record.artists_name, val_name.c_str(), sizeof(record.artists_name) - 1); break;
                case 2: record.artist_count = std::stoi(val_name); break;
                case 3: record.year = std::stoi(val_name); break;
                case 4: record.month = std::stoi(val_name); break;
                case 5: record.in_spotify_playlist = std::stol(val_name); break;
                case 6: record.streams = std::stoll(val_name); break;
                case 7: strncpy(record.key, val_name.c_str(), sizeof(record.key) - 1); break;
                case 8: strncpy(record.mode, val_name.c_str(), sizeof(record.mode) - 1); break;
                case 9: record.danceability = std::stoi(val_name); break;
                case 10: strncpy(record.cover_url, val_name.c_str(), sizeof(record.cover_url) - 1); break;
            }
            column_index++;
        }
        seq.add(record);
        //measureInsertionTime(seq, record);//contar la cantidad de tiempo en que se demora en insertar
        //std::cout << "Accesos de lectura después de insercion: " << read_accesses << std::endl;
        //std::cout << "Accesos de escritura después de insercion: " << write_accesses << std::endl;
        std::cout << "Registro insertado.\n";
    //para seleccionar todos los registros del archivo
    } else if (std::regex_match(sql, match, select_regex)) {
        // Procesar un comando SELECT *
        std::cout << "Registros:\n";
        vector<Registro> registros;
        registros = seq.rangeSearch("A", "zzzzzzzzzzzzzzzzzzzz");
        for (Registro i: registros){
            i.showData();
        }
    //busqueda de un solo registro
    } else if (std::regex_match(sql, match, select_specific_regex)) {
        // Procesar un comando SELECT específico
        std::string track_name = match[1];
        Registro record;
        record = seq.search(track_name);
        //std::cout << "Accesos de lectura durante busqueda: " << read_accesses << std::endl;
        //measureSearchTime(seq,track_name.c_str());
        record.showData();
        // if (node) {
        //     node->record.printRecord();
        // } else {
        //     std::cout << "Registro no encontrado.\n";
        // }
    //eliminar un registro
    } else if (std::regex_match(sql, match, delete_regex)) {
        // Procesar un comando DELETE
        std::string track_name = match[1];
        seq.eliminar(track_name.c_str());
        std::cout << "Registro eliminado.\n";
    //comando que no se reconoce
    } else {
        std::cout << "Comando SQL no reconocido.\n";
    }
}


int main(){
    SequentialFile seq("data.dat", "aux.dat");
    Registro reg;
    vector<Registro> vec;

    // seq.insertALL(leerCSV("data_songs.csv"));
    seq.insertALL(leerCSV("data2.csv"));
    // reg = seq.search("LALA");

    std::string sql;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, sql);
        if (sql == "exit") {
            break;
        }
        executeSQL(seq, sql);
    }

    // cout << reg.track_name << endl << endl;

    // cout << "range: " << endl;
    // vec = seq.rangeSearch("LALA", "WHERE SHE GOES");

    // for (Registro i: vec){
    //     cout << i.track_name << endl;
    // }

    // cout << endl;
    // cout << endl << "range2: " << endl;
    // vec.clear();
    // vec = seq.rangeSearch("A", "zzzzzzz");

    // for (Registro i: vec){
    //     cout << i.track_name << endl;
    // }

    // cout << endl;
    // string name;
    // name = "A";
    // strncpy(reg.track_name, name.c_str(), sizeof(reg.track_name));
    // seq.add(reg);

    // cout << endl;
    // cout << endl << "range3: " << endl;
    // vec.clear();
    // vec = seq.rangeSearch("A", "zzzzzzz");
    
    // for (Registro i: vec){
    //     cout << i.track_name << endl;
    // }
    
    // search A
    // cout << endl;
    // cout << "search : " << endl;
    // reg = seq.search("A");
    // cout << reg.track_name << endl;

    // name = "B";
    // strncpy(reg.track_name, name.c_str(), sizeof(reg.track_name));
    // seq.add(reg);

    // name = "C";
    // strncpy(reg.track_name, name.c_str(), sizeof(reg.track_name));
    // seq.add(reg);
    
    // name = "MITSUO";
    // strncpy(reg.track_name, name.c_str(), sizeof(reg.track_name));
    // seq.add(reg);

    // name = "zzz";
    // strncpy(reg.track_name, name.c_str(), sizeof(reg.track_name));
    // seq.add(reg);


    // cout << endl;
    // cout << endl << "-------------------------------------------------------------------------------- " << endl;
    // vec.clear();
    // vec = seq.rangeSearch("A", "zzzzzzzzzz");
    
    // for (Registro i: vec){
    //     cout << i.track_name << endl;
    // }

    // seq.eliminar("zzz");
    // vec = seq.rangeSearch("A", "zzzzzzzzzz");
    
}

