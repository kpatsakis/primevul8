Expr *sqlite3ExprAddCollateToken(
  Parse *pParse,           /* Parsing context */
  Expr *pExpr,             /* Add the "COLLATE" clause to this expression */
  const Token *pCollName,  /* Name of collating sequence */
  int dequote              /* True to dequote pCollName */
){
  if( pCollName->n>0 ){
    Expr *pNew = sqlite3ExprAlloc(pParse->db, TK_COLLATE, pCollName, dequote);
    if( pNew ){
      pNew->pLeft = pExpr;
      pNew->flags |= EP_Collate|EP_Skip;
      pExpr = pNew;
    }
  }
  return pExpr;
}