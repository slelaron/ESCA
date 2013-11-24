#ifndef ESCAASTVisitor_H
#define ESCAASTVisitor_H

#include <map>
#include <vector>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Decl.h>

#include "VersionedVariable.h"
#include "FSM.h"

class ASTWalker;

class ESCAASTVisitor : public clang::RecursiveASTVisitor<ESCAASTVisitor> 
{
	public:
		ESCAASTVisitor();
	public:
		bool VisitStmt(clang::Stmt *s);
		bool VisitBinaryOperator(clang::BinaryOperator* bo);
		bool shouldVisitTemplateInstantiations() const;
		bool VisitCXXOperatorCallExprs(clang::CXXOperatorCallExpr *e);
		bool VisitCXXConstructorDecl(clang::CXXConstructorDecl *c);
		bool VisitDeclRefExpr(clang::DeclRefExpr* expr);
		bool VisitVarDecl(clang::VarDecl *v);
		bool VisitTypedefDecl(clang::TypedefDecl *d);
		bool VisitFunctionDecl(clang::FunctionDecl *f);

	public:
		inline void SetWalker(ASTWalker *ast_walker) { walker = ast_walker; }

	private:
		bool ProcessAssignment(clang::BinaryOperator *binop);
		bool ProcessDeclaration(clang::VarDecl *vd);

	private:
		ASTWalker *walker;

		bool insideMain;

		//std::map<std::string, std::vector<VersionedVariable> > variables;
		std::map<std::string, int> variables;
		std::vector<VersionedVariable> allocated;
		FSM fsm;
};
#endif
