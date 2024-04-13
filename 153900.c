int sqlite3ExprCheckIN(Parse *pParse, Expr *pIn){
  int nVector = sqlite3ExprVectorSize(pIn->pLeft);
  if( (pIn->flags & EP_xIsSelect) ){
    if( nVector!=pIn->x.pSelect->pEList->nExpr ){
      sqlite3SubselectError(pParse, pIn->x.pSelect->pEList->nExpr, nVector);
      return 1;
    }
  }else if( nVector!=1 ){
    sqlite3VectorErrorMsg(pParse, pIn->pLeft);
    return 1;
  }
  return 0;
}