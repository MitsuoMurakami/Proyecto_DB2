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
    char car_name[80]; // PK
    char price[15];
    float engine_capacity;
    int cylinder;
    int HP;
    int top_speed;
    char seats[9];
    char brand[13];
    char country[7];
    

    void showData() {
        cout << "\nCar name: " << car_name;
        cout << "\nprice: " << price;
        cout << "\nengine capacity: " << engine_capacity;
        cout << "\ncylinder: " << cylinder;
        cout << "\nHP: " << HP;
        cout << "\ntop speed: " << top_speed;
        cout << "\nsears: " << seats;
        cout << "\nbrand: " << brand;
        cout << "\ncountry: " << country;
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
        char car_name[80]; // key
        map<string,int> mp; // map (key, index)
        vector<Registro> vector; // resultado
        
        for (int i=0; i<registros.size(); i++){ // almacenar ordenado by car_name
            mp.insert(make_pair(registros[i].car_name, i));
        }
        
        for (auto elem: mp){ // almacenar ordenado en vector
            vector.push_back(registros[elem.second]);
        }

        return vector;
    }


    void insertALL(vector<Registro> registros){ // inserta todos los registros de un vector, ordenado por codigo, archivo vacio
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

    Registro search(string key){ // car_name
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

            if (string(reg.car_name) < key) {
                l = m + 1;
            }
            else if (string(reg.car_name) > key) {
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
            if (std::string(reg.car_name) == key) {
                file.close();
                aux.close();
                return reg;
            }
        }

        file.close();
        aux.close();
        throw ("No encontrado");
    }

    vector<Registro> rangeSearch(string begin, string end){ // busqueda entre rango de car_name
        ifstream file(this->file_name, ios::binary);
        ifstream aux(this->aux_name, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        if(!aux.is_open()) throw ("No se pudo abrir el archivo");

        bool b=false;
        vector<Registro> vec; // vector de registos
        Registro reg;  

        file.seekg(0, ios::end);
        int size = file.tellg() / sizeof(Registro);

        // car_name[80]

        file.seekg(0, ios::beg);

        int l = 0;
        int u = size - 1;
        int m;
        
        while (u >= l) {
            Registro reg;
            m = l + (u - l) / 2;

            file.seekg(m * sizeof(Registro), ios::beg);
            file.read((char*)&reg, sizeof(Registro));
            
            if (reg.car_name < begin) {
                l = m + 1;
            }
            else if (reg.car_name > begin) {
                u = m - 1;
            }
            else { // Encontrado
                break;
            }
        }
        
        // recorrer mientras el codigo sea < end
        file.seekg(m * sizeof(Registro), ios::beg); 
        file.read((char*)&reg, sizeof(Registro)); // leer registro
        while (reg.car_name<=end && m<size){
            if (reg.car_name>=begin && reg.car_name<=end){
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
            if (reg.car_name>=begin && reg.car_name<=end){
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

            if (string(reg.car_name) < key) {
                l = m + 1;
            }
            else if (string(reg.car_name) > key) {
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
                if (reg.car_name == key){
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
            // aux.seekg(i*sizeof(Registro), ios::beg);
            aux.seekg(i * sizeof(Registro), ios::beg);
            aux.read((char*)&aux_reg, sizeof(Registro));

            // ir a la posición a insertar (binary search)
            file.seekg(0, ios::beg);

            int l = 0;
            int u = this->size - 1;
            int m;
            
            while (u >= l) {
                // if (u==l) break;
                m = l + (u - l) / 2;

                file.seekg(m * sizeof(Registro), ios::beg);
                file.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
                if (string(reg.car_name) > string(aux_reg.car_name)) {
                    u = m - 1;
                }
                else if (string(reg.car_name) < string(aux_reg.car_name)) {
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
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Registro record;
        // Guardar info

        // car_name[80]
        // char price[135]
        // engine_capacity
        // cylinder
        // HP
        // in_spotify_playlist
        // streams
        // key[2]
        // mode[5]
        // danceability
        // cover_url[70]

        getline(ss, token, ';');
        strncpy(record.car_name, token.c_str(), sizeof(record.car_name));
        record.car_name[sizeof(record.car_name) - 1] = '\0';
        
        strncpy(record.price, token.c_str(), sizeof(record.price));
        record.price[sizeof(record.price) - 1] = '\0';  // Asegurar el null-termination
        getline(ss, token, ';');
        
        getline(ss, token, ';');
        record.engine_capacity = stof(token);

        getline(ss, token, ';');
        record.cylinder = stoi(token);


        getline(ss, token, ';');
        record.HP = stoi(token);

        getline(ss, token, ';');
        record.top_speed = stoi(token);

        getline(ss, token, ';');
        strncpy(record.seats, token.c_str(), sizeof(record.seats));
        record.seats[sizeof(record.seats) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ';');
        strncpy(record.brand, token.c_str(), sizeof(record.brand));
        record.brand[sizeof(record.brand) - 1] = '\0';  // Asegurar el null-termination
        
        getline(ss, token, ';');
        strncpy(record.country, token.c_str(), sizeof(record.country));
        record.country[sizeof(record.country) - 1] = '\0';  // Asegurar el null-termination

        records.push_back(record); // añadir record
    }
    
    file.close();
    return records;
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
        record.showData();
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
    SequentialFile seq("data_car.dat", "aux_car.dat");
    Registro reg;
    vector<Registro> vec;

    seq.insertALL(leerCSV("data_cars.csv"));

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


