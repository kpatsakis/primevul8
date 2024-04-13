void sqlite3ExprSetHeightAndFlags(Parse *pParse, Expr *p){
  if( pParse->nErr ) return;
  exprSetHeight(p);
  sqlite3ExprCheckHeight(pParse, p->nHeight);
}