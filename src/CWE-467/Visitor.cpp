//
// Created by slelaron on 04.01.2021.
//

#include "Visitor.h"
#include <iostream>

bool Visitor::VisitFunctionDecl(clang::FunctionDecl *f) {
    sourceManager = &f->getASTContext().getSourceManager();
    //f->dump(llvm::errs(), sourceManager);
    //f->dump();
    return true;
}

bool Visitor::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr *expr) {
    if (expr->getKind() == clang::UETT_SizeOf && !expr->isArgumentType() &&
        (expr->getArgumentExpr()->getType()->isPointerType() || expr->getArgumentExpr()->getType()->isArrayType())) {
        auto loc = expr->getBeginLoc();
        std::string location = loc.printToString(*sourceManager);
        std::cerr << "The code calls sizeof() on a malloced pointer type, "
                     "which always returns the wordsize/8 on line " << location << std::endl;
    }
    return true;
}
