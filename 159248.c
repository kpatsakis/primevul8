static void windowRemoveExprFromSelect(Select *pSelect, Expr *pExpr){
  if( pSelect->pWin ){
    Walker sWalker;
    memset(&sWalker, 0, sizeof(Walker));
    sWalker.xExprCallback = resolveRemoveWindowsCb;
    sWalker.u.pSelect = pSelect;
    sqlite3WalkExpr(&sWalker, pExpr);
  }
}