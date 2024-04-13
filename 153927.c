Expr *sqlite3ExprAnd(Parse *pParse, Expr *pLeft, Expr *pRight){
  sqlite3 *db = pParse->db;
  if( pLeft==0  ){
    return pRight;
  }else if( pRight==0 ){
    return pLeft;
  }else if( ExprAlwaysFalse(pLeft) || ExprAlwaysFalse(pRight) ){
    sqlite3ExprUnmapAndDelete(pParse, pLeft);
    sqlite3ExprUnmapAndDelete(pParse, pRight);
    return sqlite3Expr(db, TK_INTEGER, "0");
  }else{
    return sqlite3PExpr(pParse, TK_AND, pLeft, pRight);
  }
}