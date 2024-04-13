void sqlite3ExprIfFalseDup(Parse *pParse, Expr *pExpr, int dest,int jumpIfNull){
  sqlite3 *db = pParse->db;
  Expr *pCopy = sqlite3ExprDup(db, pExpr, 0);
  if( db->mallocFailed==0 ){
    sqlite3ExprIfFalse(pParse, pCopy, dest, jumpIfNull);
  }
  sqlite3ExprDelete(db, pCopy);
}