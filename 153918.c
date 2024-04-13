void sqlite3ExprCodeFactorable(Parse *pParse, Expr *pExpr, int target){
  if( pParse->okConstFactor && sqlite3ExprIsConstantNotJoin(pExpr) ){
    sqlite3ExprCodeAtInit(pParse, pExpr, target);
  }else{
    sqlite3ExprCode(pParse, pExpr, target);
  }
}