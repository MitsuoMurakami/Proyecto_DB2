#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>

struct Record {
    char track_name[50];
    char artist_name[50];
    int artist_count;
    int released_year;
    int released_month;
    long playlists;
    long long streams;
    char key[3];
    char mode[10];
    int danceability;
    char cover_url[100];

    void printRecord() const {
        std::cout << "Track: " << track_name << ", Artist: " << artist_name
                  << ", Artist Count: " << artist_count
                  << ", Released Year: " << released_year
                  << ", Released Month: " << released_month
                  << ", Playlists: " << playlists
                  << ", Streams: " << streams
                  << ", Key: " << key
                  << ", Mode: " << mode
                  << ", Danceability: " << danceability
                  << ", Cover URL: " << cover_url << std::endl;
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
        if (!node) return new AVLNode(record);

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
        if (!node || strcmp(node->record.track_name, track_name) == 0)
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

void executeSQL(AVLTree& tree, const std::string& sql) {
    std::regex insert_regex(R"(INSERT INTO songs \((.*?)\) VALUES \((.*?)\);)");
    std::regex select_regex(R"(SELECT \* FROM songs;)");
    std::regex select_specific_regex(R"(SELECT \* FROM songs WHERE track_name = '(.*?)';)");
    std::regex delete_regex(R"(DELETE FROM songs WHERE track_name = '(.*?)';)");
    std::smatch match;

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
                case 1: strncpy(record.artist_name, val_name.c_str(), sizeof(record.artist_name) - 1); break;
                case 2: record.artist_count = std::stoi(val_name); break;
                case 3: record.released_year = std::stoi(val_name); break;
                case 4: record.released_month = std::stoi(val_name); break;
                case 5: record.playlists = std::stol(val_name); break;
                case 6: record.streams = std::stoll(val_name); break;
                case 7: strncpy(record.key, val_name.c_str(), sizeof(record.key) - 1); break;
                case 8: strncpy(record.mode, val_name.c_str(), sizeof(record.mode) - 1); break;
                case 9: record.danceability = std::stoi(val_name); break;
                case 10: strncpy(record.cover_url, val_name.c_str(), sizeof(record.cover_url) - 1); break;
            }
            column_index++;
        }
        tree.insert(record);
        std::cout << "Registro insertado.\n";
    } else if (std::regex_match(sql, match, select_regex)) {
        // Procesar un comando SELECT *
        std::cout << "Registros:\n";
        tree.print();
    } else if (std::regex_match(sql, match, select_specific_regex)) {
        // Procesar un comando SELECT específico
        std::string track_name = match[1];
        AVLNode* node = tree.search(track_name.c_str());
        if (node) {
            node->record.printRecord();
        } else {
            std::cout << "Registro no encontrado.\n";
        }
    } else if (std::regex_match(sql, match, delete_regex)) {
        // Procesar un comando DELETE
        std::string track_name = match[1];
        tree.remove(track_name.c_str());
        std::cout << "Registro eliminado.\n";
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
        std::string track_name, artist_name;

        std::getline(ss, track_name, ';');
        std::getline(ss, artist_name, ';');
        ss >> record.artist_count;
        ss.ignore(); // Ignora el delimitador
        ss >> record.released_year;
        ss.ignore(); // Ignora el delimitador
        ss >> record.released_month;
        ss.ignore(); // Ignora el delimitador
        ss >> record.playlists;
        ss.ignore(); // Ignora el delimitador
        ss >> record.streams;
        ss.ignore(); // Ignora el delimitador

        std::string key, mode, cover_url;

        std::getline(ss, key, ';');
        std::getline(ss, mode, ';');
        ss >> record.danceability;
        ss.ignore(); // Ignora el delimitador
        std::getline(ss, cover_url);

        // Copiar los valores a los arreglos de caracteres
        strncpy(record.key, key.c_str(), sizeof(record.key) - 1);
        record.key[sizeof(record.key) - 1] = '\0';  // Asegura que el string esté null-terminated

        strncpy(record.mode, mode.c_str(), sizeof(record.mode) - 1);
        record.mode[sizeof(record.mode) - 1] = '\0';  // Asegura que el string esté null-terminated

        strncpy(record.cover_url, cover_url.c_str(), sizeof(record.cover_url) - 1);
        record.cover_url[sizeof(record.cover_url) - 1] = '\0';  // Asegura que el string esté null-terminated

        // Asignar los valores a los atributos de la estructura Record
        strncpy(record.track_name, track_name.c_str(), sizeof(record.track_name) - 1);
        record.track_name[sizeof(record.track_name) - 1] = '\0';  // Asegura que el string esté null-terminated

        strncpy(record.artist_name, artist_name.c_str(), sizeof(record.artist_name) - 1);
        record.artist_name[sizeof(record.artist_name) - 1] = '\0';  // Asegura que el string esté null-terminated

        // Insertar el registro en el árbol AVL
        tree.insert(record);
    }
}
//comando que admite
/*
    SELECT * FROM songs;
    DELETE FROM songs WHERE track_name = 'positions';
    INSERT INTO songs (track_name, artist_name, artist_count, released_year, released_month, playlists, streams, key, mode, danceability, cover_url) VALUES ('Positions', 'Ariana Grande', 1, 2020, 10, 50, 1000000, 'C', 'major', 80, 'https://example.com/cover.jpg');
    SELECT * FROM songs WHERE track_name = 'Positions';
 */

int main() {
    AVLTree tree;
    loadRecordsFromCSV(tree, "data.csv");

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

