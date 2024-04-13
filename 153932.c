int sqlite3ExprVectorSize(Expr *pExpr){
  u8 op = pExpr->op;
  if( op==TK_REGISTER ) op = pExpr->op2;
  if( op==TK_VECTOR ){
    return pExpr->x.pList->nExpr;
  }else if( op==TK_SELECT ){
    return pExpr->x.pSelect->pEList->nExpr;
  }else{
    return 1;
  }
}