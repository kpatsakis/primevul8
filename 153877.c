int sqlite3IndexAffinityOk(Expr *pExpr, char idx_affinity){
  char aff = comparisonAffinity(pExpr);
  if( aff<SQLITE_AFF_TEXT ){
    return 1;
  }
  if( aff==SQLITE_AFF_TEXT ){
    return idx_affinity==SQLITE_AFF_TEXT;
  }
  return sqlite3IsNumericAffinity(idx_affinity);
}