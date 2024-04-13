ExprList *sqlite3ExprListAppend(
  Parse *pParse,          /* Parsing context */
  ExprList *pList,        /* List to which to append. Might be NULL */
  Expr *pExpr             /* Expression to be appended. Might be NULL */
){
  struct ExprList_item *pItem;
  sqlite3 *db = pParse->db;
  assert( db!=0 );
  if( pList==0 ){
    pList = sqlite3DbMallocRawNN(db, sizeof(ExprList) );
    if( pList==0 ){
      goto no_mem;
    }
    pList->nExpr = 0;
  }else if( (pList->nExpr & (pList->nExpr-1))==0 ){
    ExprList *pNew;
    pNew = sqlite3DbRealloc(db, pList, 
         sizeof(*pList)+(2*(sqlite3_int64)pList->nExpr-1)*sizeof(pList->a[0]));
    if( pNew==0 ){
      goto no_mem;
    }
    pList = pNew;
  }
  pItem = &pList->a[pList->nExpr++];
  assert( offsetof(struct ExprList_item,zName)==sizeof(pItem->pExpr) );
  assert( offsetof(struct ExprList_item,pExpr)==0 );
  memset(&pItem->zName,0,sizeof(*pItem)-offsetof(struct ExprList_item,zName));
  pItem->pExpr = pExpr;
  return pList;

no_mem:     
  /* Avoid leaking memory if malloc has failed. */
  sqlite3ExprDelete(db, pExpr);
  sqlite3ExprListDelete(db, pList);
  return 0;
}