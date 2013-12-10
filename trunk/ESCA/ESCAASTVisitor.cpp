#include <string>
#include <memory>

#include <llvm/Support/raw_ostream.h>
#include <clang/AST/PrettyPrinter.h>

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



bool ESCAASTVisitor::VisitFunctionDecl(FunctionDecl *f) 
{
	string funName = f->getNameInfo().getName().getAsString();
	if (funName != "main")
	{
		return false;
	}


	auto body = f->getBody();
	if (body != 0)
	{
		ProcessStmt(body);
		/*
		if (isa<CompoundStmt>(body))
		{
			ProcessCompound(cast<CompoundStmt>(body));
		}
		*/
	}
	

	insideMain = true;
    llvm::errs() << "Visiting function " << funName << "\n";
	f->dump();

    return true;
}

bool ESCAASTVisitor::ProcessStmt(clang::Stmt *stmt)
{
	if (isa<CompoundStmt>(stmt))
	{
		ProcessCompound(cast<CompoundStmt>(stmt));
	}
	if (isa<DeclStmt>(stmt))
	{
		llvm::errs() << "DeclStmt!\n";
		//(stmt)->dump();
		DeclStmt *d = cast<DeclStmt>(stmt);
		if (d)
		{
			d->dump();
			auto sd = d->getSingleDecl();
			if (sd)
			{
				if (isa<VarDecl>(sd))
				{
					llvm::errs() << "VarDecl!\n";
					ProcessDeclaration(cast<VarDecl>(sd));
				}
			}
			DeclGroupRef group = d->getDeclGroup();
			if (!group.isNull())
			{
				llvm::errs() << "DeclGroup\n";
				//TODO: do something!
			}
		}

	}
	if (isa<BinaryOperator>(stmt))
	{
		BinaryOperator *bo = cast<BinaryOperator>(stmt);
		if (bo->isAssignmentOp() == true) 
		{
			ProcessAssignment(bo);
		}

	}
	if (isa<CXXDeleteExpr>(stmt))
	{
		llvm::errs() << "delete!\n"; 
		ProcessDelete(cast<CXXDeleteExpr>(stmt));
	}
	if (isa<ReturnStmt>(stmt))
	{
		ProcessReturn(cast<ReturnStmt>(stmt));
	}
	if (isa<IfStmt>(stmt))
	{
		ProcessIf(cast<IfStmt>(stmt));
	}
	return true;
}

