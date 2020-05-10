#ifndef ESCA_COMMON_H
#define ESCA_COMMON_H

#include <set>
#include <vector>

class Options
{
public:

    Options( Options const & ) = delete;

    Options &operator=( Options const & ) = delete;

    static Options &Instance()
    {
        static Options instance;
        return instance;
    }

    /// @brief Флаг быстроты анализа, true - быстро, false - медленно, но досканально
    bool needFast;

    /// @brief Основной файл который сейчас анализируется
    std::string analyzeFile;

    /// @brief Метод устанавливает пути (которые следиет исключить из анализа AST дерева)
    /// @param _paths - пути до директорий где хранятся библиотеки (#include<some_lib>)
    void setIncludeDirs( const std::vector<std::string> &inclPaths );

    /// @brief метод проверяет находится ли файл в include директориях
    /// @param file - путь до файла который нужно проверить
    /// @return true - если файл внутри директории, false иначе
    bool isInIncludeDirs( const std::string &path );


private:
    std::vector<std::string> includeDirs;

    Options()
    {
        needFast = false;
    };

    static Options *pInstance;

};


#endif //ESCA_COMMON_H
