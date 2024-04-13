static void exprSetHeight(Expr *p){
  int nHeight = 0;
  heightOfExpr(p->pLeft, &nHeight);
  heightOfExpr(p->pRight, &nHeight);
  if( ExprHasProperty(p, EP_xIsSelect) ){
    heightOfSelect(p->x.pSelect, &nHeight);
  }else if( p->x.pList ){
    heightOfExprList(p->x.pList, &nHeight);
    p->flags |= EP_Propagate & sqlite3ExprListFlags(p->x.pList);
  }
  p->nHeight = nHeight + 1;
}