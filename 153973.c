int sqlite3ExprCoveredByIndex(
  Expr *pExpr,        /* The index to be tested */
  int iCur,           /* The cursor number for the corresponding table */
  Index *pIdx         /* The index that might be used for coverage */
){
  Walker w;
  struct IdxCover xcov;
  memset(&w, 0, sizeof(w));
  xcov.iCur = iCur;
  xcov.pIdx = pIdx;
  w.xExprCallback = exprIdxCover;
  w.u.pIdxCover = &xcov;
  sqlite3WalkExpr(&w, pExpr);
  return !w.eCode;
}