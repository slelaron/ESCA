#include <string>
#include <map>
#include <vector>
#include <set>

#include <llvm/Support/raw_ostream.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/ASTContext.h>
#include <iostream>
#include <common.h>

#include "../target/Context.h"

#include "ESCAASTVisitor.h"

#include "Output.h"


bool ESCAASTVisitor::VisitFunctionDecl( clang::FunctionDecl *f )
{
    return ProcessFunction(f);
}


std::string ESCAASTVisitor::getLocation( const clang::Stmt *st )
{
    auto loc = st->getBeginLoc();
    return loc.printToString(*currSM);
}

bool ESCAASTVisitor::ProcessFunction( clang::FunctionDecl *f )
{
    std::string funName = f->getNameInfo().getName().getAsString();

    ////////////
    auto loc = f->getLocation();
    clang::SourceManager &sm = f->getASTContext().getSourceManager();
    currSM = &sm;
    auto lstr = loc.printToString(sm);
//    clang::SourceLocation::getFromPtrEncoding(loc.getPtrEncoding()).print(llvm::nulls(), sm);

    Cout << "Location:\n";
    Cout << "\tloc.getRawEncoding(): " << loc.getRawEncoding() << " loc.isFileID(): " << loc.isFileID() << "\n";
    Cout << "\ngetFromPtrEncoding dump:\n";
    Cout << "\nlocation string: " << lstr << "\n";

    if( Options::Instance().needFast && lstr.find(Options::Instance().analyzeFile) == std::string::npos )
    {
        return true;
    }

    if( !Options::Instance().needFast && Options::Instance().isInIncludeDirs(lstr))
    {
        return true;
    }

//    std::cout << lstr << std::endl;

    /////////

//    std::string funName = f->getName();
    f->dump();

    auto body = f->getBody();
    if( body != nullptr )
    {
        if( !f->isGlobal())
        {
            static int num = 0;
            staticFuncMapping[ funName ] = funName + std::to_string(num);

            funName = funName + std::to_string(num);
            ++num;
        }

        if( funName == "main" )
        {
            static int numMain = 0;
            funName = funName + "$" + std::to_string(numMain);
            ++numMain;
        }

        Target::Context::Instance().AddFunction(funName);

        /// MAIN FUNCTION
        ProcessStmt(body);

        // lazy
        //ProcessReturnNone();
        variables.clear();
    }

    return true;
}


bool ESCAASTVisitor::ProcessStmt( clang::Stmt *stmt, bool addToStates )
{
    if( !stmt )
    {
        return true;
    }

    Cout << "~~Processing statement~~\n";
    //stmt->dump();
    using namespace clang;

    if( auto compSt = dyn_cast<CompoundStmt>(stmt))
    {
        ProcessCompound(compSt, addToStates);
    }
    else if( auto declSt = dyn_cast<DeclStmt>(stmt))
    {
        Cout << "DeclStmt!\n";
        if( declSt->isSingleDecl())
        {
            auto sd = declSt->getSingleDecl();
            if( sd )
            {
                if( auto varSt = dyn_cast<VarDecl>(sd))
                {
                    Cout << "VarDecl!\n";
                    ProcessDeclaration(varSt);
                }
            }
        }
        else
        {
            DeclGroupRef group = declSt->getDeclGroup();
            if( !group.isNull())
            {
                Cout << "DeclGroup\n";
                //TODO: do something!
            }
        }
    }
    else if( isa<BinaryOperator>(stmt))
    {
        auto bo = cast<BinaryOperator>(stmt);
        if( bo->isAssignmentOp())
        {
            ProcessAssignment(bo);
        }
    }
    else if( auto delSt = dyn_cast<CXXDeleteExpr>(stmt))
    {
        Cout << "delete!\n";
        ProcessDelete(delSt);
    }
    else if( auto retSt = dyn_cast<ReturnStmt>(stmt))
    {
        ProcessReturn(retSt);
    }
    else if( auto ifSt = dyn_cast<IfStmt>(stmt))
    {
        ProcessIf(ifSt);
    }
    else if( auto castSt = dyn_cast<ParenExpr>(stmt))
    {
        auto sub = castSt->getSubExpr();
        if( auto bo = dyn_cast<BinaryOperator>(sub))
        {
            if( bo->isAssignmentOp())
            {
                ProcessAssignment(bo);
            }
        }
    }
    else if( auto tr = dyn_cast<CXXTryStmt>(stmt))
    {
//        std::cout << "try expr" << std::endl;
//        tr->dump();
        if( auto trBlock = dyn_cast<CompoundStmt>(tr->getTryBlock()))
        {
            ProcessCompound(trBlock, addToStates);
        }

    }
    else if( auto exWithClUp = dyn_cast<ExprWithCleanups>(stmt))
    {
        auto thr = exWithClUp->children().begin();
        if( auto throws = dyn_cast<CXXThrowExpr>(*thr))
        {
            // TODO: переделать, добавить обработку throw
//            std::cout << "throw expr" << std::endl;
//            throws->dump();
        }
    }
    else if( auto cycle = dyn_cast<WhileStmt>(stmt))
    {
//        std::cout << "while" << std::endl;
        ProcessStmt(cycle->getBody());
    }
    else if( auto cycle2 = dyn_cast<ForStmt>(stmt))
    {
//        std::cout << "while" << std::endl;
        ProcessStmt(cycle2->getBody());
    }
    else if( auto rhsRefExpr = dyn_cast<CallExpr>(stmt))
    {
        ProcessFree(rhsRefExpr);
    }

    return true;
}

