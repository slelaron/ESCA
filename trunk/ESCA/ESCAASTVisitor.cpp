#include <string>

#include <llvm/Support/raw_ostream.h>

#include "ESCAASTVisitor.h"
#include "ASTWalker.h"

#include "Variable.h"

#include "FormulaSMT.h"
#include "BinarySMT.h"
#include "VariableSMT.h"

using namespace clang;
using namespace std;

ESCAASTVisitor::ESCAASTVisitor() : walker(0), insideMain(false)
{
}

bool ESCAASTVisitor::VisitStmt(clang::Stmt *s)
{
	if (!insideMain)
	{
		return false;
	}

	llvm::errs() << "Visiting statement\n";  
	walker->DumpStmt(s);
		
	if (clang::isa<clang::Expr>(s))
	{
		llvm::errs() << "\t Expression ";

		//s->printPretty(llvm::errs, 
		if (clang::isa<clang::BinaryOperator>(s)) 
		{
			llvm::errs() << ": binary operator: ";
			BinaryOperator *binop = cast<BinaryOperator>(s);
			if (cast<BinaryOperator>(s)->isAssignmentOp() == true) 
			{
				ProcessAssignment(binop);
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
	if (!insideMain)
	{
		return false;
	}

    if (bo->isAssignmentOp() == true) 
	{
        llvm::errs() << "Visiting assignment ";
        Expr *LHS;
        LHS = bo->getLHS();
        DeclRefExpr* dre;
        if ((dre = dyn_cast<DeclRefExpr>(LHS))) 
		{ 
            string name = (dre->getNameInfo()).getName().getAsString();
            llvm::errs() << "to " << name;
        }
        if (ArraySubscriptExpr* ase = dyn_cast<ArraySubscriptExpr>(LHS)) 
		{ 
            Expr *arrayBase = ase->getBase()->IgnoreParenCasts();
            if ((dre = dyn_cast<DeclRefExpr>(arrayBase))) 
			{ 
                string name = (dre->getNameInfo()).getName().getAsString();
                llvm::errs() << "to array " << name;
            }
        }
        llvm::errs() << "\n";
    }
    return true;
}

bool ESCAASTVisitor::shouldVisitTemplateInstantiations() const 
{
	if (!insideMain)
	{
		return false;
	}

    llvm::errs() << "PIPPOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << "\n";
    return true; 
}

bool ESCAASTVisitor::VisitCXXOperatorCallExprs(CXXOperatorCallExpr *e) 
{
	if (!insideMain)
	{
		return false;
	}

	llvm::errs() << "Visiting cxxoperatorcall" << "\n";
    return true;
}

bool ESCAASTVisitor::VisitCXXConstructorDecl(CXXConstructorDecl *c) 
{
	if (!insideMain)
	{
		return false;
	}

    llvm::errs() << "Visiting CXXConstructorDecl" << "\n";
    return true;        
}

bool ESCAASTVisitor::VisitDeclRefExpr(DeclRefExpr* expr) 
{
	if (!insideMain)
	{
		return false;
	}

    string name = (expr->getNameInfo()).getName().getAsString();
    llvm::errs() << name << "\n";
    return true;
}

bool ESCAASTVisitor::VisitVarDecl(VarDecl *v) 
{
	if (!insideMain)
	{
		return false;
	}

	return ProcessDeclaration(v);

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
    //llvm::errs() << "Visiting " << d->getDeclKindName() << " " << d->getName() << "\n";

    return true; // returning false aborts the traversal        
}

bool ESCAASTVisitor::VisitFunctionDecl(FunctionDecl *f) 
{
	string funName = f->getNameInfo().getName().getAsString();
	if (funName != "main")
	{
		return false;
	}

	insideMain = true;
    llvm::errs() << "Visiting function " << funName << "\n";
	f->dump();

    return true;
}

bool ESCAASTVisitor::ProcessAssignment(clang::BinaryOperator *binop)
{
	// blablabla
	llvm::errs() << "assignment \n";

	Stmt *lhs = binop->getLHS();
	if (isa<DeclRefExpr>(lhs))
	{
		llvm::errs() << "\tto reference ";
		DeclRefExpr *ref = cast<DeclRefExpr>(lhs);
		DeclarationNameInfo nameInfo = ref->getNameInfo();
		DeclarationName name = nameInfo.getName();
		//name.dump();
		string sname = name.getAsString();
		llvm::errs() << sname;
					
		/*
		if (variables.find(sname) == variables.end())
		{
			VersionedVariable vv(sname, "", VAR_POINTER, 0);
			std::vector<VersionedVariable> vvvector;
			vvvector.push_back(vv);
			variables[sname] = vvvector;
			//variables.insert(
		}
		*/
		int lhsVer = ++variables[sname];

		//std::vector<VersionedVariable> &vvvector = variables[sname];

		Stmt *rhs = binop->getRHS();
		if (isa<CXXNewExpr>(rhs))
		{
			CXXNewExpr *newOp = cast<CXXNewExpr>(rhs);
			StateFSM state;
			std::string type = "";
			VersionedVariable vv(type, sname, VAR_POINTER, variables[sname]);
			if (newOp->isArray())
			{
				vv.MetaType(VAR_ARRAY_POINTER);
				state.allocArrays.push_back(vv);
			}
			else
			{
				state.allocPointers.push_back(vv);
			}
			//־עלועטעü new.
			allocated.push_back(vv);
			fsm.AddStateToLeaves(state);
		}
		if (isa<ImplicitCastExpr>(rhs))
		{
			ImplicitCastExpr *ice = cast<ImplicitCastExpr>(rhs);
			Stmt *subexpr = ice->getSubExpr();
			if (isa<DeclRefExpr>(subexpr)) //pointer
			{
				auto rhsRefExpr = cast<DeclRefExpr>(subexpr);
				string rhsPointer = rhsRefExpr->getNameInfo().getName().getAsString();
				StateFSM state;
				VersionedVariable lhsVar("", sname, VAR_POINTER, lhsVer);
				VariableSMT *lhsForm = new VariableSMT();
				lhsForm->Var(lhsVar);
				state.formulae.push_back(lhsForm);

				int rhsVer = variables[rhsPointer];
				VersionedVariable rhsVar("", rhsPointer, VAR_POINTER, rhsVer);
				VariableSMT *rhsForm = new VariableSMT();
				rhsForm->Var(rhsVar);
				state.formulae.push_back(rhsForm);

				FormulaSMT *bs = new BinarySMT(lhsVar, rhsVar, EqualSMT, false);
				state.formulae.push_back(bs);

				fsm.AddStateToLeaves(state);
				//AddToSolver(sname, variables[sname], ...);
			}

		}

		//ref->get
	}
	return true;
}

bool ESCAASTVisitor::ProcessDeclaration(clang::VarDecl *vd)
{
	llvm::errs() << "\n\n";
	vd->dump();
	llvm::errs() << "isBlockPointerType: " 
	<< vd->getTypeSourceInfo()->getType().getTypePtr()->isBlockPointerType() 
	<< "\n";

	llvm::errs() << "isAggregateType: " 
		<< vd->getTypeSourceInfo()->getType().getTypePtr()->isAggregateType() 
	<< "\n";

	llvm::errs() << "isAnyComplexType: " 
		<< vd->getTypeSourceInfo()->getType().getTypePtr()->isAnyComplexType() 
	<< "\n";

	llvm::errs() << "isArrayType: " 
		<< vd->getTypeSourceInfo()->getType().getTypePtr()->isArrayType() 
	<< "\n";

	llvm::errs() << "isDependentSizedArrayType: " 
		<< vd->getTypeSourceInfo()->getType().getTypePtr()->isDependentSizedArrayType() 
	<< "\n";

	llvm::errs() << "isIntegerType: " 
		<< vd->getTypeSourceInfo()->getType().getTypePtr()->isIntegerType() 
	<< "\n";

	llvm::errs() <<"type: " << vd->getTypeSourceInfo()->getType().getAsString();

	llvm::errs() << "\n\n";

	if (vd->getTypeSourceInfo()->getType().getTypePtr()->isAnyPointerType())
	{
		auto name = vd->getNameAsString();
		auto iter = variables.find(name);
		if (iter != variables.end())
		{
			//Error: this variable is already declared.
			llvm::errs() << "Variable with name " << name << " declared twice\n";
			return true;
		}
		variables[name] = 1;

		auto init = vd->getAnyInitializer();
		if (init == 0)
		{
			return true;
		}
		if (!isa<CXXNewExpr>(init))
		{
			return 0;
		}

		auto newExpr = cast<CXXNewExpr>(init);
		

		StateFSM state;
		std::string type = "";
		VersionedVariable vv(type, name, VAR_POINTER, 1);

		if(newExpr->isArray()) //Declaration of array
		{
			vv.MetaType(VAR_ARRAY_POINTER);
			state.allocArrays.push_back(vv);
		}
		else
		{
			state.allocPointers.push_back(vv);
		}
		allocated.push_back(vv);
		fsm.AddStateToLeaves(state);
	}
	return true;
}
