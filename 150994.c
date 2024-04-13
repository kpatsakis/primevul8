Table *sqlite3ResultSetOfSelect(Parse *pParse, Select *pSelect){
  Table *pTab;
  sqlite3 *db = pParse->db;
  int savedFlags;

  savedFlags = db->flags;
  db->flags &= ~SQLITE_FullColNames;
  db->flags |= SQLITE_ShortColNames;
  sqlite3SelectPrep(pParse, pSelect, 0);
  if( pParse->nErr ) return 0;
  while( pSelect->pPrior ) pSelect = pSelect->pPrior;
  db->flags = savedFlags;
  pTab = sqlite3DbMallocZero(db, sizeof(Table) );
  if( pTab==0 ){
    return 0;
  }
  /* The sqlite3ResultSetOfSelect() is only used n contexts where lookaside
  ** is disabled */
  assert( db->lookaside.bDisable );
  pTab->nTabRef = 1;
  pTab->zName = 0;
  pTab->nRowLogEst = 200; assert( 200==sqlite3LogEst(1048576) );
  sqlite3ColumnsFromExprList(pParse, pSelect->pEList, &pTab->nCol, &pTab->aCol);
  sqlite3SelectAddColumnTypeAndCollation(pParse, pTab, pSelect);
  pTab->iPKey = -1;
  if( db->mallocFailed ){
    sqlite3DeleteTable(db, pTab);
    return 0;
  }
  return pTab;
}