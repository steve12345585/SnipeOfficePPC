/*
 * This file is part of the LibreOffice project.
 *
 * Based on LLVM/Clang.
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 */

#include "plugin.hxx"

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/FileManager.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

#include "bodynotinblock.hxx"
#include "lclstaticfix.hxx"
#include "postfixincrementfix.hxx"
#include "sallogareas.hxx"
#include "unusedvariablecheck.hxx"

namespace loplugin
{

Plugin::Plugin( ASTContext& context )
    : context( context )
    {
    }

DiagnosticBuilder Plugin::report( DiagnosticsEngine::Level level, StringRef message, SourceLocation loc )
    {
    DiagnosticsEngine& diag = context.getDiagnostics();
#if 0
    // Do some mappings (e.g. for -Werror) that clang does not do for custom messages for some reason.
    if( level == DiagnosticsEngine::Warning && diag.getWarningsAsErrors())
        level = DiagnosticsEngine::Error;
    if( level == DiagnosticsEngine::Error && diag.getErrorsAsFatal())
        level = DiagnosticsEngine::Fatal;
#endif
    if( loc.isValid())
        return diag.Report( loc, diag.getCustomDiagID( level, message ));
    else
        return diag.Report( diag.getCustomDiagID( level, message ));
    }

bool Plugin::ignoreLocation( SourceLocation loc )
    {
    return context.getSourceManager().isInSystemHeader( context.getSourceManager().getExpansionLoc( loc ));
    }


RewritePlugin::RewritePlugin( ASTContext& context, Rewriter& rewriter )
    : Plugin( context )
    , rewriter( rewriter )
    {
    }

bool RewritePlugin::insertText( SourceLocation Loc, StringRef Str, bool InsertAfter, bool indentNewLines )
    {
    if( rewriter.InsertText( Loc, Str, InsertAfter, indentNewLines ))
        return reportEditFailure( Loc );
    return true;
    }

bool RewritePlugin::insertTextAfter( SourceLocation Loc, StringRef Str )
    {
    if( rewriter.InsertTextAfter( Loc, Str ))
        return reportEditFailure( Loc );
    return true;
    }

bool RewritePlugin::insertTextAfterToken( SourceLocation Loc, StringRef Str )
    {
    if( rewriter.InsertTextAfterToken( Loc, Str ))
        return reportEditFailure( Loc );
    return true;
    }

bool RewritePlugin::insertTextBefore( SourceLocation Loc, StringRef Str )
    {
    if( rewriter.InsertTextBefore( Loc, Str ))
        return reportEditFailure( Loc );
    return true;
    }

bool RewritePlugin::removeText( SourceLocation Start, unsigned Length, RewriteOptions opts )
    {
    if( rewriter.RemoveText( Start, Length, opts ))
        return reportEditFailure( Start );
    return true;
    }

bool RewritePlugin::removeText( CharSourceRange range, RewriteOptions opts )
    {
    if( rewriter.RemoveText( range, opts ))
        return reportEditFailure( range.getBegin());
    return true;
    }

bool RewritePlugin::removeText( SourceRange range, RewriteOptions opts )
    {
    if( rewriter.RemoveText( range, opts ))
        return reportEditFailure( range.getBegin());
    return true;
    }

bool RewritePlugin::replaceText( SourceLocation Start, unsigned OrigLength, StringRef NewStr )
    {
    if( rewriter.ReplaceText( Start, OrigLength, NewStr ))
        return reportEditFailure( Start );
    return true;
    }

bool RewritePlugin::replaceText( SourceRange range, StringRef NewStr )
    {
    if( rewriter.ReplaceText( range, NewStr ))
        return reportEditFailure( range.getBegin());
    return true;
    }

bool RewritePlugin::replaceText( SourceRange range, SourceRange replacementRange )
    {
    if( rewriter.ReplaceText( range, replacementRange ))
        return reportEditFailure( range.getBegin());
    return true;
    }

bool RewritePlugin::reportEditFailure( SourceLocation loc )
    {
    DiagnosticsEngine& diag = context.getDiagnostics();
    diag.Report( loc, diag.getCustomDiagID( DiagnosticsEngine::Warning,
        "cannot perform source modification (macro expansion involved?) [loplugin]" ));
    return false;
    }


/**
 Class that manages all LO modules.
*/
class PluginHandler
    : public ASTConsumer
    {
    public:
        explicit PluginHandler( ASTContext& context, const vector< string >& args )
            : rewriter( context.getSourceManager(), context.getLangOpts())
            , args( args )
            , bodyNotInBlock( context )
            , lclStaticFix( context, rewriter )
            , postfixIncrementFix( context, rewriter )
            , salLogAreas( context )
            , unusedVariableCheck( context )
            {
            }
        virtual void HandleTranslationUnit( ASTContext& context )
            {
            if( context.getDiagnostics().hasErrorOccurred())
                return;
            if( isArg( "lclstaticfix" ))
                lclStaticFix.run();
            else if( isArg( "postfixincrementfix" ))
                postfixIncrementFix.run();
            else if( args.empty())
                {
                bodyNotInBlock.run();
                salLogAreas.run();
                unusedVariableCheck.run();
                }
            else
                {
                DiagnosticsEngine& diag = context.getDiagnostics();
                diag.Report( diag.getCustomDiagID( DiagnosticsEngine::Fatal,
                    "unknown plugin tool %0 [loplugin]" )) << args.front();
                }
            for( Rewriter::buffer_iterator it = rewriter.buffer_begin();
                 it != rewriter.buffer_end();
                 ++it )
                {
                const FileEntry* e = context.getSourceManager().getFileEntryForID( it->first );
                string filename = std::string( e->getName()) + ".new";
                string error;
                // TODO If there will be actually plugins also modifying headers,
                // race conditions should be avoided here.
                raw_fd_ostream ostream( filename.c_str(), error );
                DiagnosticsEngine& diag = context.getDiagnostics();
                if( !error.empty())
                    diag.Report( diag.getCustomDiagID( DiagnosticsEngine::Error,
                        "cannot write modified source to %0 (%1) [loplugin]" )) << filename << error;
                else
                    diag.Report( diag.getCustomDiagID( DiagnosticsEngine::Note,
                        "modified source %0 [loplugin]" )) << filename;
                it->second.write( ostream );
                }
            }
    private:
        bool isArg( const char* arg ) const
            {
            return find( args.begin(), args.end(), arg ) != args.end();
            }
        Rewriter rewriter;
        vector< string > args;
        BodyNotInBlock bodyNotInBlock;
        LclStaticFix lclStaticFix;
        PostfixIncrementFix postfixIncrementFix;
        SalLogAreas salLogAreas;
        UnusedVariableCheck unusedVariableCheck;
    };

/**
 The Clang plugin class, just forwards to PluginHandler.
*/
class LibreOfficeAction
    : public PluginASTAction
    {
    public:
        virtual ASTConsumer* CreateASTConsumer( CompilerInstance& Compiler, StringRef InFile )
            {
            return new PluginHandler( Compiler.getASTContext(), _args );
            }
        virtual bool ParseArgs( const CompilerInstance& CI, const vector< string >& args )
            {
            _args = args;
            return true;
            }
    private:
        vector< string > _args;
    };

} // namespace

static FrontendPluginRegistry::Add< loplugin::LibreOfficeAction > X( "loplugin", "LibreOffice compile check plugin" );
