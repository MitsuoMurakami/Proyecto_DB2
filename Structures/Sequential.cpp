#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <cmath>
#include <cstring>
using namespace std;

struct Registro{
    char codigo[9];
    char nombre[20];
    char apellidos[20];
    int ciclo;
    int next;

    void showData() {
        cout << "\nCodigo: " << codigo;
        cout << "\nNombre: " << nombre;
        cout << "\nApellido: " << apellidos;
        cout << "\nCiclo : " << ciclo;
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

    vector<Registro> sort_vector_by_codigo(vector<Registro> &registros){ // ordenar by codigo
        int codigo;
        map<int,int> mp;
        vector<Registro> vector;
        vector.resize(registros.size());
        for (int i=0; i<registros.size(); i++){
            codigo = registros[i].codigo[0]*10000+registros[i].codigo[1]*1000+registros[i].codigo[2]*100+registros[i].codigo[3]*10+registros[i].codigo[4];
            mp.insert(make_pair(codigo, i));
        }
        
        for (auto elem: mp){
            vector.push_back(registros[elem.second]);
        }

        // for (auto elem: vector){
        //     elem.showData();
        // }

        return vector;
    }


    void insertALL(vector<Registro> registros){ // inserta todos los registros de un vector, ordenado por codigo, archivo vacio
        ofstream file(this->file_name, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        // tamaño de la parte ordenada
        this->size = registros.size(); // n

        // insert ordenado
        sort_vector_by_codigo(registros); // implementar ordenamiento del vector registros por char[9] (codigo), vector<Registro>

        file.seekp(0, ios::beg);
        for (Registro reg: registros){ // insertar registros ordenado
            file.write((char*)&reg, sizeof(Registro));
        }
        file.close();
    }

    void add(Registro registro){ // usar espacio auxiliar para insert
        ofstream file(this->file_name, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.seekp(0, ios::end);

        file.write((char*)&registro, sizeof(Registro));
        // si supera k = log size
        if (log(size)<k){
            //rebuild(); // construir el archivo con los registros de los auxiliares (implementar funcion)
        }
        
        // ordenar segun el codigo 
        //sort_aux_file(); // ordenar el espacio auxiliar
        
    }

    Registro search(string key){ // key
        ifstream file(this->file_name, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.seekg(size/2*sizeof(Registro), ios::beg);

        bool b=false;
        // binarysearch para buscar key
        int l=0;
        int u=size-1;
        int m;
        Registro re;
        while(u>=l){
            m = floor((l+u)/2);
            // re = ;
            if (re.codigo<key){
                u = m-1;
            }
            else if (re.codigo>key){
                l = m+1;
            }
            break;
        }

        if (!b){ // si no encontró key, realizar una búsqueda lineal
            file.seekg(size*sizeof(Registro), ios::beg);
            for (int i=0; i<k; i++){
                file.read((char*)&re, sizeof(Registro)); // se puede implementar un buffer para traer la data del espacio auxiliar
                if (re.codigo == key){
                    return re;
                }
            }
        }

    }

    vector<Registro> rangeSearch(string begin, string end){ // busqueda entre rango de codigos
        ifstream file(this->file_name, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        bool b=false;
        // binarysearch
        int l=0;
        int u=size-1;
        int m;
        Registro re;
        char key[5];
        while(u>=l){
            m = floor((l+u)/2);
            // re = ;
            if (re.codigo<key){
                u = m-1;
            }
            else if (re.codigo>key){
                l = m+1;
            }
            break;
        }

        // recorrer mientras el codigo sea < end

        // buscar en el espacio auxiliar
        // 

    }

    void eliminar(string key){ // eliminar by key

    }

    vector<Registro> reconstruction(){
        fstream file(this->file_name, ios::app | ios::binary);
        fstream aux(this->aux_name, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        if(!aux.is_open()) throw ("No se pudo abrir el archivo");





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

        getline(ss, token, ',');
        for (int i=0; i<sizeof(record.codigo); i++){
            record.codigo[i] = token[i];
        }
        record.codigo[sizeof(record.codigo)-1] = '\0';

        getline(ss, token, ',');
        strncpy(record.nombre, token.c_str(), sizeof(record.nombre));
        record.nombre[sizeof(record.nombre) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ',');
        strncpy(record.apellidos, token.c_str(), sizeof(record.apellidos));
        record.apellidos[sizeof(record.apellidos) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ',');
        record.ciclo = stoi(token);

        records.push_back(record);
    }
    
    file.close();
    return records;

    // remove (extra)

}


int main(){
    SequentialFile seq("data.dat", "aux.dat");

    seq.insertALL(leerCSV("datos.csv"));

    


}


