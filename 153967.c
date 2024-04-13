int sqlite3ExprIsConstantOrFunction(Expr *p, u8 isInit){
  assert( isInit==0 || isInit==1 );
  return exprIsConst(p, 4+isInit, 0);
}