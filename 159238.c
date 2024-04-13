static void notValid(
  Parse *pParse,       /* Leave error message here */
  NameContext *pNC,    /* The name context */
  const char *zMsg,    /* Type of error */
  int validMask        /* Set of contexts for which prohibited */
){
  assert( (validMask&~(NC_IsCheck|NC_PartIdx|NC_IdxExpr|NC_GenCol))==0 );
  if( (pNC->ncFlags & validMask)!=0 ){
    const char *zIn = "partial index WHERE clauses";
    if( pNC->ncFlags & NC_IdxExpr )      zIn = "index expressions";
#ifndef SQLITE_OMIT_CHECK
    else if( pNC->ncFlags & NC_IsCheck ) zIn = "CHECK constraints";
#endif
#ifndef SQLITE_OMIT_GENERATED_COLUMNS
    else if( pNC->ncFlags & NC_GenCol ) zIn = "generated columns";
#endif
    sqlite3ErrorMsg(pParse, "%s prohibited in %s", zMsg, zIn);
  }
}