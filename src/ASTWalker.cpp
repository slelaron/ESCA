#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/FileSystemOptions.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/Builtins.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/FrontendOptions.h>
#include <clang/Frontend/Utils.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Parse/ParseAST.h>

#include <clang/AST/ASTContext.h>

#include <llvm/Support/Host.h>
//#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/Function.h>

#include "ASTWalker.h"
#include "ESCAASTConsumer.h"

ASTWalker::ASTWalker()
        : headerSearchOptions(new clang::HeaderSearchOptions()), astConsumer(new ESCAASTConsumer) {
    astConsumer->SetWalker(this);
}

ASTWalker::~ASTWalker() {
    delete astConsumer;
    delete astContext;
}

void ASTWalker::SetIncludeDirectories() {
    //headerSearchOptions->AddPath("D:\\Portable\\MinGW\\include", clang::frontend::Angled,
    //	false, false);
    //headerSearchOptions->AddPath("D:\\Portable\\MinGW\\include\\c++\\3.4.5" , clang::frontend::Angled,
    //	false, false);
    //headerSearchOptions->AddPath("D:\\Portable\\MinGW\\include\\c++\\3.4.5\\mingw32" , clang::frontend::Angled,
    //	false, false);
    //headerSearchOptions->AddPath("D:\\Portable\\MinGW\\lib\\gcc\\mingw32\\3.4.5\\include" , clang::frontend::Angled,
    //	false, false);
}

void ASTWalker::WalkAST(const std::string &path) {
    clang::DiagnosticOptions diagnosticOptions;
    auto *pTextDiagnosticPrinter = new clang::TextDiagnosticPrinter(llvm::outs(), &diagnosticOptions);
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    auto pDiagnosticsEngine = new clang::DiagnosticsEngine(pDiagIDs,
                                                           &diagnosticOptions, pTextDiagnosticPrinter);

    clang::LangOptions languageOptions;
    clang::FileSystemOptions fileSystemOptions = clang::FileSystemOptions();
    clang::FileManager fileManager(fileSystemOptions);

    clang::SourceManager sourceManager(*pDiagnosticsEngine, fileManager);

    clang::InputKind ik(clang::Language::CXX, clang::InputKind::Source, false);
    llvm::Triple triple;
    clang::PreprocessorOptions ppopts;
    clang::CompilerInvocation::setLangDefaults(languageOptions, ik, triple, ppopts);
    languageOptions.ImplicitInt = 1;

//	SetIncludeDirectories();
//    llvm::IntrusiveRefCntPtr<clang::HeaderSearchOptions> headerSearchOptions(new clang::HeaderSearchOptions());
//    /*
//        headerSearchOptions->ResourceDir = "/opt/llvm_build" "/lib/clang/" CLANG_VERSION_STRING;
    // <Warning!!> -- Platform Specific Code lives here
    // This depends on A) that you're running linux and
    // B) that you have the same GCC LIBs installed that
    // I do.
    // Search through Clang itself for something like this,
    // go on, you won't find it. The reason why is Clang
    // has its own versions of std* which are installed under
    // /usr/local/lib/clang/<version>/include/
    // See somewhere around Driver.cpp:77 to see Clang adding
    // its version of the headers to its include path.
//    for (int i = 2; i < argc; i++)
//    {
//        headerSearchOptions->AddPath(argv[i], clang::frontend::Angled, false, false);
//    }
    // </Warning!!> -- End of Platform Specific Code

//    headerSearchOptions->AddPath("D:\\Portable\\MinGW\\include", clang::frontend::Angled,
//        false, false);
//    headerSearchOptions->AddPath("D:\\Portable\\MinGW\\include\\c++\\3.4.5" , clang::frontend::Angled,
//        false, false);
//    headerSearchOptions->AddPath("D:\\Portable\\MinGW\\include\\c++\\3.4.5\\mingw32" , clang::frontend::Angled,
//        false, false);
//    headerSearchOptions->AddPath("D:\\Portable\\MinGW\\lib\\gcc\\mingw32\\3.4.5\\include" , clang::frontend::Angled,
//        false, false);
    // */

    //clang::TargetOptions targetOptions;
    auto targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = llvm::sys::getDefaultTargetTriple();

    clang::TargetInfo *pTargetInfo =
            clang::TargetInfo::CreateTargetInfo(
                    *pDiagnosticsEngine,
                    targetOptions);

    clang::HeaderSearch headerSearch(headerSearchOptions,
                                     sourceManager,
                                     *pDiagnosticsEngine,
                                     languageOptions,
                                     pTargetInfo);

    clang::CompilerInstance compInst;

    //llvm::IntrusiveRefCntPtr<clang::PreprocessorOptions> pOpts( new clang::PreprocessorOptions());
    auto pOpts = std::make_shared<clang::PreprocessorOptions>();
    //clang::Preprocessor preprocessor(
    //    pOpts,
    //    *pDiagnosticsEngine,
    //    languageOptions,
    //    pTargetInfo,
    //    sourceManager,
    //    headerSearch,
    //    compInst);
    clang::Preprocessor preprocessor(
            pOpts,
            *pDiagnosticsEngine,
            languageOptions,
            sourceManager,
            headerSearch,
            compInst);


    clang::FrontendOptions frontendOptions;
    clang::RawPCHContainerReader containerReader;

    // TODO: тут возникает segfault, пофиксить
    clang::InitializePreprocessor(
            preprocessor,
            *pOpts,
            containerReader,
            frontendOptions);

    clang::ApplyHeaderSearchOptions(
            headerSearch,
            *headerSearchOptions,
            languageOptions,
            triple
    );

    llvm::ErrorOr<const clang::FileEntry *> pFile = fileManager.getFile(path);
    //sourceManager.createMainFileID(pFile);
    //sourceManager.createFileID(pFile);

    const clang::TargetInfo &targetInfo = *pTargetInfo;

    clang::IdentifierTable identifierTable(languageOptions);
    clang::SelectorTable selectorTable;

    clang::Builtin::Context builtinContext;
    builtinContext.InitializeTarget(targetInfo, nullptr);

    //astContext = new ASTContext(languageOptions, sourceManager, pTargetInfo, identifierTable, selectorTable,
    //	builtinContext, 0);
    astContext = new clang::ASTContext(languageOptions, sourceManager, identifierTable, selectorTable, builtinContext);

    {
        astConsumer->SetPath(path);
        pTextDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);
        clang::ParseAST(preprocessor, astConsumer, *astContext);
        pTextDiagnosticPrinter->EndSourceFile(); 
    }
}

//void ASTWalker::DumpStmt(clang::Stmt *s) {
//    s->dump(llvm::errs, astContext->getSourceManager());
//    s->dump();
//}
