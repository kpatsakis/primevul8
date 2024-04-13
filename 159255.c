static int nameInUsingClause(IdList *pUsing, const char *zCol){
  if( pUsing ){
    int k;
    for(k=0; k<pUsing->nId; k++){
      if( sqlite3StrICmp(pUsing->a[k].zName, zCol)==0 ) return 1;
    }
  }
  return 0;
}