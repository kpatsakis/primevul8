void sqlite3ExprCodeCopy(Parse *pParse, Expr *pExpr, int target){
  sqlite3 *db = pParse->db;
  pExpr = sqlite3ExprDup(db, pExpr, 0);
  if( !db->mallocFailed ) sqlite3ExprCode(pParse, pExpr, target);
  sqlite3ExprDelete(db, pExpr);
}