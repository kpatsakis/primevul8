char sqlite3TableColumnAffinity(Table *pTab, int iCol){
  assert( iCol<pTab->nCol );
  return iCol>=0 ? pTab->aCol[iCol].affinity : SQLITE_AFF_INTEGER;
}