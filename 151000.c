static int propagateConstantExprRewrite(Walker *pWalker, Expr *pExpr){
  int i;
  WhereConst *pConst;
  if( pExpr->op!=TK_COLUMN ) return WRC_Continue;
  pConst = pWalker->u.pConst;
  for(i=0; i<pConst->nConst; i++){
    Expr *pColumn = pConst->apExpr[i*2];
    if( pColumn==pExpr ) continue;
    if( pColumn->iTable!=pExpr->iTable ) continue;
    if( pColumn->iColumn!=pExpr->iColumn ) continue;
    /* A match is found.  Transform the COLUMN into a CONSTANT */
    pConst->nChng++;
    ExprClearProperty(pExpr, EP_Leaf);
    pExpr->op = TK_UPLUS;
    pExpr->pLeft = sqlite3ExprDup(pConst->db, pConst->apExpr[i*2+1], 0);
    break;
  }
  return WRC_Prune;
}