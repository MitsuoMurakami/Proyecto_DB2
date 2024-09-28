#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <chrono>
int read_accesses = 0;
int write_accesses = 0;
struct Record {
    char track_name[50];
    char price[20];
    float engine_capacity;
    int cylinder;
    int horse_power;
    int top_speed;
    char seats[10];
    char brand[20];
    char country[10];

    void printRecord() const {
        std::cout << "Nombre: " << track_name
                  << ", Precio: " << price
                  << ", Capacidad del motor: " << engine_capacity << "L"
                  << ", Cilindros: " << cylinder
                  << ", Potencia: " << horse_power << " HP"
                  << ", Velocidad máxima: " << top_speed << " km/h"
                  << ", Asientos: " << seats
                  << ", Marca: " << brand
                  << ", Country: " << country << std::endl;
    }
};

struct AVLNode {
    Record record;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(const Record& rec) : record(rec), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* node) {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = std::max(height(y->left), height(y->right)) + 1;
        x->height = std::max(height(x->left), height(x->right)) + 1;

        return x; // Nueva raíz
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = std::max(height(x->left), height(x->right)) + 1;
        y->height = std::max(height(y->left), height(y->right)) + 1;

        return y; // Nueva raíz
    }

    AVLNode* insert(AVLNode* node, const Record& record) {
        if (!node) {
            write_accesses++;
            return new AVLNode(record);
        }
        read_accesses++;
        if (strcmp(record.track_name, node->record.track_name) < 0)
            node->left = insert(node->left, record);
        else if (strcmp(record.track_name, node->record.track_name) > 0)
            node->right = insert(node->right, record);
        else
            return node;

        node->height = 1 + std::max(height(node->left), height(node->right));

        int balance = getBalance(node);

        if (balance > 1 && strcmp(record.track_name, node->left->record.track_name) < 0)
            return rightRotate(node);
        if (balance < -1 && strcmp(record.track_name, node->right->record.track_name) > 0)
            return leftRotate(node);
        if (balance > 1 && strcmp(record.track_name, node->left->record.track_name) > 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && strcmp(record.track_name, node->right->record.track_name) < 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        write_accesses++;
        return node;
    }

    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current && current->left)
            current = current->left;
        return current;
    }

    AVLNode* remove(AVLNode* root, const char* track_name) {
        if (!root) return root;

        if (strcmp(track_name, root->record.track_name) < 0)
            root->left = remove(root->left, track_name);
        else if (strcmp(track_name, root->record.track_name) > 0)
            root->right = remove(root->right, track_name);
        else {
            if (!root->left || !root->right) {
                AVLNode* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else
                    *root = *temp;
                delete temp;
            } else {
                AVLNode* temp = minValueNode(root->right);
                root->record = temp->record;
                root->right = remove(root->right, temp->record.track_name);
            }
        }

        if (!root) return root;

        root->height = 1 + std::max(height(root->left), height(root->right));

        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    void printInOrder(AVLNode* node) const {
        if (node) {
            printInOrder(node->left);
            node->record.printRecord();
            printInOrder(node->right);
        }
    }

    AVLNode* search(AVLNode* node, const char* track_name) {
        if (!node) return nullptr;

        read_accesses++;  // Acceso de lectura

        if (strcmp(node->record.track_name, track_name) == 0)
            return node;

        if (strcmp(track_name, node->record.track_name) < 0)
            return search(node->left, track_name);

        return search(node->right, track_name);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(const Record& record) {
        root = insert(root, record);
    }

    void remove(const char* track_name) {
        root = remove(root, track_name);
    }

    AVLNode* search(const char* track_name) {
        return search(root, track_name);
    }

    void print() const {
        printInOrder(root);
    }
};

void measureInsertionTime(AVLTree& tree, const Record& record) {
    auto start = std::chrono::high_resolution_clock::now();
    tree.insert(record);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Tiempo de insercion: " << duration.count() << " ms" << std::endl;
}

void measureSearchTime(AVLTree& tree, const char* track_name) {
    auto start = std::chrono::high_resolution_clock::now();
    tree.search(track_name);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Tiempo de busqueda: " << duration.count() << " ms" << std::endl;
}

void executeSQL(AVLTree& tree, const std::string& sql) {
    //comando que reconoce
    std::regex insert_regex(R"(INSERT INTO cars \((.*?)\) VALUES \((.*?)\);)");
    std::regex select_regex(R"(SELECT \* FROM cars;)");
    std::regex select_specific_regex(R"(SELECT \* FROM cars WHERE car_name = '(.*?)';)");
    std::regex delete_regex(R"(DELETE FROM cars WHERE car_name = '(.*?)';)");
    std::smatch match;
    //verifica si la operacion es de insertar
    if (std::regex_match(sql, match, insert_regex)) {
        // Procesar un comando INSERT INTO
        std::string values = match[2];
        std::istringstream val_stream(values);
        std::string val_name;

        // Crear un nuevo registro basado en el comando SQL
        Record record;
        int column_index = 0;

        while (std::getline(val_stream, val_name, ',')) {
            val_name.erase(remove(val_name.begin(), val_name.end(), '\''), val_name.end());
            switch (column_index) {
                case 0: strncpy(record.track_name, val_name.c_str(), sizeof(record.track_name) - 1); break;
                case 1: strncpy(record.price, val_name.c_str(), sizeof(record.price) - 1); break;
                case 2: record.engine_capacity = std::stof(val_name); break;
                case 3: record.cylinder = std::stoi(val_name); break;
                case 4: record.horse_power = std::stoi(val_name); break;
                case 5: record.top_speed = std::stoi(val_name); break;
                case 6: strncpy(record.seats, val_name.c_str(), sizeof(record.seats) - 1); break;
                case 7: strncpy(record.brand, val_name.c_str(), sizeof(record.brand) - 1); break;
                case 8: strncpy(record.country, val_name.c_str(), sizeof(record.country) - 1); break;
            }

            column_index++;
        }
        tree.insert(record);
        //measureInsertionTime(tree, record);//contar la cantidad de tiempo en que se demora en insertar
        //std::cout << "Accesos de lectura después de insercion: " << read_accesses << std::endl;
        //std::cout << "Accesos de escritura después de insercion: " << write_accesses << std::endl;
        std::cout << "Registro insertado.\n";
        //para seleccionar todos los registros del archivo
    } else if (std::regex_match(sql, match, select_regex)) {
        // Procesar un comando SELECT *
        std::cout << "Registros:\n";
        tree.print();
        //busqueda de un solo registro
    } else if (std::regex_match(sql, match, select_specific_regex)) {
        // Procesar un comando SELECT específico
        std::string track_name = match[1];
        AVLNode* node = tree.search(track_name.c_str());
        //std::cout << "Accesos de lectura durante busqueda: " << read_accesses << std::endl;
        //measureSearchTime(tree,track_name.c_str());
        if (node) {
            node->record.printRecord();
        } else {
            std::cout << "Registro no encontrado.\n";
        }
        //eliminar un registro
    } else if (std::regex_match(sql, match, delete_regex)) {
        // Procesar un comando DELETE
        std::string track_name = match[1];
        tree.remove(track_name.c_str());
        std::cout << "Registro eliminado.\n";
        //comando que no se reconoce
    } else {
        std::cout << "Comando SQL no reconocido.\n";
    }
}

void loadRecordsFromCSV(AVLTree& tree, const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        Record record;
        std::istringstream ss(line);

        std::string temp;

        // Leer los campos de la línea separados por ';'
        std::getline(ss, temp, ';');               // Nombre del auto
        strncpy(record.track_name, temp.c_str(), sizeof(record.track_name) - 1);
        record.track_name[sizeof(record.track_name) - 1] = '\0';  // Null-termination

        std::getline(ss, temp, ';');               // Precio
        strncpy(record.price, temp.c_str(), sizeof(record.price) - 1);
        record.price[sizeof(record.price) - 1] = '\0';  // Null-termination

        ss >> record.engine_capacity;               // Capacidad del motor
        ss.ignore();                                // Ignora el delimitador
        ss >> record.cylinder;                      // Cilindros
        ss.ignore();                                // Ignora el delimitador
        ss >> record.horse_power;                   // Potencia
        ss.ignore();                                // Ignora el delimitador
        ss >> record.top_speed;                     // Velocidad máxima
        ss.ignore();                                // Ignora el delimitador

        std::getline(ss, temp, ';');               // Asientos
        strncpy(record.seats, temp.c_str(), sizeof(record.seats) - 1);
        record.seats[sizeof(record.seats) - 1] = '\0';  // Null-termination

        std::getline(ss, temp, ';');               // Marca
        strncpy(record.brand, temp.c_str(), sizeof(record.brand) - 1);
        record.brand[sizeof(record.brand) - 1] = '\0';  // Null-termination

        std::getline(ss, temp);                     // País
        strncpy(record.country, temp.c_str(), sizeof(record.country) - 1);
        record.country[sizeof(record.country) - 1] = '\0';  // Null-termination

        // Insertar el registro en el árbol AVL
        tree.insert(record);
    }
}


//comando que admite
/*
    SELECT * FROM cars;
    DELETE FROM cars WHERE car_name = 'ZNA Rich 2021 2.4L DC (4x4)';
    INSERT INTO cars (track_name, price, engine_capacity, cylinder, horse_power, top_speed, seats, brand, country) VALUES ('Nombre del auto', 'Precio', 1.5, 4, 120, 200, '5', 'Marca', 'País');
    SELECT * FROM cars WHERE car_name = 'ZNA Rich 2021 2.4L DC (4x4)';
 */



int main() {
    AVLTree tree;
    loadRecordsFromCSV(tree, "cars2.csv");
    //read_accesses=0;
    //write_accesses=0;
    std::string sql;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, sql);
        if (sql == "exit") {
            break;
        }
        executeSQL(tree, sql);
    }

    return 0;
}