bool ESCAASTVisitor::ProcessFree( clang::CallExpr *rhsRefExpr )
{
    using namespace clang;

    auto callee = rhsRefExpr->getCallee();
    //callee->dump();
    if( auto subExpr = dyn_cast<ImplicitCastExpr>(callee))
    {
        if( auto foo = dyn_cast<DeclRefExpr>(subExpr->getSubExpr()))
        {
            std::string fooName = foo->getNameInfo().getName().getAsString();
// TODO: make set of free_functions
#ifdef __unix__
            if( fooName == "free" || fooName == "close" || fooName == "fclose" )
#else
                if( fooName == "free" || fooName == "closesoket" || fooName == "fclose")
#endif
            {
                auto x = rhsRefExpr->getArg(0);
                std::string varName;

                if( auto im = dyn_cast<ImplicitCastExpr>(x))
                {
                    if( auto im2 = dyn_cast<ImplicitCastExpr>(im->getSubExpr())) // for free
                    {
                        auto x2 = im2->getSubExpr()->getStmtClassName();
                        if( auto d = dyn_cast<DeclRefExpr>(im2->getSubExpr()))
                        {
                            varName = d->getNameInfo().getName().getAsString();
                        }
                    }
                    else // for sockets
                    {
                        if( auto d = dyn_cast<DeclRefExpr>(im->getSubExpr()))
                        {
                            varName = d->getNameInfo().getName().getAsString();
                        }
                    }
                }

                if( staticFuncMapping.find(fooName) != staticFuncMapping.end())
                {
                    fooName = staticFuncMapping[ fooName ];
                }

                Target::Context::Instance().addToLast(new Target::DeleteStatement(varName, false));
            }
        }
    }
    return true;
}

bool ESCAASTVisitor::ProcessCompound( clang::CompoundStmt *body, bool addToStates )
{
    Target::Context::Instance().createCompoundStatement(addToStates);

//    Target::Context::Instance().setIf(state);

    for( auto iter = body->body_begin(); iter != body->body_end(); ++iter )
    {
        ProcessStmt(*iter);
    }

    Target::Context::Instance().popCompound();

    return true;
}

