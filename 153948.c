void sqlite3VectorErrorMsg(Parse *pParse, Expr *pExpr){
#ifndef SQLITE_OMIT_SUBQUERY
  if( pExpr->flags & EP_xIsSelect ){
    sqlite3SubselectError(pParse, pExpr->x.pSelect->pEList->nExpr, 1);
  }else
#endif
  {
    sqlite3ErrorMsg(pParse, "row value misused");
  }
}