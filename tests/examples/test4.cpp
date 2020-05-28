// тестирование исключений в конструкторе и деструкторе

#include <stdexcept>

class Database {
    int *x = new int[1];
public:
    void clear() {
        delete[] x;
    };
};

// исключение в деструкторе
// ESCA нашел PVS и cppcheck нет
class AA {
    Database *db;
    int *res = nullptr;
public:
    AA() : db(new Database()) {
        res = new int[10];
    }

    void doSome() {
        // doSome
        delete db;
        db = nullptr;
    }

    ~AA() {
        clearDB();
        delete db;
        delete[]res;
    }

private:
    void clearDB() {
        if( !db ) {
            throw std::runtime_error("no databse");
        }
        db->clear();
    }
};

// pvs и cppcheck ESCA нашли
class SomeBadClass {
    int *x;
    bool thr;
public:
    SomeBadClass() {
        x = new int[10];
    }

    ~SomeBadClass() {
        if( thr ) {
            throw std::runtime_error("sdsf");
        }
        delete[] x;
    }
};

int main() {
    AA *a = new AA();
    a->doSome();
    delete a;
    SomeBadClass s;
}