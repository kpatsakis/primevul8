int sqlite3FunctionUsesThisSrc(Expr *pExpr, SrcList *pSrcList){
  Walker w;
  struct SrcCount cnt;
  assert( pExpr->op==TK_AGG_FUNCTION );
  memset(&w, 0, sizeof(w));
  w.xExprCallback = exprSrcCount;
  w.xSelectCallback = sqlite3SelectWalkNoop;
  w.u.pSrcCount = &cnt;
  cnt.pSrc = pSrcList;
  cnt.nThis = 0;
  cnt.nOther = 0;
  sqlite3WalkExprList(&w, pExpr->x.pList);
  return cnt.nThis>0 || cnt.nOther==0;
}