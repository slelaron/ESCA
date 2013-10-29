#ifndef ASTWalker_h
#define ASTWalker_h

#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <clang/Lex/HeaderSearchOptions.h>
#include <clang/AST/Stmt.h>

namespace clang
{
	class ASTContext;
}
class ESCAASTConsumer;


class ASTWalker
{
	public:
		ASTWalker();
		~ASTWalker();
		void WalkAST();

	public:
		void DumpStmt(clang::Stmt *s);

	protected:
		virtual void SetIncludeDirectories();

	protected:
		llvm::IntrusiveRefCntPtr<clang::HeaderSearchOptions> headerSearchOptions;
		clang::ASTContext *astContext;
		ESCAASTConsumer *astConsumer;
};

#endif
