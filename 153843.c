static SQLITE_NOINLINE void exprListDeleteNN(sqlite3 *db, ExprList *pList){
  int i = pList->nExpr;
  struct ExprList_item *pItem =  pList->a;
  assert( pList->nExpr>0 );
  do{
    sqlite3ExprDelete(db, pItem->pExpr);
    sqlite3DbFree(db, pItem->zName);
    sqlite3DbFree(db, pItem->zSpan);
    pItem++;
  }while( --i>0 );
  sqlite3DbFreeNN(db, pList);
}