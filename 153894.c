static int gatherSelectWindowsCallback(Walker *pWalker, Expr *pExpr){
  if( pExpr->op==TK_FUNCTION && ExprHasProperty(pExpr, EP_WinFunc) ){
    Select *pSelect = pWalker->u.pSelect;
    Window *pWin = pExpr->y.pWin;
    assert( pWin );
    assert( IsWindowFunc(pExpr) );
    assert( pWin->ppThis==0 );
    sqlite3WindowLink(pSelect, pWin);
  }
  return WRC_Continue;
}