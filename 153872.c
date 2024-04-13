static void codeVectorCompare(
  Parse *pParse,        /* Code generator context */
  Expr *pExpr,          /* The comparison operation */
  int dest,             /* Write results into this register */
  u8 op,                /* Comparison operator */
  u8 p5                 /* SQLITE_NULLEQ or zero */
){
  Vdbe *v = pParse->pVdbe;
  Expr *pLeft = pExpr->pLeft;
  Expr *pRight = pExpr->pRight;
  int nLeft = sqlite3ExprVectorSize(pLeft);
  int i;
  int regLeft = 0;
  int regRight = 0;
  u8 opx = op;
  int addrDone = sqlite3VdbeMakeLabel(pParse);
  int isCommuted = ExprHasProperty(pExpr,EP_Commuted);

  if( nLeft!=sqlite3ExprVectorSize(pRight) ){
    sqlite3ErrorMsg(pParse, "row value misused");
    return;
  }
  assert( pExpr->op==TK_EQ || pExpr->op==TK_NE 
       || pExpr->op==TK_IS || pExpr->op==TK_ISNOT 
       || pExpr->op==TK_LT || pExpr->op==TK_GT 
       || pExpr->op==TK_LE || pExpr->op==TK_GE 
  );
  assert( pExpr->op==op || (pExpr->op==TK_IS && op==TK_EQ)
            || (pExpr->op==TK_ISNOT && op==TK_NE) );
  assert( p5==0 || pExpr->op!=op );
  assert( p5==SQLITE_NULLEQ || pExpr->op==op );

  p5 |= SQLITE_STOREP2;
  if( opx==TK_LE ) opx = TK_LT;
  if( opx==TK_GE ) opx = TK_GT;

  regLeft = exprCodeSubselect(pParse, pLeft);
  regRight = exprCodeSubselect(pParse, pRight);

  for(i=0; 1 /*Loop exits by "break"*/; i++){
    int regFree1 = 0, regFree2 = 0;
    Expr *pL, *pR; 
    int r1, r2;
    assert( i>=0 && i<nLeft );
    r1 = exprVectorRegister(pParse, pLeft, i, regLeft, &pL, &regFree1);
    r2 = exprVectorRegister(pParse, pRight, i, regRight, &pR, &regFree2);
    codeCompare(pParse, pL, pR, opx, r1, r2, dest, p5, isCommuted);
    testcase(op==OP_Lt); VdbeCoverageIf(v,op==OP_Lt);
    testcase(op==OP_Le); VdbeCoverageIf(v,op==OP_Le);
    testcase(op==OP_Gt); VdbeCoverageIf(v,op==OP_Gt);
    testcase(op==OP_Ge); VdbeCoverageIf(v,op==OP_Ge);
    testcase(op==OP_Eq); VdbeCoverageIf(v,op==OP_Eq);
    testcase(op==OP_Ne); VdbeCoverageIf(v,op==OP_Ne);
    sqlite3ReleaseTempReg(pParse, regFree1);
    sqlite3ReleaseTempReg(pParse, regFree2);
    if( i==nLeft-1 ){
      break;
    }
    if( opx==TK_EQ ){
      sqlite3VdbeAddOp2(v, OP_IfNot, dest, addrDone); VdbeCoverage(v);
      p5 |= SQLITE_KEEPNULL;
    }else if( opx==TK_NE ){
      sqlite3VdbeAddOp2(v, OP_If, dest, addrDone); VdbeCoverage(v);
      p5 |= SQLITE_KEEPNULL;
    }else{
      assert( op==TK_LT || op==TK_GT || op==TK_LE || op==TK_GE );
      sqlite3VdbeAddOp2(v, OP_ElseNotEq, 0, addrDone);
      VdbeCoverageIf(v, op==TK_LT);
      VdbeCoverageIf(v, op==TK_GT);
      VdbeCoverageIf(v, op==TK_LE);
      VdbeCoverageIf(v, op==TK_GE);
      if( i==nLeft-2 ) opx = op;
    }
  }
  sqlite3VdbeResolveLabel(v, addrDone);
}