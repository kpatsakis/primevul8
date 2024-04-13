CollSeq *sqlite3ExprNNCollSeq(Parse *pParse, Expr *pExpr){
  CollSeq *p = sqlite3ExprCollSeq(pParse, pExpr);
  if( p==0 ) p = pParse->db->pDfltColl;
  assert( p!=0 );
  return p;
}