bool ESCAASTVisitor::ProcessAssignment( clang::BinaryOperator *binop )
{
    Cout << "assignment \n";
    using namespace clang;

    Stmt *lhs = binop->getLHS();

//    auto name = lhs->getStmtClassName();

    if( isa<DeclRefExpr>(lhs))
    {
        Cout << "\tto reference ";
        auto *ref = cast<DeclRefExpr>(lhs);
        DeclarationNameInfo nameInfo = ref->getNameInfo();
        DeclarationName name = nameInfo.getName();
        //name.dump();
        std::string sname = name.getAsString();
        Cout << sname;

//        PtrCounter &lhsCnt = variables[ sname ];
//        ++(lhsCnt.count);

        Stmt *rhs = binop->getRHS();

        //rhs->dump();

        if( isa<CXXNewExpr>(rhs))
        {
            auto newOp = cast<CXXNewExpr>(rhs);

            newOp->getBeginLoc();

            Target::Context::Instance().addToLast(
                    new Target::VarAssigmentNewStatement(sname, newOp->isArray(), getLocation(newOp)));
        }
        if( isa<ImplicitCastExpr>(rhs))
        {
            auto ice = cast<ImplicitCastExpr>(rhs);
            Stmt *subexpr = ice->getSubExpr();
            if( isa<DeclRefExpr>(subexpr)) //pointer
            {
                auto rhsRefExpr = cast<DeclRefExpr>(subexpr);
                std::string rhsPointer = rhsRefExpr->getNameInfo().getName().getAsString();

                Target::Context::Instance().addToLast(
                        new Target::VarAssigmentFromPointerStatement(sname, rhsPointer, getLocation(rhsRefExpr)));
            }
        }
        if( isa<CallExpr>(rhs))
        {
            auto rhsRefExpr = cast<CallExpr>(rhs);
            auto callee = rhsRefExpr->getCallee();
            //callee->dump();
            if( auto subExpr = dyn_cast<ImplicitCastExpr>(callee))
            {
                if( auto foo = dyn_cast<DeclRefExpr>(subExpr->getSubExpr()))
                {
                    std::string fooName = foo->getNameInfo().getName().getAsString();

                    if( staticFuncMapping.find(fooName) != staticFuncMapping.end())
                    {
                        fooName = staticFuncMapping[ fooName ];
                    }

                    Target::Context::Instance().curFunction->callee.push_back(fooName);
                    Target::Context::Instance().addToLast(
                            new Target::VarAssigmentFromFooStatement(sname, fooName, getLocation(foo)));
                }
            }
        }
        if( auto castSt = dyn_cast<CastExpr>(rhs))
        {
            if( auto rhsRefExpr = dyn_cast<CallExpr>(castSt->getSubExpr()))
            {
                auto callee = rhsRefExpr->getCallee();
                //callee->dump();
                if( auto subExpr = dyn_cast<ImplicitCastExpr>(callee))
                {
                    if( auto foo = dyn_cast<DeclRefExpr>(subExpr->getSubExpr()))
                    {
                        std::string fooName = foo->getNameInfo().getName().getAsString();

                        if( staticFuncMapping.find(fooName) != staticFuncMapping.end())
                        {
                            fooName = staticFuncMapping[ fooName ];
                        }

                        Target::Context::Instance().curFunction->callee.push_back(fooName);
                        Target::Context::Instance().addToLast(
                                new Target::VarAssigmentFromFooStatement(sname, fooName, getLocation(foo)));
                    }
                }
            }
        }
        //ref->get
    }
    return true;
}

bool ESCAASTVisitor::ProcessDeclaration( clang::VarDecl *vd )
{
    Cout << "\n\n";
    vd->print(llvm::nulls());

    auto tmp1 = vd->getTypeSourceInfo();
    auto tmp = vd->getTypeSourceInfo()->getType();
    auto tmp2 = vd->getTypeSourceInfo()->getType().getTypePtr();

    if( !vd->getType()->isAnyPointerType() && !vd->getType()->isIntegerType())
    {
        return true;
    }

    Cout << "isBlockPointerType: "
         << vd->getTypeSourceInfo()->getType().getTypePtr()->isBlockPointerType()
         << "\n";

    Cout << "isAggregateType: "
         << vd->getTypeSourceInfo()->getType().getTypePtr()->isAggregateType()
         << "\n";

    Cout << "isAnyComplexType: "
         << vd->getTypeSourceInfo()->getType().getTypePtr()->isAnyComplexType()
         << "\n";

    Cout << "isArrayType: "
         << vd->getTypeSourceInfo()->getType().getTypePtr()->isArrayType()
         << "\n";

    Cout << "isDependentSizedArrayType: "
         << vd->getTypeSourceInfo()->getType().getTypePtr()->isDependentSizedArrayType()
         << "\n";

    Cout << "isIntegerType: "
         << vd->getTypeSourceInfo()->getType().getTypePtr()->isIntegerType()
         << "\n";

    Cout << "type: " << vd->getTypeSourceInfo()->getType().getAsString();

    Cout << "\n\n";
    using namespace clang;

//    if( vd->getTypeSourceInfo()->getType().getTypePtr()->isAnyPointerType())
//    {
    auto name = vd->getNameAsString();
    if( variables.find(name) != variables.end())
    {
        //Error: this variable is already declared.
        Cout << "Variable with name " << name << " declared twice\n";
        return true;
    }
    variables.insert(name);

    auto init = vd->getAnyInitializer();
    if( init == nullptr )
    {
        return true;
    }
    if( !isa<CXXNewExpr>(init))
    {
        if( auto castSt = dyn_cast<CastExpr>(init))
        {
            init = castSt->getSubExpr();
        }

        if( auto rhsRefExpr = dyn_cast<CallExpr>(init))
        {
            auto callee = rhsRefExpr->getCallee();
            //callee->dump();
            if( auto subExpr = dyn_cast<ImplicitCastExpr>(callee))
            {
                if( auto foo = dyn_cast<DeclRefExpr>(subExpr->getSubExpr()))
                {
                    std::string fooName = foo->getNameInfo().getName().getAsString();

                    if( staticFuncMapping.find(fooName) != staticFuncMapping.end())
                    {
                        fooName = staticFuncMapping[ fooName ];
                    }

                    Target::Context::Instance().curFunction->callee.push_back(fooName);
                    Target::Context::Instance().addToLast(
                            new Target::VarDeclFromFooStatement(name, fooName, getLocation(foo)));
                }
            }
        }
        return false;
    }

    auto newExpr = cast<CXXNewExpr>(init);

    Target::Context::Instance().addToLast(
            new Target::VarDeclNewStatement(name, newExpr->isArray(), getLocation(newExpr)));

    return true;
}

