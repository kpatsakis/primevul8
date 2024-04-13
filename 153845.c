int sqlite3ExprIsConstantNotJoin(Expr *p){
  return exprIsConst(p, 2, 0);
}