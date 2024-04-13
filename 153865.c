static int codeCursorHintIsOrFunction(Walker *pWalker, Expr *pExpr){
  if( pExpr->op==TK_IS 
   || pExpr->op==TK_ISNULL || pExpr->op==TK_ISNOT 
   || pExpr->op==TK_NOTNULL || pExpr->op==TK_CASE 
  ){
    pWalker->eCode = 1;
  }else if( pExpr->op==TK_FUNCTION ){
    int d1;
    char d2[4];
    if( 0==sqlite3IsLikeFunction(pWalker->pParse->db, pExpr, &d1, d2) ){
      pWalker->eCode = 1;
    }
  }

  return WRC_Continue;
}