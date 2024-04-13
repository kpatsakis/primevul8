static int resolveRemoveWindowsCb(Walker *pWalker, Expr *pExpr){
  UNUSED_PARAMETER(pWalker);
  if( ExprHasProperty(pExpr, EP_WinFunc) ){
    Window *pWin = pExpr->y.pWin;
    sqlite3WindowUnlinkFromSelect(pWin);
  }
  return WRC_Continue;
}