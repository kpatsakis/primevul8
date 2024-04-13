static char comparisonAffinity(Expr *pExpr){
  char aff;
  assert( pExpr->op==TK_EQ || pExpr->op==TK_IN || pExpr->op==TK_LT ||
          pExpr->op==TK_GT || pExpr->op==TK_GE || pExpr->op==TK_LE ||
          pExpr->op==TK_NE || pExpr->op==TK_IS || pExpr->op==TK_ISNOT );
  assert( pExpr->pLeft );
  aff = sqlite3ExprAffinity(pExpr->pLeft);
  if( pExpr->pRight ){
    aff = sqlite3CompareAffinity(pExpr->pRight, aff);
  }else if( ExprHasProperty(pExpr, EP_xIsSelect) ){
    aff = sqlite3CompareAffinity(pExpr->x.pSelect->pEList->a[0].pExpr, aff);
  }else if( aff==0 ){
    aff = SQLITE_AFF_BLOB;
  }
  return aff;
}