//
// Created by slelaron on 06.01.2021.
//

#ifndef ESCA_CONSUMER_H
#define ESCA_CONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclGroup.h>

#include "Consumer.h"
#include "Visitor.h"

struct Consumer : public clang::ASTConsumer {
    bool HandleTopLevelDecl(clang::DeclGroupRef DR) override;
private:
    Visitor visitor;
};

#endif //ESCA_CONSUMER_H
