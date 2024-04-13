int sqlite3ExprCodeAtInit(
  Parse *pParse,    /* Parsing context */
  Expr *pExpr,      /* The expression to code when the VDBE initializes */
  int regDest       /* Store the value in this register */
){
  ExprList *p;
  assert( ConstFactorOk(pParse) );
  p = pParse->pConstExpr;
  if( regDest<0 && p ){
    struct ExprList_item *pItem;
    int i;
    for(pItem=p->a, i=p->nExpr; i>0; pItem++, i--){
      if( pItem->reusable && sqlite3ExprCompare(0,pItem->pExpr,pExpr,-1)==0 ){
        return pItem->u.iConstExprReg;
      }
    }
  }
  pExpr = sqlite3ExprDup(pParse->db, pExpr, 0);
  p = sqlite3ExprListAppend(pParse, p, pExpr);
  if( p ){
     struct ExprList_item *pItem = &p->a[p->nExpr-1];
     pItem->reusable = regDest<0;
     if( regDest<0 ) regDest = ++pParse->nMem;
     pItem->u.iConstExprReg = regDest;
  }
  pParse->pConstExpr = p;
  return regDest;
}