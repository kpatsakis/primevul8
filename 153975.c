static int codeCursorHintCheckExpr(Walker *pWalker, Expr *pExpr){
  struct CCurHint *pHint = pWalker->u.pCCurHint;
  assert( pHint->pIdx!=0 );
  if( pExpr->op==TK_COLUMN
   && pExpr->iTable==pHint->iTabCur
   && sqlite3TableColumnToIndex(pHint->pIdx, pExpr->iColumn)<0
  ){
    pWalker->eCode = 1;
  }
  return WRC_Continue;
}