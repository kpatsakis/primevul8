static void explainIndexRange(StrAccum *pStr, WhereLoop *pLoop){
  Index *pIndex = pLoop->u.btree.pIndex;
  u16 nEq = pLoop->u.btree.nEq;
  u16 nSkip = pLoop->nSkip;
  int i, j;

  if( nEq==0 && (pLoop->wsFlags&(WHERE_BTM_LIMIT|WHERE_TOP_LIMIT))==0 ) return;
  sqlite3_str_append(pStr, " (", 2);
  for(i=0; i<nEq; i++){
    const char *z = explainIndexColumnName(pIndex, i);
    if( i ) sqlite3_str_append(pStr, " AND ", 5);
    sqlite3_str_appendf(pStr, i>=nSkip ? "%s=?" : "ANY(%s)", z);
  }

  j = i;
  if( pLoop->wsFlags&WHERE_BTM_LIMIT ){
    explainAppendTerm(pStr, pIndex, pLoop->u.btree.nBtm, j, i, ">");
    i = 1;
  }
  if( pLoop->wsFlags&WHERE_TOP_LIMIT ){
    explainAppendTerm(pStr, pIndex, pLoop->u.btree.nTop, j, i, "<");
  }
  sqlite3_str_append(pStr, ")", 1);
}