static int havingToWhereExprCb(Walker *pWalker, Expr *pExpr){
  if( pExpr->op!=TK_AND ){
    Select *pS = pWalker->u.pSelect;
    if( sqlite3ExprIsConstantOrGroupBy(pWalker->pParse, pExpr, pS->pGroupBy) ){
      sqlite3 *db = pWalker->pParse->db;
      Expr *pNew = sqlite3ExprAlloc(db, TK_INTEGER, &sqlite3IntTokens[1], 0);
      if( pNew ){
        Expr *pWhere = pS->pWhere;
        SWAP(Expr, *pNew, *pExpr);
        pNew = sqlite3ExprAnd(db, pWhere, pNew);
        pS->pWhere = pNew;
        pWalker->eCode = 1;
      }
    }
    return WRC_Prune;
  }
  return WRC_Continue;
}