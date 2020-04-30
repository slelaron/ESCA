#include <clang/Basic/SourceManager.h>
#include <clang/Basic/Builtins.h>
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
#include <llvm/IR/Function.h>
#include <iostream>

#include "ASTWalker.h"

ASTWalker::ASTWalker()
        : headerSearchOptions(new clang::HeaderSearchOptions()), astConsumer(new ESCAASTConsumer())
{
}

ASTWalker::~ASTWalker()
{
//    delete astConsumer;
//    delete astContext;
}

void ASTWalker::SetIncludeDirectories( const std::vector<std::string> &paths )
{
    astConsumer->SetExcludedPaths(paths); // исключаем из анализа системные директории
    for( const auto &path : paths )
    {
        headerSearchOptions->AddPath(path, clang::frontend::Angled, false, false);
    }
}

bool ASTWalker::WalkAST( const std::string &path )
{
    astConsumer->SetAnaliseFile(path);

    clang::DiagnosticOptions diagnosticOptions;
    // TextDiagnosticPrinter анализирует файлы, чтобы не выводил весь анализ в stdout,
    // создаем файл куда и выводим всю информацию
    std::error_code ec;
    llvm::raw_fd_ostream dbg_inf("diagnostic_info.txt", ec);
    if( ec )
    {
        std::cerr << ec.message() << std::endl;
        return false;
    }

    auto pTextDiagnosticPrinter = new clang::TextDiagnosticPrinter(dbg_inf, &diagnosticOptions);

    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    auto pDiagnosticsEngine = new clang::DiagnosticsEngine(pDiagIDs, &diagnosticOptions, pTextDiagnosticPrinter);

    clang::FileSystemOptions fileSystemOptions = clang::FileSystemOptions();
    clang::FileManager fileManager(fileSystemOptions);

    clang::SourceManager sourceManager(*pDiagnosticsEngine, fileManager);

    clang::InputKind ik(clang::Language::CXX, clang::InputKind::Source, false);
    llvm::Triple triple;
    clang::PreprocessorOptions ppopts;
    clang::LangOptions languageOptions;

    clang::CompilerInvocation::setLangDefaults(languageOptions, ik, triple, ppopts);
    languageOptions.ImplicitInt = 1;


    auto targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
    clang::TargetInfo *pTargetInfo = clang::TargetInfo::CreateTargetInfo(*pDiagnosticsEngine, targetOptions);

    clang::HeaderSearch headerSearch(headerSearchOptions,
                                     sourceManager,
                                     *pDiagnosticsEngine,
                                     languageOptions,
                                     pTargetInfo);

    clang::CompilerInstance compInst;

    auto pOpts = std::make_shared<clang::PreprocessorOptions>();
    clang::Preprocessor preprocessor(
            pOpts,
            *pDiagnosticsEngine,
            languageOptions,
            sourceManager,
            headerSearch,
            compInst
    );

    preprocessor.Initialize(*pTargetInfo);

    clang::FrontendOptions frontendOptions;
    clang::RawPCHContainerReader containerReader;

    clang::ApplyHeaderSearchOptions(
            headerSearch,
            *headerSearchOptions,
            languageOptions,
            triple
    );

    clang::InitializePreprocessor(
            preprocessor,
            *pOpts,
            containerReader,
            frontendOptions
    );


    llvm::ErrorOr<const clang::FileEntry *> pFile = fileManager.getFile(path);
//    clang::SourceLocation sourceLocation;
    if( !pFile )
    {
        std::cerr << pFile.getError().message() << ", path: " << path << std::endl;
        return false;
    }
    auto mainID = sourceManager.getOrCreateFileID(pFile.get(), clang::SrcMgr::C_System);
    sourceManager.setMainFileID(mainID);
    const clang::TargetInfo &targetInfo = *pTargetInfo;

    clang::IdentifierTable identifierTable(languageOptions);
    clang::SelectorTable selectorTable;

    clang::Builtin::Context builtinContext;
    builtinContext.InitializeTarget(targetInfo, nullptr);

    astContext = std::make_unique<clang::ASTContext>(languageOptions, sourceManager, identifierTable, selectorTable,
                                                     builtinContext);
    astContext->InitBuiltinTypes(targetInfo);
//    astConsumer->Initialize(*astContext);
    {
        pTextDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);
        clang::ParseAST(preprocessor, static_cast<clang::ASTConsumer *>(astConsumer.get()), *astContext);
        pTextDiagnosticPrinter->EndSourceFile();
    }
    return true;
}

void ASTWalker::DumpStmt( clang::Stmt *s )
{
    s->dump(llvm::errs(), astContext->getSourceManager());
    s->dump();
}

Target::Context ASTWalker::GetContext()
{
    return astConsumer->GetContext();
}

void ASTWalker::RunAnalyzer()
{
    for( auto p : allFunctions )
    {
        p.second->process();
    }
}
