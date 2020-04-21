#include <cstdio>
#include <iostream>
#include <filesystem>
#include <vector>



void foo1( int &x, int y )
{
    int *a = new int(2);
    *a = x + y;
    int *b = new int(*a);
    x = *b;
    delete a;
}

void foo2()
{
// PVS не нашла +
    int *x = new int;
    int *y = x;
}

void getPath( char **p )
{
    *p = const_cast<char *>("fasdf");
}

FILE *openFile()
{
// PVS и Cppcheck не нашли
    char *path = new char[256];
    getPath(&path);
    return fopen(path, "r");
}

int *creatVal()
{
    auto *p = new int(42);
    return p;
}

int *create( int x1 )
{
// clang static analyzer не нашел
    int *v1 = creatVal();
    int x2 = *v1;
    int *v2 = new int(111);
    *v1 = *v2;
    std::cout << x1 << " " << x2 << std::endl;
    std::cout << v1 << " " << v2 << std::endl;
    return v2;
}

int getValue( int x )
{
// никто из аналогов не нашел
    int *a = create(x);
    if( *a > 10 )
        *a = 10;
    return *a;
}

void foo3( int a )
{
    int *b = new int(a);
    if( a > 5 )
    {
        delete b;
    }
}

void foo4( int a )
{
    int *b = new int(123);
    return;
    delete b;
}

void foo5( int a )
{
    int *b = nullptr;
    if( a == 1 )
    {
        b = new int(123);
    }
    if( a == 1 )
    {
        delete b;
    }
}

void foo6( int a )
{
    // не нашел
    int *b[10];
    for( int i = 0; i < 10; ++i )
    {
        b[ i ] = new int(i);
    }
    for( int i = 0; i < 9; ++i )
    {
        delete b[ i ];
    }
}

int *f()
{
    int *a = new int(123);
    return a;
}

void bar( int *a )
{
    if( *a > 50 )
    {
        delete a;
    }
}

int foo7()
{
    int *a = f();
    bar(a);
    return 123;
}

std::vector<char *> dd;

void foo8()
{
    dd.resize(10);
    dd[ 1 ] = new char[10];
    auto a = new char[10];
    dd[1][0] = a[1];
    try
    { // никто не нашел
        if( !dd.empty())
            throw std::logic_error("e");
        delete[] dd[ 1 ];
        delete[] a;
    }
    catch( const std::exception &e )
    {
        throw;
    }
}

int main( int argc, char *argv[] )
{
    auto f = openFile();
    fclose(f);
    foo1(argc, 10);
    foo2();
    foo3(argc);
    foo4(argc);
    foo5(argc);
    foo6(argc);
    int x = foo7();
    int y = getValue(argc);
    int z = getValue(1);
    x = 1 - z + x;
    foo8();
    return x + y;
}