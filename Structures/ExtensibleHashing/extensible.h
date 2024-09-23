//
// Created by Usuario on 23/09/2024.
//

#ifndef EXTENSIBLE_H
#define EXTENSIBLE_H

#define MAX_RECORDS 3
#define GLOBAL_DEPTH 5

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
#include <vector>
#include <cmath>
#include <set>
#include <string.h>
#include "record.h"

using namespace std;

int posBucket(int key, int d)  {
    hash<int> hasher;
    return hasher(key) & ((1 << d) - 1);  // Considerar solo los bits de la profundidad global.
}


struct Bucket {
    int capacity;
    int size;
    int localDepth;
    Record records[MAX_RECORDS];
    int nextBucket;
    Bucket() {
        capacity = MAX_RECORDS;
        size = 0;
        localDepth = 1;
        nextBucket = -1;
    }
    Bucket(int size, int localDepth) {
        this->capacity = MAX_RECORDS;
        this->size = size;
        this->localDepth = localDepth;
        this->nextBucket = -1;
    }
    void printBucket() {
        cout << "Bucket: " << endl;

        for (int i = 0; i < size; i++) {
            records[i].printRecord();
        }
    }
};

struct IndexFile {
    int globalDepth; // fijo
    int size;
    vector<int> posBuckets;

    IndexFile() {
        globalDepth = GLOBAL_DEPTH;
        size = pow(2, GLOBAL_DEPTH);
        posBuckets.resize(size);
        for (int i = 0; i < size; i++) {
            posBuckets[i] = -1;
        }

    }
};
class ExtendibleHashing {
private:
    fstream fileData;
    fstream fileIndex;
    IndexFile indexFile;

