static With *withDup(sqlite3 *db, With *p){
  With *pRet = 0;
  if( p ){
    sqlite3_int64 nByte = sizeof(*p) + sizeof(p->a[0]) * (p->nCte-1);
    pRet = sqlite3DbMallocZero(db, nByte);
    if( pRet ){
      int i;
      pRet->nCte = p->nCte;
      for(i=0; i<p->nCte; i++){
        pRet->a[i].pSelect = sqlite3SelectDup(db, p->a[i].pSelect, 0);
        pRet->a[i].pCols = sqlite3ExprListDup(db, p->a[i].pCols, 0);
        pRet->a[i].zName = sqlite3DbStrDup(db, p->a[i].zName);
      }
    }
  }
  return pRet;
}