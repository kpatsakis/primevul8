static void findConstInWhere(WhereConst *pConst, Expr *pExpr){
  if( pExpr==0 ) return;
  if( ExprHasProperty(pExpr, EP_FromJoin) ) return;
  if( pExpr->op==TK_AND ){
    findConstInWhere(pConst, pExpr->pRight);
    findConstInWhere(pConst, pExpr->pLeft);
    return;
  }
  if( pExpr->op!=TK_EQ ) return;
  assert( pExpr->pRight!=0 );
  assert( pExpr->pLeft!=0 );
  if( pExpr->pRight->op==TK_COLUMN && sqlite3ExprIsConstant(pExpr->pLeft) ){
    constInsert(pConst, pExpr->pRight, pExpr->pLeft);
  }else
  if( pExpr->pLeft->op==TK_COLUMN && sqlite3ExprIsConstant(pExpr->pRight) ){
    constInsert(pConst, pExpr->pLeft, pExpr->pRight);
  }
}