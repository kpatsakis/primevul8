int sqlite3ExprIsVector(Expr *pExpr){
  return sqlite3ExprVectorSize(pExpr)>1;
}