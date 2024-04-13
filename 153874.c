static int dupedExprSize(Expr *p, int flags){
  int nByte = 0;
  if( p ){
    nByte = dupedExprNodeSize(p, flags);
    if( flags&EXPRDUP_REDUCE ){
      nByte += dupedExprSize(p->pLeft, flags) + dupedExprSize(p->pRight, flags);
    }
  }
  return nByte;
}