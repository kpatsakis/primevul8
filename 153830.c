static void updateRangeAffinityStr(
  Expr *pRight,                   /* RHS of comparison */
  int n,                          /* Number of vector elements in comparison */
  char *zAff                      /* Affinity string to modify */
){
  int i;
  for(i=0; i<n; i++){
    Expr *p = sqlite3VectorFieldSubexpr(pRight, i);
    if( sqlite3CompareAffinity(p, zAff[i])==SQLITE_AFF_BLOB
     || sqlite3ExprNeedsNoAffinityChange(p, zAff[i])
    ){
      zAff[i] = SQLITE_AFF_BLOB;
    }
  }
}