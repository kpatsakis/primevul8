int sqlite3ExprIsTableConstant(Expr *p, int iCur){
  return exprIsConst(p, 3, iCur);
}