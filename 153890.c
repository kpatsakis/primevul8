void sqlite3ExprIfTrue(Parse *pParse, Expr *pExpr, int dest, int jumpIfNull){
  Vdbe *v = pParse->pVdbe;
  int op = 0;
  int regFree1 = 0;
  int regFree2 = 0;
  int r1, r2;

  assert( jumpIfNull==SQLITE_JUMPIFNULL || jumpIfNull==0 );
  if( NEVER(v==0) )     return;  /* Existence of VDBE checked by caller */
  if( NEVER(pExpr==0) ) return;  /* No way this can happen */
  op = pExpr->op;
  switch( op ){
    case TK_AND:
    case TK_OR: {
      Expr *pAlt = sqlite3ExprSimplifiedAndOr(pExpr);
      if( pAlt!=pExpr ){
        sqlite3ExprIfTrue(pParse, pAlt, dest, jumpIfNull);
      }else if( op==TK_AND ){
        int d2 = sqlite3VdbeMakeLabel(pParse);
        testcase( jumpIfNull==0 );
        sqlite3ExprIfFalse(pParse, pExpr->pLeft, d2,
                           jumpIfNull^SQLITE_JUMPIFNULL);
        sqlite3ExprIfTrue(pParse, pExpr->pRight, dest, jumpIfNull);
        sqlite3VdbeResolveLabel(v, d2);
      }else{
        testcase( jumpIfNull==0 );
        sqlite3ExprIfTrue(pParse, pExpr->pLeft, dest, jumpIfNull);
        sqlite3ExprIfTrue(pParse, pExpr->pRight, dest, jumpIfNull);
      }
      break;
    }
    case TK_NOT: {
      testcase( jumpIfNull==0 );
      sqlite3ExprIfFalse(pParse, pExpr->pLeft, dest, jumpIfNull);
      break;
    }
    case TK_TRUTH: {
      int isNot;      /* IS NOT TRUE or IS NOT FALSE */
      int isTrue;     /* IS TRUE or IS NOT TRUE */
      testcase( jumpIfNull==0 );
      isNot = pExpr->op2==TK_ISNOT;
      isTrue = sqlite3ExprTruthValue(pExpr->pRight);
      testcase( isTrue && isNot );
      testcase( !isTrue && isNot );
      if( isTrue ^ isNot ){
        sqlite3ExprIfTrue(pParse, pExpr->pLeft, dest,
                          isNot ? SQLITE_JUMPIFNULL : 0);
      }else{
        sqlite3ExprIfFalse(pParse, pExpr->pLeft, dest,
                           isNot ? SQLITE_JUMPIFNULL : 0);
      }
      break;
    }
    case TK_IS:
    case TK_ISNOT:
      testcase( op==TK_IS );
      testcase( op==TK_ISNOT );
      op = (op==TK_IS) ? TK_EQ : TK_NE;
      jumpIfNull = SQLITE_NULLEQ;
      /* Fall thru */
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      if( sqlite3ExprIsVector(pExpr->pLeft) ) goto default_expr;
      testcase( jumpIfNull==0 );
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op,
                  r1, r2, dest, jumpIfNull, ExprHasProperty(pExpr,EP_Commuted));
      assert(TK_LT==OP_Lt); testcase(op==OP_Lt); VdbeCoverageIf(v,op==OP_Lt);
      assert(TK_LE==OP_Le); testcase(op==OP_Le); VdbeCoverageIf(v,op==OP_Le);
      assert(TK_GT==OP_Gt); testcase(op==OP_Gt); VdbeCoverageIf(v,op==OP_Gt);
      assert(TK_GE==OP_Ge); testcase(op==OP_Ge); VdbeCoverageIf(v,op==OP_Ge);
      assert(TK_EQ==OP_Eq); testcase(op==OP_Eq);
      VdbeCoverageIf(v, op==OP_Eq && jumpIfNull==SQLITE_NULLEQ);
      VdbeCoverageIf(v, op==OP_Eq && jumpIfNull!=SQLITE_NULLEQ);
      assert(TK_NE==OP_Ne); testcase(op==OP_Ne);
      VdbeCoverageIf(v, op==OP_Ne && jumpIfNull==SQLITE_NULLEQ);
      VdbeCoverageIf(v, op==OP_Ne && jumpIfNull!=SQLITE_NULLEQ);
      testcase( regFree1==0 );
      testcase( regFree2==0 );
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      assert( TK_ISNULL==OP_IsNull );   testcase( op==TK_ISNULL );
      assert( TK_NOTNULL==OP_NotNull ); testcase( op==TK_NOTNULL );
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      sqlite3VdbeAddOp2(v, op, r1, dest);
      VdbeCoverageIf(v, op==TK_ISNULL);
      VdbeCoverageIf(v, op==TK_NOTNULL);
      testcase( regFree1==0 );
      break;
    }
    case TK_BETWEEN: {
      testcase( jumpIfNull==0 );
      exprCodeBetween(pParse, pExpr, dest, sqlite3ExprIfTrue, jumpIfNull);
      break;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_IN: {
      int destIfFalse = sqlite3VdbeMakeLabel(pParse);
      int destIfNull = jumpIfNull ? dest : destIfFalse;
      sqlite3ExprCodeIN(pParse, pExpr, destIfFalse, destIfNull);
      sqlite3VdbeGoto(v, dest);
      sqlite3VdbeResolveLabel(v, destIfFalse);
      break;
    }
#endif
    default: {
    default_expr:
      if( ExprAlwaysTrue(pExpr) ){
        sqlite3VdbeGoto(v, dest);
      }else if( ExprAlwaysFalse(pExpr) ){
        /* No-op */
      }else{
        r1 = sqlite3ExprCodeTemp(pParse, pExpr, &regFree1);
        sqlite3VdbeAddOp3(v, OP_If, r1, dest, jumpIfNull!=0);
        VdbeCoverage(v);
        testcase( regFree1==0 );
        testcase( jumpIfNull==0 );
      }
      break;
    }
  }
  sqlite3ReleaseTempReg(pParse, regFree1);
  sqlite3ReleaseTempReg(pParse, regFree2);  
}