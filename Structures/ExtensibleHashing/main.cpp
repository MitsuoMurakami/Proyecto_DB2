#include <iostream>
#include "extensible.h"


int main() {
    ExtendibleHashing extendible("dataRecord.dat", "dataIndex.dat");
    extendible.load_csv("datacsv.csv");
    extendible.printRecords();

    return 0;
}
