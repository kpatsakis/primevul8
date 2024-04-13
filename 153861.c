void sqlite3ExprListSetSortOrder(ExprList *p, int iSortOrder, int eNulls){
  struct ExprList_item *pItem;
  if( p==0 ) return;
  assert( p->nExpr>0 );

  assert( SQLITE_SO_UNDEFINED<0 && SQLITE_SO_ASC==0 && SQLITE_SO_DESC>0 );
  assert( iSortOrder==SQLITE_SO_UNDEFINED 
       || iSortOrder==SQLITE_SO_ASC 
       || iSortOrder==SQLITE_SO_DESC 
  );
  assert( eNulls==SQLITE_SO_UNDEFINED 
       || eNulls==SQLITE_SO_ASC 
       || eNulls==SQLITE_SO_DESC 
  );

  pItem = &p->a[p->nExpr-1];
  assert( pItem->bNulls==0 );
  if( iSortOrder==SQLITE_SO_UNDEFINED ){
    iSortOrder = SQLITE_SO_ASC;
  }
  pItem->sortFlags = (u8)iSortOrder;

  if( eNulls!=SQLITE_SO_UNDEFINED ){
    pItem->bNulls = 1;
    if( iSortOrder!=eNulls ){
      pItem->sortFlags |= KEYINFO_ORDER_BIGNULL;
    }
  }
}