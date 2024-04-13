static void whereApplyPartialIndexConstraints(
  Expr *pTruth,
  int iTabCur,
  WhereClause *pWC
){
  int i;
  WhereTerm *pTerm;
  while( pTruth->op==TK_AND ){
    whereApplyPartialIndexConstraints(pTruth->pLeft, iTabCur, pWC);
    pTruth = pTruth->pRight;
  }
  for(i=0, pTerm=pWC->a; i<pWC->nTerm; i++, pTerm++){
    Expr *pExpr;
    if( pTerm->wtFlags & TERM_CODED ) continue;
    pExpr = pTerm->pExpr;
    if( sqlite3ExprCompare(0, pExpr, pTruth, iTabCur)==0 ){
      pTerm->wtFlags |= TERM_CODED;
    }
  }
}