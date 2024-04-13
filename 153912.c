int sqlite3ExprCompareSkip(Expr *pA, Expr *pB, int iTab){
  return sqlite3ExprCompare(0,
             sqlite3ExprSkipCollateAndLikely(pA),
             sqlite3ExprSkipCollateAndLikely(pB),
             iTab);
}