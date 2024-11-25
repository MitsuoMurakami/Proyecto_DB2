#include <iostream>
// #include "parser.h"


#include "extensibleCars.h"




int main() {

    Parser parser;

    // parser.executeSQL("CREATE TABLE table1 FROM FILE data.csv USING INDEX EXT;");
    // SELECT * FROM table1;
    // SELECT * FROM table1 WHERE key = Bite Me;
    // INSERT INTO table1 VALUES ('MyTrack', 'Diego', 1, 2024, 9, 16, 1000, 'A', 'Major', 50, 'diego.com');
    // SELECT * FROM table1 WHERE key = MyTrack;
    // DELETE FROM table1 WHERE key = MyTrack;
    // SELECT * FROM table1 WHERE key = MyTrack;

    // CREATE TABLE table1 FROM FILE cars2.csv USING INDEX EXT;
    // SELECT * FROM table1 WHERE key = Porsche 911 2021 Carrera Cabriolet;
    // INSERT INTO table1 VALUES ('MyCar', '20000 Soles', 4, 6, 300, 100, '4 Seater', 'MyBrand', 'Peru');
    // SELECT * FROM table1 WHERE key = MyCar;
    // DELETE FROM table1 WHERE key = MyCar;
   string sql;
    while (true) {
        cout << "> ";
        getline(cin, sql);
        if (sql == "exit") {
            break;
        }
        // parser.executeSQL(sql);
        parser.executeSQL(sql);
    }

    return 0;
}
