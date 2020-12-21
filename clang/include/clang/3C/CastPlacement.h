//=--CastPlacement.h----------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class contains functions and classes that deal with
// placing casts into the text as needing during the rewrite phase
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_3C_CASTPLACEMENT_H
#define LLVM_CLANG_3C_CASTPLACEMENT_H

#include "clang/3C/ConstraintResolver.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "RewriteUtils.h"

// Locates expressions which are children of explicit cast expressions after
// ignoring any intermediate implicit expressions introduced in the clang AST.
class CastLocatorVisitor : public RecursiveASTVisitor<CastLocatorVisitor> {
public:
  explicit CastLocatorVisitor(ASTContext *C) : Context(C) {}

  bool VisitCastExpr(CastExpr *C);

  bool exprHasCast(Expr *E) const;

private:
  ASTContext *Context;
  std::set<Expr *> ExprsWithCast;

  static Expr *ignoreCheckedCImplicit(Expr *E);
};

class CastPlacementVisitor : public RecursiveASTVisitor<CastPlacementVisitor> {
public:
  explicit CastPlacementVisitor
    (ASTContext *C, ProgramInfo &I, Rewriter &R, CastLocatorVisitor &L)
    : Context(C), Info(I), Writer(R), CR(Info, Context), ABRewriter(I),
      Locator(L) {}

  bool VisitCallExpr(CallExpr *C);

private:
  ASTContext *Context;
  ProgramInfo &Info;
  Rewriter &Writer;
  ConstraintResolver CR;
  ArrayBoundsRewriter ABRewriter;
  CastLocatorVisitor &Locator;

  // Enumeration indicating what type of cast is required at a call site
  enum CastNeeded {
    NO_CAST = 0,     // No casting required
    CAST_TO_CHECKED, // A CheckedC bounds cast required (wild -> checked)
    CAST_TO_WILD     // A standard C explicit cast required (checked -> wild)
  };

  CastNeeded needCasting(InternalExternalPair<ConstraintVariable> Src,
                         InternalExternalPair<ConstraintVariable> Dst);

  std::pair<std::string, std::string>
  getCastString(InternalExternalPair<ConstraintVariable> Src,
                InternalExternalPair<ConstraintVariable> Dst);

  void surroundByCast(InternalExternalPair<ConstraintVariable> Src,
                      InternalExternalPair<ConstraintVariable> Dst, Expr *);
};
#endif // LLVM_CLANG_3C_CASTPLACEMENT_H