    Bucket* readBucket(int posBucket) {
        // TODO verificar si lee todos los record del Bucket
        if(posBucket == -1) {
            return nullptr;
        }
        Bucket* newBucket = new Bucket();

        fileData.seekg(posBucket, ios::beg);
        fileData.read((char*)newBucket, sizeof(Bucket));

        return newBucket;
    }
    void splitOnlyBucket(Bucket* bucket, int pos) {
        Bucket* oneBucket = new Bucket(0, bucket->localDepth);
        Bucket* zeroBucket = new Bucket(0, bucket->localDepth);

        int ones = 0;
        int zeroes = 0;

        // indices de los buckets
        int onesHash = 0;
        int zeroesHash = 0;

        for(int i = 0; i < bucket->size; i++) {
            Record record = bucket->records[i];
            // generar nuevas posiciones, unos tedran la misma posicion y
            // otros incrementara
            int newPos = posBucket(record.code, bucket->localDepth + 1);
            int lastPos = posBucket(record.code, bucket->localDepth);
            if(newPos == lastPos) {
                zeroBucket->records[zeroes] = record;
                zeroes++;
                zeroesHash = newPos; // pos del bucket
            } else {
                oneBucket->records[ones] = record;
                ones++;
                onesHash = newPos;
            }

        }

        zeroBucket->size = zeroes;
        oneBucket->size = ones;

        oneBucket->localDepth = bucket->localDepth + 1;
        zeroBucket->localDepth = bucket->localDepth + 1;

        writeBucket(zeroBucket, indexFile.posBuckets[zeroesHash]);
        int newBucketPos = writeBucket(oneBucket, -1);

        for(int i = 0; i < indexFile.size; i++) {
        // todos los indices que apuntan al bucket y su nuevo es es diferente del antiguo
            if(indexFile.posBuckets[i] == pos && posBucket(i, bucket->localDepth + 1) != posBucket(i, bucket->localDepth)) {
                indexFile.posBuckets[i] = newBucketPos;
            }
        }

        // write the index
        updateFileIndex();


    }
    int writeBucket(Bucket* bucket, int posData) {

        if (!fileIndex.is_open() || !fileData.is_open()) {
            throw runtime_error("Error opening files in write");
        }

        int cantBuckets = 0;
        fileData.seekg(0, ios::beg);
        fileData.read((char*)&cantBuckets, sizeof(int));

        if(posData != -1){
            fileData.seekp(posData, ios::beg);
            fileData.write((char*)bucket, sizeof(Bucket));

            return posData;
        } else{
            fileData.seekp(cantBuckets * sizeof(Bucket) + sizeof(int), ios::beg);
            int newPos = fileData.tellp();
            fileData.write((char*)bucket, sizeof(Bucket));

            cantBuckets++;
            fileData.seekp(0, ios::beg);
            fileData.write((char*)&cantBuckets, sizeof(int));

            return newPos;
        }

    }
    void updateFileIndex() {

        fileIndex.seekp(0, ios::beg);
        fileIndex.write((char*)&indexFile.globalDepth, sizeof(int));
        fileIndex.write((char*)&indexFile.size, sizeof(int));
        int arr[indexFile.size];
        for(int i = 0; i < indexFile.size; i++) {
            arr[i] = indexFile.posBuckets[i];
        }
        fileIndex.write((char*)arr, sizeof(int) * indexFile.size);

    }
    void readIndexFile() {

        fileIndex.seekg(0, ios::beg);
        int gd = 0;
        int sz = 0;
        int arr[indexFile.size];
        fileIndex.read((char*)&gd, sizeof(int));
        fileIndex.read((char*)&sz, sizeof(int));
        fileIndex.read((char*)arr, sizeof(int) * indexFile.size );

    }

public:
    ExtendibleHashing(string filedata, string fileindex) {
        // Create files
        fileIndex.open(fileindex, ios::out | ios::binary);
        fileData.open(filedata, ios::out | ios::binary);

        if (!fileIndex.is_open() || !fileData.is_open()) {
            throw runtime_error("Error opening files");
        }

        fileIndex.close();
        fileData.close();

        // open to read
        fileIndex.open(fileindex, ios::out | ios::binary | ios::in);
        fileData.open(filedata, ios::out | ios::binary | ios::in);

        fileIndex.seekg(0, ios::end);

        indexFile = IndexFile();

        if(fileIndex.tellg() < 1) {
            // create the first 2 buckets
            Bucket bucket;
            fileData.seekp(0, ios::beg);
            int cantBuckets = 2;
            fileData.write((char*)&cantBuckets, sizeof(int));
            int pos0 = fileData.tellp();
            fileData.write((char*)&bucket, sizeof(Bucket));
            int pos1 = fileData.tellp();
            fileData.write((char*)&bucket, sizeof(Bucket));

            for(int i = 0; i < pow(2, GLOBAL_DEPTH ); i++) {
                if(i % 2 == 0) {
                    indexFile.posBuckets[i] = pos0;
                } else {
                    indexFile.posBuckets[i] = pos1;
                }

            }

            updateFileIndex();
        } else {

            readIndexFile();
        }

    }
    void insertRecord(Record record) {

        // Buscar el bucket y:
        int posIndex = posBucket(record.code, indexFile.globalDepth);

        if(posIndex >= pow(2, indexFile.globalDepth)) {
            throw out_of_range("Bucket out of range");
        }
        int pos = indexFile.posBuckets[posIndex];
        Bucket* theBucket = readBucket(pos);
        // 1. Si hay espacio, insertar


        if(theBucket->size < theBucket->capacity) {
            theBucket->records[theBucket->size] = record;
            theBucket->size = theBucket->size + 1;
            writeBucket(theBucket, pos);
            cout << "Insertando1: " << record.code << "  " << pos <<endl;
        }
        // 2. Si esta lleno:
        // 2.1 Si su local depth es menor  al global depth
        // hacer una division del bucket: leer el bucket, separarlo en 2
        // y localizar la posicion del otro bucket
        // 2.2 Si su local depth es igual al global depth
        //  hacer chaining
        else if(theBucket->size == theBucket->capacity) {
            if(theBucket->localDepth < indexFile.globalDepth) {
                splitOnlyBucket(theBucket, pos);
                insertRecord(record);
                cout << "spliting " << record.code << " " << pos << endl;
            } else if(theBucket->localDepth == indexFile.globalDepth) {
                int posNextBucket = theBucket->nextBucket;
                int actualPosBucket = pos;
                Bucket* actualBucket = theBucket;
                // hasta ir al ultimo bucket chaining
                while(posNextBucket != -1) {
                    actualBucket = readBucket(posNextBucket);
                    actualPosBucket = posNextBucket;
                    posNextBucket = actualBucket->nextBucket;
                }
                // crear bucket si el actual esta lleno

                if(actualBucket->size >= actualBucket->capacity) {
                    Bucket newNextBucket;
                    newNextBucket.records[0] = record;
                    newNextBucket.size = 1;
                    int newBucketPos = writeBucket(&newNextBucket, -1);

                    actualBucket->nextBucket = newBucketPos;
                    writeBucket(actualBucket, actualPosBucket);
                    cout << "Insertando2: " << record.code << "  " << newBucketPos <<endl;
                } else{
                    actualBucket->records[actualBucket->size] = record;
                    actualBucket->size = actualBucket->size + 1;
                    writeBucket(actualBucket, actualPosBucket);
                    cout << "Insertando3: " << record.code << "  " << actualPosBucket <<endl;
                }

            }

        } else {
            cout << "k?" << endl;
        }
    }