bool ESCAASTVisitor::ProcessCompound(clang::CompoundStmt *body)
{
	auto iter = body->body_begin();
	//body->
	for (; iter != body->body_end(); ++iter)
	{
		ProcessStmt(*iter);
	}
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
		//int lhsVer = ++variables[sname];
		PtrCounter &lhsCnt = variables[sname];
		int lhsVer = ++(lhsCnt.count);

		//std::vector<VersionedVariable> &vvvector = variables[sname];

		Stmt *rhs = binop->getRHS();
		if (isa<CXXNewExpr>(rhs))
		{
			CXXNewExpr *newOp = cast<CXXNewExpr>(rhs);
			StateFSM state;
			std::string type = "";
			VersionedVariable vv(type, sname, VAR_POINTER, lhsCnt.count);
			if (newOp->isArray())
			{
				vv.MetaType(VAR_ARRAY_POINTER);
				state.allocArrays.push_back(vv);
				lhsCnt.meta = VAR_ARRAY_POINTER;
			}
			else
			{
				state.allocPointers.push_back(vv);
				lhsCnt.meta = VAR_POINTER;
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
				//VariableSMT *lhsForm = new VariableSMT();
				shared_ptr<VariableSMT> lhsForm(new VariableSMT());
				lhsForm->Var(lhsVar);
				state.formulae.push_back(lhsForm);

				PtrCounter &rhsCnt = variables[rhsPointer];
				int rhsVer = rhsCnt.count;
				VersionedVariable rhsVar("", rhsPointer, VAR_POINTER, rhsVer);
				//VariableSMT *rhsForm = new VariableSMT();				
				shared_ptr<VariableSMT> rhsForm(new VariableSMT());
				rhsForm->Var(rhsVar);
				state.formulae.push_back(rhsForm);
				lhsCnt.meta = rhsCnt.meta;

				//shared_ptr<VariableSMT> leftForm(new VariableSMT);
				//leftForm->Var(lhsVar);
				//state.formulae.push_back(leftForm);
				shared_ptr<FormulaSMT> bs(new BinarySMT(lhsVar, rhsVar, EqualSMT, false));
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
		PtrCounter ptrCnt;
		ptrCnt.count = 0;
		ptrCnt.meta = VAR_POINTER;
		//variables[name] = ptrCnt;
		auto cntIter = variables.insert(pair<string, PtrCounter>(name, ptrCnt));

		auto init = vd->getAnyInitializer();
		if (init == 0)
		{
			return true;
		}
		if (!isa<CXXNewExpr>(init))
		{
			return 0;
		}

		++cntIter.first->second.count;

		auto newExpr = cast<CXXNewExpr>(init);
		

		StateFSM state;
		std::string type = "";
		VersionedVariable vv(type, name, VAR_POINTER, 1);

		if(newExpr->isArray()) //Declaration of array
		{
			vv.MetaType(VAR_ARRAY_POINTER);
			cntIter.first->second.meta = VAR_ARRAY_POINTER;
			state.allocArrays.push_back(vv);
		}
		else
		{
			state.allocPointers.push_back(vv);
		}
		allocated.push_back(vv);

		shared_ptr<VariableSMT> vvForm(new VariableSMT());
		vvForm->Var(vv);
		state.formulae.push_back(vvForm);

		fsm.AddStateToLeaves(state);
	}
	return true;
}

bool ESCAASTVisitor::ProcessDelete(clang::CXXDeleteExpr *del)
{
	auto argDel = del->getArgument();
	//TODO: change this if by function that gives a full name of pointer or null when there is no pointer.
	if (isa<ImplicitCastExpr>(argDel))
	{
		auto delCast = cast<ImplicitCastExpr>(argDel);
		auto dexpr = delCast->getSubExpr();
		if (isa<DeclRefExpr>(dexpr))
		{
			auto dptr = cast<DeclRefExpr>(dexpr);
			string name = dptr->getNameInfo().getAsString();
			//StateFSM state;
			std::string type = "";
			auto cntIter = variables.find(name);
			if (cntIter == variables.end())
			{
				llvm::errs() << "We delete undeclated variable!\n";
				return false;
			}
			VersionedVariable vv(type, name, cntIter->second.meta, cntIter->second.count);
			llvm::errs() << "isArrayForm: " << del->isArrayForm() << " isArrayFormAsWritten: " 
				<< del->isArrayFormAsWritten() << "\n";
			fsm.AddDeleteState(vv, del->isArrayForm());
		}
	}
	return true;
}

bool ESCAASTVisitor::ProcessReturn(clang::ReturnStmt *ret)
{
	auto retVal = ret->getRetValue();

	//TODO: check return value.
	return ProcessReturnNone(ret);
}

bool ESCAASTVisitor::ProcessReturnNone(clang::ReturnStmt *ret)
{
	fsm.ProcessReturnNone();
	return true;
}

bool ESCAASTVisitor::ProcessReturnPtr(clang::ReturnStmt *ret)
{
	auto retVal = ret->getRetValue();

	return true;
}

bool ESCAASTVisitor::ProcessIf(clang::IfStmt *ifstmt)
{
	llvm::errs() << "If statement!\n";

	auto cond = ifstmt->getCond();
	std::string typeS;
	llvm::raw_string_ostream lso(typeS);
	clang::LangOptions langOpts;
	langOpts.CPlusPlus11 = true;
	PrintingPolicy pol(langOpts);
	cond->printPretty(lso, 0, pol);
	string condStr = lso.str();
	llvm::errs() << "\tCondition: " << condStr << "\n";
	//TODO: Create the state branching.
	fsm.PushCondition(condStr);
	ProcessStmt(ifstmt->getThen());
	fsm.PopCondition();
	
	fsm.PushCondition("else - " + condStr);
	ProcessStmt(ifstmt->getElse());
	fsm.PopCondition();
	return true;
}
