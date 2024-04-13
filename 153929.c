static int whereIndexExprTransNode(Walker *p, Expr *pExpr){
  IdxExprTrans *pX = p->u.pIdxTrans;
  if( sqlite3ExprCompare(0, pExpr, pX->pIdxExpr, pX->iTabCur)==0 ){
    pExpr->affExpr = sqlite3ExprAffinity(pExpr);
    pExpr->op = TK_COLUMN;
    pExpr->iTable = pX->iIdxCur;
    pExpr->iColumn = pX->iIdxCol;
    pExpr->y.pTab = 0;
    return WRC_Prune;
  }else{
    return WRC_Continue;
  }
}