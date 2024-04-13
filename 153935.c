void sqlite3ExprListSetName(
  Parse *pParse,          /* Parsing context */
  ExprList *pList,        /* List to which to add the span. */
  Token *pName,           /* Name to be added */
  int dequote             /* True to cause the name to be dequoted */
){
  assert( pList!=0 || pParse->db->mallocFailed!=0 );
  if( pList ){
    struct ExprList_item *pItem;
    assert( pList->nExpr>0 );
    pItem = &pList->a[pList->nExpr-1];
    assert( pItem->zName==0 );
    pItem->zName = sqlite3DbStrNDup(pParse->db, pName->z, pName->n);
    if( dequote ) sqlite3Dequote(pItem->zName);
    if( IN_RENAME_OBJECT ){
      sqlite3RenameTokenMap(pParse, (void*)pItem->zName, pName);
    }
  }
}