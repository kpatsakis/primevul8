static int codeCursorHintFixExpr(Walker *pWalker, Expr *pExpr){
  int rc = WRC_Continue;
  struct CCurHint *pHint = pWalker->u.pCCurHint;
  if( pExpr->op==TK_COLUMN ){
    if( pExpr->iTable!=pHint->iTabCur ){
      int reg = ++pWalker->pParse->nMem;   /* Register for column value */
      sqlite3ExprCode(pWalker->pParse, pExpr, reg);
      pExpr->op = TK_REGISTER;
      pExpr->iTable = reg;
    }else if( pHint->pIdx!=0 ){
      pExpr->iTable = pHint->iIdxCur;
      pExpr->iColumn = sqlite3TableColumnToIndex(pHint->pIdx, pExpr->iColumn);
      assert( pExpr->iColumn>=0 );
    }
  }else if( pExpr->op==TK_AGG_FUNCTION ){
    /* An aggregate function in the WHERE clause of a query means this must
    ** be a correlated sub-query, and expression pExpr is an aggregate from
    ** the parent context. Do not walk the function arguments in this case.
    **
    ** todo: It should be possible to replace this node with a TK_REGISTER
    ** expression, as the result of the expression must be stored in a 
    ** register at this point. The same holds for TK_AGG_COLUMN nodes. */
    rc = WRC_Prune;
  }
  return rc;
}