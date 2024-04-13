static int exprCodeSubselect(Parse *pParse, Expr *pExpr){
  int reg = 0;
#ifndef SQLITE_OMIT_SUBQUERY
  if( pExpr->op==TK_SELECT ){
    reg = sqlite3CodeSubselect(pParse, pExpr);
  }
#endif
  return reg;
}