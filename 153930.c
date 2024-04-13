static int sqlite3InRhsIsConstant(Expr *pIn){
  Expr *pLHS;
  int res;
  assert( !ExprHasProperty(pIn, EP_xIsSelect) );
  pLHS = pIn->pLeft;
  pIn->pLeft = 0;
  res = sqlite3ExprIsConstant(pIn);
  pIn->pLeft = pLHS;
  return res;
}