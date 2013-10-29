#ifndef ESCAASTVisitor_H
#define ESCAASTVisitor_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Decl.h>

class ASTWalker;

class ESCAASTVisitor : public clang::RecursiveASTVisitor<ESCAASTVisitor> 
{
	public:
		ESCAASTVisitor() : walker(0) {}
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
		ASTWalker *walker;
};
#endif
