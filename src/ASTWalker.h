#ifndef ASTWalker_h
#define ASTWalker_h

#include <string>

#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <clang/Lex/HeaderSearchOptions.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/ASTContext.h>

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

	void WalkAST(const std::string& path);

	void DumpStmt(clang::Stmt* s);

protected:
	virtual void SetIncludeDirectories();

protected:
	std::shared_ptr<clang::HeaderSearchOptions> headerSearchOptions;
	clang::ASTContext* astContext;
	ESCAASTConsumer* astConsumer;
};

#endif
