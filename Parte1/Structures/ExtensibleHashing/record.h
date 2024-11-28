//
// Created by Usuario on 23/09/2024.
//

#ifndef RECORD_H
#define RECORD_H
#include <iostream>

struct Record {

    char track_name[100];
    char artist_name[50];
    int artist_count;
    int released_year;
    int released_month;
    long playlists;
    long long streams;
    char key[3];
    char mode[10];
    int danceability;
    char url[100];
    void printRecord() {
        std::cout << track_name << " | " << artist_name << " | " << artist_count << " | " << released_year << " | " <<
                released_month
                << " | " << playlists << " | " << streams << " | " << key << " | " << mode << " | " << danceability << " | " <<
                url << std::endl;
    }

};

#endif //RECORD_H
