static void exprToRegister(Expr *pExpr, int iReg){
  Expr *p = sqlite3ExprSkipCollateAndLikely(pExpr);
  p->op2 = p->op;
  p->op = TK_REGISTER;
  p->iTable = iReg;
  ExprClearProperty(p, EP_Skip);
}