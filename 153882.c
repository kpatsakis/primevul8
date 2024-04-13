int sqlite3ExprImpliesExpr(Parse *pParse, Expr *pE1, Expr *pE2, int iTab){
  if( sqlite3ExprCompare(pParse, pE1, pE2, iTab)==0 ){
    return 1;
  }
  if( pE2->op==TK_OR
   && (sqlite3ExprImpliesExpr(pParse, pE1, pE2->pLeft, iTab)
             || sqlite3ExprImpliesExpr(pParse, pE1, pE2->pRight, iTab) )
  ){
    return 1;
  }
  if( pE2->op==TK_NOTNULL
   && exprImpliesNotNull(pParse, pE1, pE2->pLeft, iTab, 0)
  ){
    return 1;
  }
  return 0;
}