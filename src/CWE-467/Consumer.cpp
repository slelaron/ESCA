//
// Created by slelaron on 06.01.2021.
//

#include "Consumer.h"

bool Consumer::HandleTopLevelDecl(clang::DeclGroupRef DR) {
    for (auto it: DR) {
        visitor.TraverseDecl(it);
    }
    return true;
}
