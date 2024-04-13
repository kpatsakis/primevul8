int sqlite3MatchSpanName(
  const char *zSpan,
  const char *zCol,
  const char *zTab,
  const char *zDb
){
  int n;
  for(n=0; ALWAYS(zSpan[n]) && zSpan[n]!='.'; n++){}
  if( zDb && (sqlite3StrNICmp(zSpan, zDb, n)!=0 || zDb[n]!=0) ){
    return 0;
  }
  zSpan += n+1;
  for(n=0; ALWAYS(zSpan[n]) && zSpan[n]!='.'; n++){}
  if( zTab && (sqlite3StrNICmp(zSpan, zTab, n)!=0 || zTab[n]!=0) ){
    return 0;
  }
  zSpan += n+1;
  if( zCol && sqlite3StrICmp(zSpan, zCol)!=0 ){
    return 0;
  }
  return 1;
}