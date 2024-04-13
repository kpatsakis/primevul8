void sqlite3ExprAnalyzeAggregates(NameContext *pNC, Expr *pExpr){
  Walker w;
  w.xExprCallback = analyzeAggregate;
  w.xSelectCallback = analyzeAggregatesInSelect;
  w.xSelectCallback2 = analyzeAggregatesInSelectEnd;
  w.walkerDepth = 0;
  w.u.pNC = pNC;
  w.pParse = 0;
  assert( pNC->pSrcList!=0 );
  sqlite3WalkExpr(&w, pExpr);
}