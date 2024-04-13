void sqlite3ExprSetHeightAndFlags(Parse *pParse, Expr *p){
  if( p && p->x.pList && !ExprHasProperty(p, EP_xIsSelect) ){
    p->flags |= EP_Propagate & sqlite3ExprListFlags(p->x.pList);
  }
}