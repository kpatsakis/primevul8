static int exprIsConst(Expr *p, int initFlag, int iCur){
  Walker w;
  w.eCode = initFlag;
  w.xExprCallback = exprNodeIsConstant;
  w.xSelectCallback = sqlite3SelectWalkFail;
#ifdef SQLITE_DEBUG
  w.xSelectCallback2 = sqlite3SelectWalkAssert2;
#endif
  w.u.iCur = iCur;
  sqlite3WalkExpr(&w, p);
  return w.eCode;
}