static void resolveOutOfRangeError(
  Parse *pParse,         /* The error context into which to write the error */
  const char *zType,     /* "ORDER" or "GROUP" */
  int i,                 /* The index (1-based) of the term out of range */
  int mx                 /* Largest permissible value of i */
){
  sqlite3ErrorMsg(pParse, 
    "%r %s BY term out of range - should be "
    "between 1 and %d", i, zType, mx);
}