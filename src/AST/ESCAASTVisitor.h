#ifndef ESCAASTVisitor_H
#define ESCAASTVisitor_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Decl.h>
#include <z3++.h>


/// @class Класс, который выполняет предварительный обход всего Clang AST дерева и посещает каждый узел.
class ESCAASTVisitor : public clang::RecursiveASTVisitor<ESCAASTVisitor>
{
public:
    ESCAASTVisitor() = default;

    /// @brief Метод для посещения функции
    /// @param f - основная функция
    bool VisitFunctionDecl( clang::FunctionDecl *f );

private:
    /// @brief Получение сторки с названием файла и номером строки в нем, где находится состояние
    /// @param st - состояние
    std::string getLocation( const clang::Stmt *st );

    /// @brief Метод для посещения функции
    /// @param f - основная функция
    bool ProcessFunction( clang::FunctionDecl *f );

    /// @brief Проверяет тип состояния (stmt) и выполняет действие в зависимости от него
    /// @param stmt - состояние которое нужно проверить
    /// @param addToState - нужно ли сохранять в контекст
    bool ProcessStmt( clang::Stmt *stmt, bool addToState = true );

    bool ProcessCompound( clang::CompoundStmt *body, bool );

    bool ProcessDeclaration( clang::VarDecl *vd );

    /// @brief Метод анализирует операцию присваивания
    /// @param binop - состояние операции присваивания
    bool ProcessBinOp( clang::BinaryOperator *binop );

    bool ProcessDelete( clang::CXXDeleteExpr *del );

    bool ProcessFree( clang::CallExpr *rhsRef );

    bool ProcessReturn( clang::ReturnStmt *ret );

    bool ProcessIf( clang::IfStmt *ifstmt );

    //    bool ProcessReturnNone(); //Pointers are not returned.

    //    bool ProcessReturnPtr( clang::ReturnStmt *ret ); //Pointers are returned.


private:
    void
    AddVarDeclFromFoo( const std::string &varName, std::string &fooName, const std::string &location, bool isDecl );

    /// @brief Метод анализирует операцию присваивания
    /// @param init - инициализатор переменной
    /// @param lhsName - имя алоцируемой переменной
    /// @param isDecl - проделорирована ли переменная
    bool ProcessAssignment( const clang::Stmt *init, const std::string &lhsName, bool isDecl );

    bool EvaluateBool( const clang::Stmt *init, bool &res );

    clang::ASTContext *astContext;
    /// @brief Менеджер для получения строки в текущей функции
    clang::SourceManager *currSM = nullptr;

    std::map<std::string, std::string> staticFuncMapping;

    /// @brief Хранилище переменных
    std::set<std::string> allVariables;

//    std::set<std::string> localVariables;

    std::map<std::string, bool> variableToExpr;

    z3::context z3contex;


};

#endif
