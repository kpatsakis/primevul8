CollSeq *sqlite3ExprCompareCollSeq(Parse *pParse, Expr *p){
  if( ExprHasProperty(p, EP_Commuted) ){
    return sqlite3BinaryCompareCollSeq(pParse, p->pRight, p->pLeft);
  }else{
    return sqlite3BinaryCompareCollSeq(pParse, p->pLeft, p->pRight);
  }
}