Expr *sqlite3VectorFieldSubexpr(Expr *pVector, int i){
  assert( i<sqlite3ExprVectorSize(pVector) );
  if( sqlite3ExprIsVector(pVector) ){
    assert( pVector->op2==0 || pVector->op==TK_REGISTER );
    if( pVector->op==TK_SELECT || pVector->op2==TK_SELECT ){
      return pVector->x.pSelect->pEList->a[i].pExpr;
    }else{
      return pVector->x.pList->a[i].pExpr;
    }
  }
  return pVector;
}