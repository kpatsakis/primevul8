Expr *sqlite3ExprFunction(
  Parse *pParse,        /* Parsing context */
  ExprList *pList,      /* Argument list */
  Token *pToken,        /* Name of the function */
  int eDistinct         /* SF_Distinct or SF_ALL or 0 */
){
  Expr *pNew;
  sqlite3 *db = pParse->db;
  assert( pToken );
  pNew = sqlite3ExprAlloc(db, TK_FUNCTION, pToken, 1);
  if( pNew==0 ){
    sqlite3ExprListDelete(db, pList); /* Avoid memory leak when malloc fails */
    return 0;
  }
  if( pList && pList->nExpr > pParse->db->aLimit[SQLITE_LIMIT_FUNCTION_ARG] ){
    sqlite3ErrorMsg(pParse, "too many arguments on function %T", pToken);
  }
  pNew->x.pList = pList;
  ExprSetProperty(pNew, EP_HasFunc);
  assert( !ExprHasProperty(pNew, EP_xIsSelect) );
  sqlite3ExprSetHeightAndFlags(pParse, pNew);
  if( eDistinct==SF_Distinct ) ExprSetProperty(pNew, EP_Distinct);
  return pNew;
}