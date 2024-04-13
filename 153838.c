static int exprCompareVariable(Parse *pParse, Expr *pVar, Expr *pExpr){
  int res = 0;
  int iVar;
  sqlite3_value *pL, *pR = 0;
  
  sqlite3ValueFromExpr(pParse->db, pExpr, SQLITE_UTF8, SQLITE_AFF_BLOB, &pR);
  if( pR ){
    iVar = pVar->iColumn;
    sqlite3VdbeSetVarmask(pParse->pVdbe, iVar);
    pL = sqlite3VdbeGetBoundValue(pParse->pReprepare, iVar, SQLITE_AFF_BLOB);
    if( pL ){
      if( sqlite3_value_type(pL)==SQLITE_TEXT ){
        sqlite3_value_text(pL); /* Make sure the encoding is UTF-8 */
      }
      res =  0==sqlite3MemCompare(pL, pR, 0);
    }
    sqlite3ValueFree(pR);
    sqlite3ValueFree(pL);
  }

  return res;
}