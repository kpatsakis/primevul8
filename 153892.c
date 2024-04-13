static void explainAppendTerm(
  StrAccum *pStr,             /* The text expression being built */
  Index *pIdx,                /* Index to read column names from */
  int nTerm,                  /* Number of terms */
  int iTerm,                  /* Zero-based index of first term. */
  int bAnd,                   /* Non-zero to append " AND " */
  const char *zOp             /* Name of the operator */
){
  int i;

  assert( nTerm>=1 );
  if( bAnd ) sqlite3_str_append(pStr, " AND ", 5);

  if( nTerm>1 ) sqlite3_str_append(pStr, "(", 1);
  for(i=0; i<nTerm; i++){
    if( i ) sqlite3_str_append(pStr, ",", 1);
    sqlite3_str_appendall(pStr, explainIndexColumnName(pIdx, iTerm+i));
  }
  if( nTerm>1 ) sqlite3_str_append(pStr, ")", 1);

  sqlite3_str_append(pStr, zOp, 1);

  if( nTerm>1 ) sqlite3_str_append(pStr, "(", 1);
  for(i=0; i<nTerm; i++){
    if( i ) sqlite3_str_append(pStr, ",", 1);
    sqlite3_str_append(pStr, "?", 1);
  }
  if( nTerm>1 ) sqlite3_str_append(pStr, ")", 1);
}