// тестирование исключений 3 ошибки
// ESCA - 3 нашел 0 ложных
// cppcheck - 1 нашел 0 ложных
// pvs - 1 нашел 0 ложных

#include <stdexcept>


// cppcheck + | ESCA + | PVS +
void test1() {
    try {
        auto x = new char[10];
        throw std::logic_error("exc");
    }
    catch( const std::exception &e ) {
        throw;
    }
}

// нет ошибки
void test2() {
    char *x = nullptr;
    try {
        x = new char[2];
        throw std::logic_error("exc");
    }
    catch( const std::exception &e ) {
        delete[] x;
        throw;
    }
}

// cppcheck - | ESCA +
void test3() {
    int x = rand();
    auto a = new char[10];
    try {
        char ch = a[ 9 ];
        if( x > 5 ) {
            x = (int) ch + 2;
            throw std::logic_error("asfd" + std::to_string(x));
        }
        delete[] a;
    }
    catch( const std::exception &e ) {
//        std::cerr << e.what();
        throw;
    }
}


// cppcheck - | ESCA +
void test4() {
    int *x = new int[10];
    try {
        int z = rand();
        z += x[ 0 ];
        if( z ) {
            throw std::runtime_error("ww");
        }
    }
    catch( std::logic_error &le ) {
        delete[] x; // очистка не в том месте
        le.what();
    }
    catch( std::runtime_error &re ) {
    }
    catch( std::exception &e ) {
    }
    catch( ... ) {
    }
}

int main() {
    test1();
    test2();
    return 0;
}