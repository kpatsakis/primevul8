static int dupedExprNodeSize(Expr *p, int flags){
  int nByte = dupedExprStructSize(p, flags) & 0xfff;
  if( !ExprHasProperty(p, EP_IntValue) && p->u.zToken ){
    nByte += sqlite3Strlen30NN(p->u.zToken)+1;
  }
  return ROUND8(nByte);
}