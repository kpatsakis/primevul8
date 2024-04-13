static void constInsert(
  WhereConst *pConst,
  Expr *pColumn,
  Expr *pValue
){
  pConst->nConst++;
  pConst->apExpr = sqlite3DbReallocOrFree(pConst->db, pConst->apExpr,
                         pConst->nConst*2*sizeof(Expr*));
  if( pConst->apExpr==0 ){
    pConst->nConst = 0;
  }else{
    pConst->apExpr[pConst->nConst*2-2] = pColumn;
    pConst->apExpr[pConst->nConst*2-1] = pValue;
  }
}