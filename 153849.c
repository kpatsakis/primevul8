static Expr *removeUnindexableInClauseTerms(
  Parse *pParse,        /* The parsing context */
  int iEq,              /* Look at loop terms starting here */
  WhereLoop *pLoop,     /* The current loop */
  Expr *pX              /* The IN expression to be reduced */
){
  sqlite3 *db = pParse->db;
  Expr *pNew = sqlite3ExprDup(db, pX, 0);
  if( db->mallocFailed==0 ){
    ExprList *pOrigRhs = pNew->x.pSelect->pEList;  /* Original unmodified RHS */
    ExprList *pOrigLhs = pNew->pLeft->x.pList;     /* Original unmodified LHS */
    ExprList *pRhs = 0;         /* New RHS after modifications */
    ExprList *pLhs = 0;         /* New LHS after mods */
    int i;                      /* Loop counter */
    Select *pSelect;            /* Pointer to the SELECT on the RHS */

    for(i=iEq; i<pLoop->nLTerm; i++){
      if( pLoop->aLTerm[i]->pExpr==pX ){
        int iField = pLoop->aLTerm[i]->iField - 1;
        if( pOrigRhs->a[iField].pExpr==0 ) continue; /* Duplicate PK column */
        pRhs = sqlite3ExprListAppend(pParse, pRhs, pOrigRhs->a[iField].pExpr);
        pOrigRhs->a[iField].pExpr = 0;
        assert( pOrigLhs->a[iField].pExpr!=0 );
        pLhs = sqlite3ExprListAppend(pParse, pLhs, pOrigLhs->a[iField].pExpr);
        pOrigLhs->a[iField].pExpr = 0;
      }
    }
    sqlite3ExprListDelete(db, pOrigRhs);
    sqlite3ExprListDelete(db, pOrigLhs);
    pNew->pLeft->x.pList = pLhs;
    pNew->x.pSelect->pEList = pRhs;
    if( pLhs && pLhs->nExpr==1 ){
      /* Take care here not to generate a TK_VECTOR containing only a
      ** single value. Since the parser never creates such a vector, some
      ** of the subroutines do not handle this case.  */
      Expr *p = pLhs->a[0].pExpr;
      pLhs->a[0].pExpr = 0;
      sqlite3ExprDelete(db, pNew->pLeft);
      pNew->pLeft = p;
    }
    pSelect = pNew->x.pSelect;
    if( pSelect->pOrderBy ){
      /* If the SELECT statement has an ORDER BY clause, zero the 
      ** iOrderByCol variables. These are set to non-zero when an 
      ** ORDER BY term exactly matches one of the terms of the 
      ** result-set. Since the result-set of the SELECT statement may
      ** have been modified or reordered, these variables are no longer 
      ** set correctly.  Since setting them is just an optimization, 
      ** it's easiest just to zero them here.  */
      ExprList *pOrderBy = pSelect->pOrderBy;
      for(i=0; i<pOrderBy->nExpr; i++){
        pOrderBy->a[i].u.x.iOrderByCol = 0;
      }
    }

#if 0
    printf("For indexing, change the IN expr:\n");
    sqlite3TreeViewExpr(0, pX, 0);
    printf("Into:\n");
    sqlite3TreeViewExpr(0, pNew, 0);
#endif
  }
  return pNew;
}