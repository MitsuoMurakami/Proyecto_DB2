#include <iostream>
// #include "parser.h"
#include "extensibleCars.h"




int main() {

    // Parser parser;
    // parser.executeSQL("CREATE TABLE table1 FROM FILE data.csv USING INDEX EXT;");
    // parser.executeSQL("SELECT * FROM table1 WHERE key = Bite Me;");
    // extendible.load_csv("data.csv");

    ExtendibleHashing ex("dataRecordC.dat", "dataIndexC.dat");
    ex.load_csv("cars2.csv");
    // ex.measureTimes();
    auto res = ex.getAll();
    for(auto it : res) {
        it.printRecord();
    }
    return 0;
}
