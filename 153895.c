static int exprVectorRegister(
  Parse *pParse,                  /* Parse context */
  Expr *pVector,                  /* Vector to extract element from */
  int iField,                     /* Field to extract from pVector */
  int regSelect,                  /* First in array of registers */
  Expr **ppExpr,                  /* OUT: Expression element */
  int *pRegFree                   /* OUT: Temp register to free */
){
  u8 op = pVector->op;
  assert( op==TK_VECTOR || op==TK_REGISTER || op==TK_SELECT );
  if( op==TK_REGISTER ){
    *ppExpr = sqlite3VectorFieldSubexpr(pVector, iField);
    return pVector->iTable+iField;
  }
  if( op==TK_SELECT ){
    *ppExpr = pVector->x.pSelect->pEList->a[iField].pExpr;
     return regSelect+iField;
  }
  *ppExpr = pVector->x.pList->a[iField].pExpr;
  return sqlite3ExprCodeTemp(pParse, *ppExpr, pRegFree);
}