    void printRecords() {
        set<int> bucketsPos;
        cout << "Buckets pos: " << endl;
        for(int i = 0; i < indexFile.size; i++) {
            bucketsPos.insert(indexFile.posBuckets[i]);
            cout << i << " : " << indexFile.posBuckets[i] << endl;
        }
        Bucket bucket;
        for(int i : bucketsPos) {
            cout << "i: " << i << endl;
            fileData.seekg(i, ios::beg);
            fileData.read((char*)&bucket, sizeof(Bucket));
            bucket.printBucket();

            while(bucket.nextBucket != -1) {
                cout << "i: " << bucket.nextBucket << endl;
                fileData.seekg(bucket.nextBucket, ios::beg);
                fileData.read((char*)&bucket, sizeof(Bucket));
                bucket.printBucket();
            }
        }

    }
    void load_csv(string filecsv) {
        ifstream file(filecsv, ios::in);
        if (!file.is_open()) {
            throw runtime_error("Csv file could not be opened");
        }
        string line;
        getline(file, line);
        cout << "Loading..." << endl;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            Record record;

            getline(ss, token, ';');
            record.code = stoi(token);

            getline(ss, token, ';');
            strncpy(record.track_name, token.c_str(), sizeof(record.track_name));
            record.track_name[sizeof(record.track_name) - 1] = '\0';

            getline(ss, token, ';');
            strncpy(record.artist_name, token.c_str(), sizeof(record.artist_name));
            record.artist_name[sizeof(record.artist_name) - 1] = '\0';

            getline(ss, token, ';');
            record.artist_count = stoi(token);

            getline(ss, token, ';');
            record.released_year = stoi(token);

            getline(ss, token, ';');
            record.released_month = stoi(token);


            getline(ss, token, ';');
            record.playlists = stol(token);

            getline(ss, token, ';');
            record.streams = stoll(token);

            getline(ss, token, ';');
            strncpy(record.key, token.c_str(), sizeof(record.key));
            record.key[sizeof(record.key) - 1] = '\0';

            getline(ss, token, ';');
            strncpy(record.mode, token.c_str(), sizeof(record.mode));
            record.mode[sizeof(record.mode) - 1] = '\0';

            getline(ss, token, ';');
            record.danceability = stoi(token);

            getline(ss, token, ';');
            strncpy(record.url, token.c_str(), sizeof(record.url));
            record.url[sizeof(record.url) - 1] = '\0';

            insertRecord(record);
        }
    }



};

#endif //EXTENSIBLE_H
