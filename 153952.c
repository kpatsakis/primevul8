int sqlite3ExprImpliesNonNullRow(Expr *p, int iTab){
  Walker w;
  p = sqlite3ExprSkipCollateAndLikely(p);
  if( p==0 ) return 0;
  if( p->op==TK_NOTNULL ){
    p = p->pLeft;
  }else{
    while( p->op==TK_AND ){
      if( sqlite3ExprImpliesNonNullRow(p->pLeft, iTab) ) return 1;
      p = p->pRight;
    }
  }
  w.xExprCallback = impliesNotNullRow;
  w.xSelectCallback = 0;
  w.xSelectCallback2 = 0;
  w.eCode = 0;
  w.u.iCur = iTab;
  sqlite3WalkExpr(&w, p);
  return w.eCode;
}