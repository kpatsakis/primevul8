static int exprIdxCover(Walker *pWalker, Expr *pExpr){
  if( pExpr->op==TK_COLUMN
   && pExpr->iTable==pWalker->u.pIdxCover->iCur
   && sqlite3TableColumnToIndex(pWalker->u.pIdxCover->pIdx, pExpr->iColumn)<0
  ){
    pWalker->eCode = 1;
    return WRC_Abort;
  }
  return WRC_Continue;
}