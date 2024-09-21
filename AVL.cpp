#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cmath>

using namespace std;

struct Record // Node
{
    int codigo;
    char nombre[12];
    char apellido[12];
    int ciclo;
    // Agregar campos extra si los necesitan
    int height; // altura del nodo
    int left; // puntero al siguiente nodo
    int right; // puntero al siguiente nodo

    Record() : left(-1), right(-1), height(0) {}  

    Record(int codigo, string nombre, string apellido, int ciclo) : codigo(codigo), ciclo(ciclo), left(-1), right(-1), height(0) {
        for (int i=0; i<12; i++){
            this->nombre[i] = nombre[i];
            this->apellido[i] = apellido[i];
        }
    }

    void showData() {
        cout << "\nCodigo: " << codigo;
        cout << "\nNombre: " << nombre;
        cout << "\nApellido: " << apellido;
        cout << "\nCiclo : " << ciclo;
    }

    bool operator==(const Record& other) const {
        return codigo == other.codigo && strcmp(nombre, other.nombre) == 0 && strcmp(apellido, other.apellido) == 0 && ciclo == other.ciclo;
    }
};

template <typename TK>
class AVLFile {
private :
    string filename;
    long pos_root;

public :
    AVLFile (string filename){
        this->filename = filename;
        pos_root = 1;
    }

    Record find (TK key){
        ifstream file(this->filename, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        Record record;
        file.seekg((pos_root-1)*sizeof(Record), ios::beg);
        while (key != record.codigo){ // mientras no se encuentra
            if (record.left==-1 && record.right==-1){ // es hoja y no se encontro
                throw ("No se pudo encontrar el registro");
            }
            if (record.codigo>key){ // el codigo del record es mayor a la key, left
                file.seekg(record.left*sizeof(Record), ios::beg);
                file.read((char*)&record, sizeof(Record));
            }
            else if (record.codigo<key){ // el codigo del record es menor a la key, right
                file.seekg(record.right*sizeof(Record), ios::beg);
                file.read((char*)&record, sizeof(Record));
            }
        }

        file.close();
        return record;
    }

    void updateheight(){
        
    }

    void balance(){

    }

    void left_rot(){

    }

    void right_rot(){

    }
    
    void insert(Record record){
        fstream file(this->filename, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        Record record_aux; 
        file.seekg(pos_root*sizeof(Record), ios::beg);
        file.read((char*)&record_aux, sizeof(Record));
        while(true){
            if (record.codigo<record_aux.codigo){ // el registro a insertar es menor que el nodo actual
                if (record_aux.left==-1){ // si el left esta libre
                    file.seekp(0, ios::end);
                    file.write((char*)&record, sizeof(Record));
                    break;
                }
                else { // si no esta libre el nodo izquierdo
                    file.seekg(record_aux.left*sizeof(Record), ios::beg);
                    file.read((char*)&record, sizeof(Record));
                }
            }
            else if (record.codigo>record_aux.codigo){ // el registro a insertar es mayor 
                if (record_aux.right==-1){ // si el right esta libre
                    file.seekp(0, ios::end);
                    file.write((char*)&record, sizeof(Record));
                    break;
                }
                else { // si no esta libre el nodo derecho
                    file.seekg(record_aux.right*sizeof(Record), ios::beg);
                    file.read((char*)&record, sizeof(Record));
                }
            }
        }

        // updateheight?

        // balance?
        file.close();
    }

    vector<Record> inorder(){ // lee los registros ordenadamente
        ifstream file(this->filename, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        vector<Record> records;
        file.seekg(pos_root*sizeof(Record), ios::beg);
        

        file.close();
    }

    // true remove(TK key){
    bool remove(TK key){
        fstream file(this->filename, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        int left, right, parent;
        Record record; 
        
        file.seekg(pos_root*sizeof(Record), ios::beg);
        file.read((char*)&record, sizeof(Record));
        left = -1;
        right = -1;
        parent = floor(record.left/2);
        while(record.left != -1 || record.right != -1){
            if (record.codigo==key){
                left = record.left;
                right = record.right;
                record.left = -2;
                record.right = -2;
            }
            else if (record.left<key){

            }
            else if (record.right>key){
                
            }
        }


        file.close();
    }

    void buildFromCSV(const string& csvFileName) {
        ifstream csvFile(csvFileName);
        if (!csvFile.is_open()) {
            cerr << "Error al abrir el archivo CSV: " << csvFileName << endl;
            return;
        }

        string line;
        // Saltar la primera línea (encabezado)
        getline(csvFile, line);

        while (getline(csvFile, line)) {
            if (line.empty()) {
                cerr << "Línea vacía encontrada." << endl;
                continue;
            }

            stringstream ss(line);
            string codigo, nombre, apellido, cicloStr;

            getline(ss, codigo, ',');
            getline(ss, nombre, ',');
            getline(ss, apellido, ',');
            getline(ss, cicloStr);


            try {
                int ciclo = stoi(cicloStr);
                nombre.erase(remove(nombre.begin(), nombre.end(), ' '), nombre.end());
                apellido.erase(remove(apellido.begin(), apellido.end(), ' '), apellido.end());

                Record record(codigo, nombre, apellido, ciclo);
                insert(record);

            } catch (const invalid_argument& e) {
                cerr << "Error al convertir el ciclo a entero: " << cicloStr << endl;
            } catch (const out_of_range& e) {
                cerr << "Número fuera del rango permitido: " << cicloStr << endl;
            }
        }

        csvFile.close();
    }
};


vector<Record> leerCSV(const string& filename) {
    ifstream file(filename);
    vector<Record> records;
    string line;

    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo.\n";
        return records;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Record record;

        getline(ss, token, ',');
        record.codigo = stoi(token);

        getline(ss, token, ',');
        strncpy(record.nombre, token.c_str(), sizeof(record.nombre));
        record.nombre[sizeof(record.nombre) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ',');
        strncpy(record.apellido, token.c_str(), sizeof(record.apellido));
        record.apellido[sizeof(record.apellido) - 1] = '\0';  // Asegurar el null-termination

        getline(ss, token, ',');
        record.ciclo = stoi(token);

        records.push_back(record);
    }

    file.close();
    return records;
}

void readFile(string filename){
    AVLFile<int> file(filename);
    cout<<"------------------------------------------\n";
    vector<Record> records = leerCSV("datos.csv");
    bool passed = true;
    for (auto& record : records) {
        Record r = file.find(record.codigo);
        if (!(r == record)){
        passed = false;
        cout << "Error en el record con codigo: " << record.codigo << "\n";
        cout << "Se esperaba: \n";
        record.showData();
        cout << "\nSe obtuvo: \n";
        r.showData();
        cout << "\n";
        break;
        }
    }
    if (passed)
        cout << "Todos los records fueron leidos correctamente\n";
}


int main() {
    // AVLFile<int> avl("avl_data.dat");

    readFile("avl_data.dat");
    // Agruegue mas casos de prueba.

    // in order
    // vector<Record> records;
    // records = avl.inorder();
    // for (Record record: records){
    //     record.showData();
    //     cout << endl;
    // }



    return 0;
}
