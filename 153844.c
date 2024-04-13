Expr *sqlite3ExprSkipCollateAndLikely(Expr *pExpr){
  while( pExpr && ExprHasProperty(pExpr, EP_Skip|EP_Unlikely) ){
    if( ExprHasProperty(pExpr, EP_Unlikely) ){
      assert( !ExprHasProperty(pExpr, EP_xIsSelect) );
      assert( pExpr->x.pList->nExpr>0 );
      assert( pExpr->op==TK_FUNCTION );
      pExpr = pExpr->x.pList->a[0].pExpr;
    }else{
      assert( pExpr->op==TK_COLLATE );
      pExpr = pExpr->pLeft;
    }
  }   
  return pExpr;
}