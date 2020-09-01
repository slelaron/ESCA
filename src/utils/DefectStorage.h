#ifndef DEFECT_STORAGE_H
#define DEFECT_STORAGE_H

#include <vector>
#include <string>
#include <set>

/// @class Синглтон класса в котором сохраняются все найденные утечки ресурсов
class DefectStorage
{
public:
    /// @brief Предоставляет единственный экземпляр
    static DefectStorage &Instance();

    /// @brief Добавляет утечку переменной в хранилище
    /// @param var - Имя переменной
    /// @param location - Место в коде программы, где происходит утечка
    void AddDefect( const std::string &var, const std::string &location );

    /// @brief Печатает все найденные утечки
    /// @param outputFile - Имя файла куда выводить ошибки, если пустое выводит в stdout
    void PrintDefects( const std::string &outputFile = "" );

    /// @brief Возвращает количество найденных утечек
    inline size_t GetCountDefects()
    {
        return defects.size();
    }

    DefectStorage( const DefectStorage &inst ) = delete;

    DefectStorage &operator=( const DefectStorage &rhs ) = delete;

private:
    DefectStorage() = default;

    std::set<std::string> defectsLocations;

    struct Defect
    {
        std::string varName;
        std::string location;

        bool operator>( Defect &other ) const
        {
            return location > other.location;
        }
        bool operator<( Defect &other ) const
        {
            return location < other.location;
        }
    };

    std::vector<Defect> defects;

};

#endif
