Expr *sqlite3ExprSkipCollate(Expr *pExpr){
  while( pExpr && ExprHasProperty(pExpr, EP_Skip) ){
    assert( pExpr->op==TK_COLLATE );
    pExpr = pExpr->pLeft;
  }   
  return pExpr;
}