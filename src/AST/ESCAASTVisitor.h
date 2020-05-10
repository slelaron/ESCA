#ifndef ESCAASTVisitor_H
#define ESCAASTVisitor_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Decl.h>

//#include "../SMT/Variable.h"

class ESCAASTVisitor : public clang::RecursiveASTVisitor<ESCAASTVisitor>
{
public:
    ESCAASTVisitor() = default;

    /// @brief основная функция, неявно вызывается при проходе по AST дереву
    bool VisitFunctionDecl( clang::FunctionDecl *f );

private:
    /// @brief Получение сторки с названием файла и номером строки в нем, где находится состояние
    /// @param st - состояние
    std::string getLocation( const clang::Stmt *st );

    /// MAIN FUNCTION
    bool ProcessFunction( clang::FunctionDecl *f );

    /// @brief Проверяет тип состояния (stmt) и выполняет действие в зависимости от него
    /// @param stmt - состояние которое нужно проверить
    /// @param addToState - нужно ли сохранять в контекст
    bool ProcessStmt( clang::Stmt *stmt, bool addToState = true );

    bool ProcessCompound( clang::CompoundStmt *body, bool );

    bool ProcessAssignment( clang::BinaryOperator *binop );

    bool ProcessDeclaration( clang::VarDecl *vd );

    bool ProcessDelete( clang::CXXDeleteExpr *del );

    bool ProcessFree( clang::CallExpr *rhsRef );

    bool ProcessReturn( clang::ReturnStmt *ret );

    bool ProcessIf( clang::IfStmt *ifstmt );

    //    bool ProcessReturnNone(); //Pointers are not returned.

    //    bool ProcessReturnPtr( clang::ReturnStmt *ret ); //Pointers are returned.


private:

    /// @brief Нужен для получения строки в текущей функции
    clang::SourceManager *currSM = nullptr;

    std::map<std::string, std::string> staticFuncMapping;

    /// @brief Хранилище переменных
    std::set<std::string> variables;

};

#endif
