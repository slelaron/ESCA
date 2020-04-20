#ifndef ESCAASTVisitor_H
#define ESCAASTVisitor_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Decl.h>

#include "../target/Context.h"


class ESCAASTVisitor : public clang::RecursiveASTVisitor<ESCAASTVisitor>
{
public:
    ESCAASTVisitor() = default;

    /// @brief основная функция, неявно вызывается при проходе по AST дереву
    bool VisitFunctionDecl( clang::FunctionDecl *f );

    /// @brief Метод устанавливает пути которые следиет исключить из анализа AST дерева
    /// @param _paths - пути до директорий где хранятся библиотеки (#include<some_lib>)
    inline void SetExcludedPaths( const std::vector<std::string> &_paths )
    {
        excludedPaths = _paths;
    }

private:
    std::string getLocation( const clang::Stmt *st );

    void Reset();

    /// MAIN FUNCTION
    bool ProcessFunction( clang::FunctionDecl *f );

    bool ProcessStmt( clang::Stmt *stmt, bool = true );

    bool ProcessCompound( clang::CompoundStmt *body, bool );

    bool ProcessAssignment( clang::BinaryOperator *binop );

    bool ProcessDeclaration( clang::VarDecl *vd );

    bool ProcessDelete( clang::CXXDeleteExpr *del );

    bool ProcessReturn( clang::ReturnStmt *ret );

    bool ProcessIf( clang::IfStmt *ifstmt );

    bool ProcessReturnNone(); //Pointers are not returned.

    bool ProcessReturnPtr( clang::ReturnStmt *ret ); //Pointers are returned.

    /// @brief метод проверяет находится ли файл в исключенных директориях
    /// @param file - путь до файла который нужно проверить
    /// @return true - если файл внутри директории, false иначе
    bool IsInExcludedPath( const std::string &file );

private:
    Target::Context ctx;
    clang::SourceManager *currSM = nullptr;

    std::map<std::string, std::string> staticFuncMapping;

    ///@brief пути до системных библиотек и тех которые следует исключить из анализа
    std::vector<std::string> excludedPaths;

    //std::map<std::string, std::vector<VersionedVariable> > variables;
    //std::map<std::string, int> variables;

    // ������ �������
    std::map<std::string, PtrCounter> variables;

    std::vector<VersionedVariable> allocated;
    FSM fsm;

    FairLeafPredicate fairPred;
    BranchLeafPredicate branchPred;

    clang::Expr *returnExpr = nullptr;
    std::string returnVarName;
};

#endif
