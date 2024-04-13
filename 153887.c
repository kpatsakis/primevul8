IdList *sqlite3IdListDup(sqlite3 *db, IdList *p){
  IdList *pNew;
  int i;
  assert( db!=0 );
  if( p==0 ) return 0;
  pNew = sqlite3DbMallocRawNN(db, sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->nId = p->nId;
  pNew->a = sqlite3DbMallocRawNN(db, p->nId*sizeof(p->a[0]) );
  if( pNew->a==0 ){
    sqlite3DbFreeNN(db, pNew);
    return 0;
  }
  /* Note that because the size of the allocation for p->a[] is not
  ** necessarily a power of two, sqlite3IdListAppend() may not be called
  ** on the duplicate created by this function. */
  for(i=0; i<p->nId; i++){
    struct IdList_item *pNewItem = &pNew->a[i];
    struct IdList_item *pOldItem = &p->a[i];
    pNewItem->zName = sqlite3DbStrDup(db, pOldItem->zName);
    pNewItem->idx = pOldItem->idx;
  }
  return pNew;
}