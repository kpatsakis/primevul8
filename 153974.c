ExprList *sqlite3ExprListDup(sqlite3 *db, ExprList *p, int flags){
  ExprList *pNew;
  struct ExprList_item *pItem, *pOldItem;
  int i;
  Expr *pPriorSelectCol = 0;
  assert( db!=0 );
  if( p==0 ) return 0;
  pNew = sqlite3DbMallocRawNN(db, sqlite3DbMallocSize(db, p));
  if( pNew==0 ) return 0;
  pNew->nExpr = p->nExpr;
  pItem = pNew->a;
  pOldItem = p->a;
  for(i=0; i<p->nExpr; i++, pItem++, pOldItem++){
    Expr *pOldExpr = pOldItem->pExpr;
    Expr *pNewExpr;
    pItem->pExpr = sqlite3ExprDup(db, pOldExpr, flags);
    if( pOldExpr 
     && pOldExpr->op==TK_SELECT_COLUMN
     && (pNewExpr = pItem->pExpr)!=0 
    ){
      assert( pNewExpr->iColumn==0 || i>0 );
      if( pNewExpr->iColumn==0 ){
        assert( pOldExpr->pLeft==pOldExpr->pRight );
        pPriorSelectCol = pNewExpr->pLeft = pNewExpr->pRight;
      }else{
        assert( i>0 );
        assert( pItem[-1].pExpr!=0 );
        assert( pNewExpr->iColumn==pItem[-1].pExpr->iColumn+1 );
        assert( pPriorSelectCol==pItem[-1].pExpr->pLeft );
        pNewExpr->pLeft = pPriorSelectCol;
      }
    }
    pItem->zName = sqlite3DbStrDup(db, pOldItem->zName);
    pItem->zSpan = sqlite3DbStrDup(db, pOldItem->zSpan);
    pItem->sortFlags = pOldItem->sortFlags;
    pItem->done = 0;
    pItem->bNulls = pOldItem->bNulls;
    pItem->bSpanIsTab = pOldItem->bSpanIsTab;
    pItem->bSorterRef = pOldItem->bSorterRef;
    pItem->u = pOldItem->u;
  }
  return pNew;
}