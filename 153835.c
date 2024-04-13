char sqlite3ExprAffinity(Expr *pExpr){
  int op;
  while( ExprHasProperty(pExpr, EP_Skip) ){
    assert( pExpr->op==TK_COLLATE );
    pExpr = pExpr->pLeft;
    assert( pExpr!=0 );
  }
  op = pExpr->op;
  if( op==TK_SELECT ){
    assert( pExpr->flags&EP_xIsSelect );
    return sqlite3ExprAffinity(pExpr->x.pSelect->pEList->a[0].pExpr);
  }
  if( op==TK_REGISTER ) op = pExpr->op2;
#ifndef SQLITE_OMIT_CAST
  if( op==TK_CAST ){
    assert( !ExprHasProperty(pExpr, EP_IntValue) );
    return sqlite3AffinityType(pExpr->u.zToken, 0);
  }
#endif
  if( (op==TK_AGG_COLUMN || op==TK_COLUMN) && pExpr->y.pTab ){
    return sqlite3TableColumnAffinity(pExpr->y.pTab, pExpr->iColumn);
  }
  if( op==TK_SELECT_COLUMN ){
    assert( pExpr->pLeft->flags&EP_xIsSelect );
    return sqlite3ExprAffinity(
        pExpr->pLeft->x.pSelect->pEList->a[pExpr->iColumn].pExpr
    );
  }
  if( op==TK_VECTOR ){
    return sqlite3ExprAffinity(pExpr->x.pList->a[0].pExpr);
  }
  return pExpr->affExpr;
}