bool ESCAASTVisitor::ProcessDelete( clang::CXXDeleteExpr *del )
{
    using namespace clang;

    auto argDel = del->getArgument();
    //TODO: change this if by function that gives a full name of pointer or null when there is no pointer.
    if( isa<ImplicitCastExpr>(argDel))
    {
        auto delCast = cast<ImplicitCastExpr>(argDel);
        auto dexpr = delCast->getSubExpr();
        if( isa<DeclRefExpr>(dexpr))
        {
            auto dptr = cast<DeclRefExpr>(dexpr);
            std::string name = dptr->getNameInfo().getAsString();

            Target::Context::Instance().addToLast(new Target::DeleteStatement(name, del->isArrayForm()));
        }
    }
    return true;
}

bool ESCAASTVisitor::ProcessReturn( clang::ReturnStmt *ret )
{
    clang::Expr *retVal = ret->getRetValue();

//    returnExpr = retVal;

    std::string returnVarName2;

    //retVal->dump();
    using namespace clang;

    if( retVal && isa<ImplicitCastExpr>(retVal))
    {
        auto ice = cast<ImplicitCastExpr>(retVal);
        Stmt *subexpr = ice->getSubExpr();
        if( isa<DeclRefExpr>(subexpr)) //pointer
        {
            auto ref = cast<DeclRefExpr>(subexpr);
            DeclarationNameInfo nameInfo = ref->getNameInfo();
            DeclarationName name = nameInfo.getName();
            //name.dump();
            std::string sname = name.getAsString();

            Target::Context::Instance().curFunction->returnName.insert(sname);

            returnVarName2 = sname;
        }
    }

    Target::Context::Instance().addToLast(new Target::ReturnStatement(returnVarName2));

    return /*res*/true;
}

bool ESCAASTVisitor::ProcessIf( clang::IfStmt *ifstmt )
{
    Cout << "If statement!\n";
//    ifstmt->dump();
    auto cond = ifstmt->getCond();
    {
        auto curCond = cond;
        if( auto c = clang::dyn_cast<clang::UnaryOperator>(curCond))
        {
            curCond = c->getSubExpr();
        }

        if( auto castSt = clang::dyn_cast<clang::CastExpr>(curCond))
        {
            auto sub4 = castSt->getSubExpr();
            if( auto castSt3 = clang::dyn_cast<clang::CastExpr>(sub4))
            {
                auto sub3 = castSt3->getSubExpr();
                if( auto parenSt = clang::dyn_cast<clang::ParenExpr>(sub3))
                {
                    auto sub = parenSt->getSubExpr();
                    if( auto bo = clang::dyn_cast<clang::BinaryOperator>(sub))
                    {
                        if( bo->isAssignmentOp())
                        {
                            ProcessAssignment(bo);
                        }
                    }
                }
            }
        }
    }
    std::string typeS;
    llvm::raw_string_ostream lso(typeS);
    clang::LangOptions langOpts;
    langOpts.CPlusPlus17 = true;
    clang::PrintingPolicy pol(langOpts);
    cond->printPretty(lso, nullptr, pol);
    std::string condStr = "~if - " + lso.str();
    std::string elseStr = "~else - " + condStr;
    Cout << "\tCondition: " << condStr << "\n";

    //TODO: Create the state branching.
    Target::Statement *thenSt = nullptr;
//    Target::Context::Instance().startIfSt(&thenSt);

    // почему-то не заходим в then и в else,
    // видимо для скорости
    ProcessStmt(ifstmt->getThen(), false);

    Target::Statement *elseSt = nullptr;
//    Target::Context::Instance().startIfSt(&elseSt);
    if( ifstmt->hasElseStorage())
    {
        ProcessStmt(ifstmt->getElse(), false);
    }

    Target::Context::Instance().addToLast(new Target::IfStatement(thenSt, elseSt, condStr, elseStr));

    return true;
}


//bool ESCAASTVisitor::ProcessReturnNone()
//{
//    fsm.ProcessReturnNone();
//    return true;
//}

//bool ESCAASTVisitor::ProcessReturnPtr( clang::ReturnStmt *ret )
//{
//    auto retVal = ret->getRetValue();
//
//    return true;
//}
