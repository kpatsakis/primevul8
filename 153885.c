int sqlite3ExprTruthValue(const Expr *pExpr){
  pExpr = sqlite3ExprSkipCollate((Expr*)pExpr);
  assert( pExpr->op==TK_TRUEFALSE );
  assert( sqlite3StrICmp(pExpr->u.zToken,"true")==0
       || sqlite3StrICmp(pExpr->u.zToken,"false")==0 );
  return pExpr->u.zToken[4]==0;
}