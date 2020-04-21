#ifndef ESCAASTConsumer_h
#define ESCAASTConsumer_h

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclGroup.h>

#include "ESCAASTVisitor.h"


class ESCAASTConsumer : public clang::ASTConsumer
{
public:
    /// @brief Переопределяемая функция для прохода по AST дереву нашим AST visitor
    bool HandleTopLevelDecl( clang::DeclGroupRef DR ) override
    {
        for( auto it : DR )
        {
            // Traverse the declaration using our AST visitor.
            visitor.TraverseDecl(it);
        }

        return true;
    }

    /// @brief Запоминаем пути для AST visitor которые следует проигнорировать
    inline void SetExcludedPaths( const std::vector<std::string> &path )
    {
        visitor.SetExcludedPaths(path);
    }

    Target::Context GetContext()
    {
        return visitor.getContext();
    }

private:
    ESCAASTVisitor visitor;
};

#endif
