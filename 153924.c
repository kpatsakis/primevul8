static char *exprINAffinity(Parse *pParse, Expr *pExpr){
  Expr *pLeft = pExpr->pLeft;
  int nVal = sqlite3ExprVectorSize(pLeft);
  Select *pSelect = (pExpr->flags & EP_xIsSelect) ? pExpr->x.pSelect : 0;
  char *zRet;

  assert( pExpr->op==TK_IN );
  zRet = sqlite3DbMallocRaw(pParse->db, nVal+1);
  if( zRet ){
    int i;
    for(i=0; i<nVal; i++){
      Expr *pA = sqlite3VectorFieldSubexpr(pLeft, i);
      char a = sqlite3ExprAffinity(pA);
      if( pSelect ){
        zRet[i] = sqlite3CompareAffinity(pSelect->pEList->a[i].pExpr, a);
      }else{
        zRet[i] = a;
      }
    }
    zRet[nVal] = '\0';
  }
  return zRet;
}