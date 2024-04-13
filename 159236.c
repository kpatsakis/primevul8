static void incrAggFunctionDepth(Expr *pExpr, int N){
  if( N>0 ){
    Walker w;
    memset(&w, 0, sizeof(w));
    w.xExprCallback = incrAggDepth;
    w.u.n = N;
    sqlite3WalkExpr(&w, pExpr);
  }
}