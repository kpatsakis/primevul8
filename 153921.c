static void codeCursorHint(
  struct SrcList_item *pTabItem,  /* FROM clause item */
  WhereInfo *pWInfo,    /* The where clause */
  WhereLevel *pLevel,   /* Which loop to provide hints for */
  WhereTerm *pEndRange  /* Hint this end-of-scan boundary term if not NULL */
){
  Parse *pParse = pWInfo->pParse;
  sqlite3 *db = pParse->db;
  Vdbe *v = pParse->pVdbe;
  Expr *pExpr = 0;
  WhereLoop *pLoop = pLevel->pWLoop;
  int iCur;
  WhereClause *pWC;
  WhereTerm *pTerm;
  int i, j;
  struct CCurHint sHint;
  Walker sWalker;

  if( OptimizationDisabled(db, SQLITE_CursorHints) ) return;
  iCur = pLevel->iTabCur;
  assert( iCur==pWInfo->pTabList->a[pLevel->iFrom].iCursor );
  sHint.iTabCur = iCur;
  sHint.iIdxCur = pLevel->iIdxCur;
  sHint.pIdx = pLoop->u.btree.pIndex;
  memset(&sWalker, 0, sizeof(sWalker));
  sWalker.pParse = pParse;
  sWalker.u.pCCurHint = &sHint;
  pWC = &pWInfo->sWC;
  for(i=0; i<pWC->nTerm; i++){
    pTerm = &pWC->a[i];
    if( pTerm->wtFlags & (TERM_VIRTUAL|TERM_CODED) ) continue;
    if( pTerm->prereqAll & pLevel->notReady ) continue;

    /* Any terms specified as part of the ON(...) clause for any LEFT 
    ** JOIN for which the current table is not the rhs are omitted
    ** from the cursor-hint. 
    **
    ** If this table is the rhs of a LEFT JOIN, "IS" or "IS NULL" terms 
    ** that were specified as part of the WHERE clause must be excluded.
    ** This is to address the following:
    **
    **   SELECT ... t1 LEFT JOIN t2 ON (t1.a=t2.b) WHERE t2.c IS NULL;
    **
    ** Say there is a single row in t2 that matches (t1.a=t2.b), but its
    ** t2.c values is not NULL. If the (t2.c IS NULL) constraint is 
    ** pushed down to the cursor, this row is filtered out, causing
    ** SQLite to synthesize a row of NULL values. Which does match the
    ** WHERE clause, and so the query returns a row. Which is incorrect.
    **
    ** For the same reason, WHERE terms such as:
    **
    **   WHERE 1 = (t2.c IS NULL)
    **
    ** are also excluded. See codeCursorHintIsOrFunction() for details.
    */
    if( pTabItem->fg.jointype & JT_LEFT ){
      Expr *pExpr = pTerm->pExpr;
      if( !ExprHasProperty(pExpr, EP_FromJoin) 
       || pExpr->iRightJoinTable!=pTabItem->iCursor
      ){
        sWalker.eCode = 0;
        sWalker.xExprCallback = codeCursorHintIsOrFunction;
        sqlite3WalkExpr(&sWalker, pTerm->pExpr);
        if( sWalker.eCode ) continue;
      }
    }else{
      if( ExprHasProperty(pTerm->pExpr, EP_FromJoin) ) continue;
    }

    /* All terms in pWLoop->aLTerm[] except pEndRange are used to initialize
    ** the cursor.  These terms are not needed as hints for a pure range
    ** scan (that has no == terms) so omit them. */
    if( pLoop->u.btree.nEq==0 && pTerm!=pEndRange ){
      for(j=0; j<pLoop->nLTerm && pLoop->aLTerm[j]!=pTerm; j++){}
      if( j<pLoop->nLTerm ) continue;
    }

    /* No subqueries or non-deterministic functions allowed */
    if( sqlite3ExprContainsSubquery(pTerm->pExpr) ) continue;

    /* For an index scan, make sure referenced columns are actually in
    ** the index. */
    if( sHint.pIdx!=0 ){
      sWalker.eCode = 0;
      sWalker.xExprCallback = codeCursorHintCheckExpr;
      sqlite3WalkExpr(&sWalker, pTerm->pExpr);
      if( sWalker.eCode ) continue;
    }

    /* If we survive all prior tests, that means this term is worth hinting */
    pExpr = sqlite3ExprAnd(pParse, pExpr, sqlite3ExprDup(db, pTerm->pExpr, 0));
  }
  if( pExpr!=0 ){
    sWalker.xExprCallback = codeCursorHintFixExpr;
    sqlite3WalkExpr(&sWalker, pExpr);
    sqlite3VdbeAddOp4(v, OP_CursorHint, 
                      (sHint.pIdx ? sHint.iIdxCur : sHint.iTabCur), 0, 0,
                      (const char*)pExpr, P4_EXPR);
  }
}