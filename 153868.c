int sqlite3ExprIdToTrueFalse(Expr *pExpr){
  assert( pExpr->op==TK_ID || pExpr->op==TK_STRING );
  if( !ExprHasProperty(pExpr, EP_Quoted)
   && (sqlite3StrICmp(pExpr->u.zToken, "true")==0
       || sqlite3StrICmp(pExpr->u.zToken, "false")==0)
  ){
    pExpr->op = TK_TRUEFALSE;
    ExprSetProperty(pExpr, pExpr->u.zToken[4]==0 ? EP_IsTrue : EP_IsFalse);
    return 1;
  }
  return 0;
}