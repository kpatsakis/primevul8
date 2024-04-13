SrcList *sqlite3SrcListDup(sqlite3 *db, SrcList *p, int flags){
  SrcList *pNew;
  int i;
  int nByte;
  assert( db!=0 );
  if( p==0 ) return 0;
  nByte = sizeof(*p) + (p->nSrc>0 ? sizeof(p->a[0]) * (p->nSrc-1) : 0);
  pNew = sqlite3DbMallocRawNN(db, nByte );
  if( pNew==0 ) return 0;
  pNew->nSrc = pNew->nAlloc = p->nSrc;
  for(i=0; i<p->nSrc; i++){
    struct SrcList_item *pNewItem = &pNew->a[i];
    struct SrcList_item *pOldItem = &p->a[i];
    Table *pTab;
    pNewItem->pSchema = pOldItem->pSchema;
    pNewItem->zDatabase = sqlite3DbStrDup(db, pOldItem->zDatabase);
    pNewItem->zName = sqlite3DbStrDup(db, pOldItem->zName);
    pNewItem->zAlias = sqlite3DbStrDup(db, pOldItem->zAlias);
    pNewItem->fg = pOldItem->fg;
    pNewItem->iCursor = pOldItem->iCursor;
    pNewItem->addrFillSub = pOldItem->addrFillSub;
    pNewItem->regReturn = pOldItem->regReturn;
    if( pNewItem->fg.isIndexedBy ){
      pNewItem->u1.zIndexedBy = sqlite3DbStrDup(db, pOldItem->u1.zIndexedBy);
    }
    pNewItem->pIBIndex = pOldItem->pIBIndex;
    if( pNewItem->fg.isTabFunc ){
      pNewItem->u1.pFuncArg = 
          sqlite3ExprListDup(db, pOldItem->u1.pFuncArg, flags);
    }
    pTab = pNewItem->pTab = pOldItem->pTab;
    if( pTab ){
      pTab->nTabRef++;
    }
    pNewItem->pSelect = sqlite3SelectDup(db, pOldItem->pSelect, flags);
    pNewItem->pOn = sqlite3ExprDup(db, pOldItem->pOn, flags);
    pNewItem->pUsing = sqlite3IdListDup(db, pOldItem->pUsing);
    pNewItem->colUsed = pOldItem->colUsed;
  }
  return pNew;
}