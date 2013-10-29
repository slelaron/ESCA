#include <string>

#include <llvm/Support/raw_ostream.h>

#include "ESCAASTVisitor.h"
#include "ASTWalker.h"

using namespace clang;
using namespace std;

bool ESCAASTVisitor::VisitStmt(clang::Stmt *s)
{
		llvm::errs() << "Visiting statement\n";  
		walker->DumpStmt(s);
		
		if (clang::isa<clang::Expr>(s))
		{
			llvm::errs() << "\t Expression";

			//s->printPretty(llvm::errs, 
			if (clang::isa<clang::BinaryOperator>(s)) 
			{
				llvm::errs() << ": binary operator: ";

				if (cast<BinaryOperator>(s)->isAssignmentOp() == true) 
				{
				  // blablabla
					llvm::errs() << "assignment \n";
				}
			}
			if (isa<clang::CXXNewExpr>(s))
			{
				llvm::errs() << ": new operator: ";
				CXXNewExpr *newExpr = cast<CXXNewExpr>(s);
				if (newExpr->isArray())
				{
					QualType qt = newExpr->getAllocatedType();
					Expr *size = newExpr->getArraySize();

					
					llvm::errs() << "array of " << qt.getAsString();

					//size->dump( (errs);
					//if (size->isConstantInitializer())
					{

					}
					
				}
			}
		}
        return true;
}

bool ESCAASTVisitor::VisitBinaryOperator(BinaryOperator* bo) 
	{
        if (bo->isAssignmentOp() == true) 
		{
            llvm::errs() << "Visiting assignment ";
            Expr *LHS;
            LHS = bo->getLHS();
            DeclRefExpr* dre;
            if ((dre = dyn_cast<DeclRefExpr>(LHS))) { 
                string name = (dre->getNameInfo()).getName().getAsString();
                llvm::errs() << "to " << name;
            }
            if (ArraySubscriptExpr* ase = dyn_cast<ArraySubscriptExpr>(LHS)) { 
                Expr *arrayBase = ase->getBase()->IgnoreParenCasts();
                if ((dre = dyn_cast<DeclRefExpr>(arrayBase))) { 
                    string name = (dre->getNameInfo()).getName().getAsString();
                    llvm::errs() << "to array " << name;
                }
            }
            llvm::errs() << "\n";
        }
        return true;
    }

bool ESCAASTVisitor::shouldVisitTemplateInstantiations() const { 
    llvm::errs() << "PIPPOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << "\n";
    return true; }

bool ESCAASTVisitor::VisitCXXOperatorCallExprs(CXXOperatorCallExpr *e) {
    llvm::errs() << "Visiting cxxoperatorcall" << "\n";
    return true;
}

bool ESCAASTVisitor::VisitCXXConstructorDecl(CXXConstructorDecl *c) {
    llvm::errs() << "Visiting CXXConstructorDecl" << "\n";
    return true;        
}

bool ESCAASTVisitor::VisitDeclRefExpr(DeclRefExpr* expr) {
    string name = (expr->getNameInfo()).getName().getAsString();
    llvm::errs() << name << "\n";
    return true;
}

bool ESCAASTVisitor::VisitVarDecl(VarDecl *v) 
{
    llvm::errs() << "Visiting declaration of variable " << v->getDeclName().getAsString() << "\n";
    llvm::errs() << "  type: " << v->getTypeSourceInfo()->getType().getTypePtr()->getTypeClassName();
    if (v->getTypeSourceInfo()->getType().getTypePtr()->isFloatingType() == true) 
	{
        llvm::errs() << " -> float";
    }
    if(v->getTypeSourceInfo()->getType().getTypePtr()->isConstantArrayType() == true) 
	{
        llvm::errs() << " of ";
        llvm::errs() << v->getTypeSourceInfo()->getType().getAsString();
        llvm::errs() << " size ";
        llvm::APInt arraySize = cast<ConstantArrayType>(v->getTypeSourceInfo()->getType().getTypePtr())->getSize();
        llvm::errs() << arraySize;
    }
    if(v->getTypeSourceInfo()->getType().getTypePtr()->isPointerType() == true) 
	{
        llvm::errs() << " to " << v->getTypeSourceInfo()->getType().getAsString();

    }
    llvm::errs() << "\n";
    return true;
}

bool ESCAASTVisitor::VisitTypedefDecl(clang::TypedefDecl *d) 
{
    llvm::errs() << "Visiting " << d->getDeclKindName() << " " << d->getName() << "\n";

    return true; // returning false aborts the traversal        
}

bool ESCAASTVisitor::VisitFunctionDecl(FunctionDecl *f) 
{
    llvm::errs() << "Visiting function " << f->getNameInfo().getName().getAsString() << "\n";
	f->dump();

    return true;
}
