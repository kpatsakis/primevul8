static void resolveAlias(
  Parse *pParse,         /* Parsing context */
  ExprList *pEList,      /* A result set */
  int iCol,              /* A column in the result set.  0..pEList->nExpr-1 */
  Expr *pExpr,           /* Transform this into an alias to the result set */
  const char *zType,     /* "GROUP" or "ORDER" or "" */
  int nSubquery          /* Number of subqueries that the label is moving */
){
  Expr *pOrig;           /* The iCol-th column of the result set */
  Expr *pDup;            /* Copy of pOrig */
  sqlite3 *db;           /* The database connection */

  assert( iCol>=0 && iCol<pEList->nExpr );
  pOrig = pEList->a[iCol].pExpr;
  assert( pOrig!=0 );
  db = pParse->db;
  pDup = sqlite3ExprDup(db, pOrig, 0);
  if( pDup!=0 ){
    if( zType[0]!='G' ) incrAggFunctionDepth(pDup, nSubquery);
    if( pExpr->op==TK_COLLATE ){
      pDup = sqlite3ExprAddCollateString(pParse, pDup, pExpr->u.zToken);
    }

    /* Before calling sqlite3ExprDelete(), set the EP_Static flag. This 
    ** prevents ExprDelete() from deleting the Expr structure itself,
    ** allowing it to be repopulated by the memcpy() on the following line.
    ** The pExpr->u.zToken might point into memory that will be freed by the
    ** sqlite3DbFree(db, pDup) on the last line of this block, so be sure to
    ** make a copy of the token before doing the sqlite3DbFree().
    */
    ExprSetProperty(pExpr, EP_Static);
    sqlite3ExprDelete(db, pExpr);
    memcpy(pExpr, pDup, sizeof(*pExpr));
    if( !ExprHasProperty(pExpr, EP_IntValue) && pExpr->u.zToken!=0 ){
      assert( (pExpr->flags & (EP_Reduced|EP_TokenOnly))==0 );
      pExpr->u.zToken = sqlite3DbStrDup(db, pExpr->u.zToken);
      pExpr->flags |= EP_MemToken;
    }
    if( ExprHasProperty(pExpr, EP_WinFunc) ){
      if( pExpr->y.pWin!=0 ){
        pExpr->y.pWin->pOwner = pExpr;
      }else{
        assert( db->mallocFailed );
      }
    }
    sqlite3DbFree(db, pDup);
  }
  ExprSetProperty(pExpr, EP_Alias);
}