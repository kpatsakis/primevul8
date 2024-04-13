int sqlite3ExprListCompare(ExprList *pA, ExprList *pB, int iTab){
  int i;
  if( pA==0 && pB==0 ) return 0;
  if( pA==0 || pB==0 ) return 1;
  if( pA->nExpr!=pB->nExpr ) return 1;
  for(i=0; i<pA->nExpr; i++){
    Expr *pExprA = pA->a[i].pExpr;
    Expr *pExprB = pB->a[i].pExpr;
    if( pA->a[i].sortFlags!=pB->a[i].sortFlags ) return 1;
    if( sqlite3ExprCompare(0, pExprA, pExprB, iTab) ) return 1;
  }
  return 0;
}