int sqlite3ExprIsConstant(Expr *p){
  return exprIsConst(p, 1, 0);
}