

#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include "extensible.h"


class Parser {
    string currentTableName;
    string typeIndex;
    ExtendibleHashing* extendibleHashing;


public:
    Parser() {
        currentTableName = "";
        typeIndex = "";
    }
    void executeSQL(string sql) {
    //comando que reconocev
    regex create_table_regex(R"(CREATE TABLE (.*?) FROM FILE (.*?) USING INDEX (.*?);)");
    regex insert_regex(R"(INSERT INTO (.*?) VALUES \((.*?)\);)");
    regex select_regex(R"(SELECT \* FROM (.*?) WHERE key = (.*?);)");
    regex select_range_regex(R"(SELECT FROM (.*?) WHERE key BETWEEN (.*?) AND (.*?);)");
    regex select_all_regex(R"(SELECT \* FROM (.*?);)");
    regex delete_regex(R"(DELETE FROM (.*?) WHERE key = (.*?);)");

    smatch match;
    //verifica si la operacion es de crear
    if (regex_match(sql, match, create_table_regex)) {
        // Procesar el comando CREATE TABLE
        string table_name = match[1];
        string file_path = match[2];
        string index_type = match[3];

        this->typeIndex = index_type;
        currentTableName = table_name;
        if (typeIndex == "EXT") {
            extendibleHashing = new ExtendibleHashing("dataRecord.dat", "dataIndex.dat");
            extendibleHashing->load_csv(file_path);
        }
        cout << "Tabla creada con el índice " << typeIndex << " desde el archivo: " << file_path << "\n";

    } else if (regex_match(sql, match, insert_regex)) { //verifica si la operacion es de insertar
        // INSERT
        string table_name = match[1];
        string values = match[2];

        // Crwar un nuevo registro basado en el comando SQL
        Record record;
        istringstream val_stream(values);
        string val_name;
        int column_index = 0;

        while (getline(val_stream, val_name, ',')) {
            val_name.erase(remove(val_name.begin(), val_name.end(), '\''), val_name.end());
            switch (column_index) {
                case 0: strncpy(record.track_name, val_name.c_str(), sizeof(record.track_name) - 1); break;
                case 1: strncpy(record.artist_name, val_name.c_str(), sizeof(record.artist_name) - 1); break;
                case 2: record.artist_count = stoi(val_name); break;
                case 3: record.released_year = stoi(val_name); break;
                case 4: record.released_month = stoi(val_name); break;
                case 5: record.playlists = stol(val_name); break;
                case 6: record.streams = stoll(val_name); break;
                case 7: strncpy(record.key, val_name.c_str(), sizeof(record.key) - 1); break;
                case 8: strncpy(record.mode, val_name.c_str(), sizeof(record.mode) - 1); break;
                case 9: record.danceability = stoi(val_name); break;
                case 10: strncpy(record.url, val_name.c_str(), sizeof(record.url) - 1); break;
            }
            column_index++;
        }
        record.printRecord();

        if (table_name == currentTableName) {
            if(typeIndex == "EXT") {
                extendibleHashing->insertRecord(record);
            }
        }
        cout << "Registro insertado en " << table_name << endl;

    } else if (regex_match(sql, match, select_regex)) {
        // SELECT where key
        string table_name = match[1];
        string key = match[2];

        vector<Record> result;
        if (typeIndex == "EXT") {
            result = extendibleHashing->search(key);
            if (result.size() > 0) {
                std::cout << "Track name" << " | " << "Artist name" << " | " << "Artist count" << " | " << "Released year" << " | " <<
                "Released month"
                << " | " << "Playlists" << " | " << "Streams" << " | " << "Key" << " | " << "Mode" << " | " << "Danceability" << " | " <<
                "Url" << std::endl;
                for (int i = 0; i < result.size(); i++) {
                    result[i].printRecord();
                    cout << endl;
                }
            } else {
                cout << "Registro no encontrado" << endl;
            }
        }



    } else if (std::regex_match(sql, match, delete_regex)) {
        // DELETE
        string table_name = match[1];
        string key = match[2];

        bool resultado = false;

        if (typeIndex == "EXT") {
            resultado = extendibleHashing->remove(key);
            if(resultado == true) {
                cout << "Registro eliminado de " << table_name << endl;
            } else {
                cout << "Registro no encontrado" << endl;
            }
        }

    } else if (regex_match(sql, match, select_range_regex)) {
        //SELECT con rango
        string table_name = match[1];
        string key_start = match[2];
        string key_end = match[3];

        if (typeIndex == "EXT") {
            cout << "Operacion no esta permitida" << endl;
        }
    } else if(regex_match(sql, match, select_all_regex)) {
        string table_name = match[1];
        vector<Record> result;
        result = extendibleHashing->getAll();
        if (result.size() > 0) {
            std::cout << "Track name" << " | " << "Artist name" << " | " << "Artist count" << " | " << "Released year" << " | " <<
                "Released month"
                << " | " << "Playlists" << " | " << "Streams" << " | " << "Key" << " | " << "Mode" << " | " << "Danceability" << " | " <<
                "Url" << std::endl;
            for (int i = 0; i < result.size(); i++) {
                result[i].printRecord();
                cout << endl;
            }
        } else {
            cout << "Tabla vacia" << endl;
        }

    } else {
        cout << "Comando SQL no reconocido.\n";
    }
}
};




#endif //PARSER_H
