static int resolveOrderGroupBy(
  NameContext *pNC,     /* The name context of the SELECT statement */
  Select *pSelect,      /* The SELECT statement holding pOrderBy */
  ExprList *pOrderBy,   /* An ORDER BY or GROUP BY clause to resolve */
  const char *zType     /* Either "ORDER" or "GROUP", as appropriate */
){
  int i, j;                      /* Loop counters */
  int iCol;                      /* Column number */
  struct ExprList_item *pItem;   /* A term of the ORDER BY clause */
  Parse *pParse;                 /* Parsing context */
  int nResult;                   /* Number of terms in the result set */

  if( pOrderBy==0 ) return 0;
  nResult = pSelect->pEList->nExpr;
  pParse = pNC->pParse;
  for(i=0, pItem=pOrderBy->a; i<pOrderBy->nExpr; i++, pItem++){
    Expr *pE = pItem->pExpr;
    Expr *pE2 = sqlite3ExprSkipCollateAndLikely(pE);
    if( zType[0]!='G' ){
      iCol = resolveAsName(pParse, pSelect->pEList, pE2);
      if( iCol>0 ){
        /* If an AS-name match is found, mark this ORDER BY column as being
        ** a copy of the iCol-th result-set column.  The subsequent call to
        ** sqlite3ResolveOrderGroupBy() will convert the expression to a
        ** copy of the iCol-th result-set expression. */
        pItem->u.x.iOrderByCol = (u16)iCol;
        continue;
      }
    }
    if( sqlite3ExprIsInteger(pE2, &iCol) ){
      /* The ORDER BY term is an integer constant.  Again, set the column
      ** number so that sqlite3ResolveOrderGroupBy() will convert the
      ** order-by term to a copy of the result-set expression */
      if( iCol<1 || iCol>0xffff ){
        resolveOutOfRangeError(pParse, zType, i+1, nResult);
        return 1;
      }
      pItem->u.x.iOrderByCol = (u16)iCol;
      continue;
    }

    /* Otherwise, treat the ORDER BY term as an ordinary expression */
    pItem->u.x.iOrderByCol = 0;
    if( sqlite3ResolveExprNames(pNC, pE) ){
      return 1;
    }
    for(j=0; j<pSelect->pEList->nExpr; j++){
      if( sqlite3ExprCompare(0, pE, pSelect->pEList->a[j].pExpr, -1)==0 ){
        /* Since this expresion is being changed into a reference
        ** to an identical expression in the result set, remove all Window
        ** objects belonging to the expression from the Select.pWin list. */
        windowRemoveExprFromSelect(pSelect, pE);
        pItem->u.x.iOrderByCol = j+1;
      }
    }
  }
  return sqlite3ResolveOrderGroupBy(pParse, pSelect, pOrderBy, zType);
}