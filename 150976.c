static struct SrcList_item *isSelfJoinView(
  SrcList *pTabList,           /* Search for self-joins in this FROM clause */
  struct SrcList_item *pThis   /* Search for prior reference to this subquery */
){
  struct SrcList_item *pItem;
  for(pItem = pTabList->a; pItem<pThis; pItem++){
    if( pItem->pSelect==0 ) continue;
    if( pItem->fg.viaCoroutine ) continue;
    if( pItem->zName==0 ) continue;
    if( sqlite3_stricmp(pItem->zDatabase, pThis->zDatabase)!=0 ) continue;
    if( sqlite3_stricmp(pItem->zName, pThis->zName)!=0 ) continue;
    if( sqlite3ExprCompare(0, 
          pThis->pSelect->pWhere, pItem->pSelect->pWhere, -1) 
    ){
      /* The view was modified by some other optimization such as
      ** pushDownWhereTerms() */
      continue;
    }
    return pItem;
  }
  return 0;
}