int sqlite3ExprIsConstantOrGroupBy(Parse *pParse, Expr *p, ExprList *pGroupBy){
  Walker w;
  w.eCode = 1;
  w.xExprCallback = exprNodeIsConstantOrGroupBy;
  w.xSelectCallback = 0;
  w.u.pGroupBy = pGroupBy;
  w.pParse = pParse;
  sqlite3WalkExpr(&w, p);
  return w.eCode;
}