static const char *explainIndexColumnName(Index *pIdx, int i){
  i = pIdx->aiColumn[i];
  if( i==XN_EXPR ) return "<expr>";
  if( i==XN_ROWID ) return "rowid";
  return pIdx->pTable->aCol[i].zName;
}