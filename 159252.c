static int resolveCompoundOrderBy(
  Parse *pParse,        /* Parsing context.  Leave error messages here */
  Select *pSelect       /* The SELECT statement containing the ORDER BY */
){
  int i;
  ExprList *pOrderBy;
  ExprList *pEList;
  sqlite3 *db;
  int moreToDo = 1;

  pOrderBy = pSelect->pOrderBy;
  if( pOrderBy==0 ) return 0;
  db = pParse->db;
  if( pOrderBy->nExpr>db->aLimit[SQLITE_LIMIT_COLUMN] ){
    sqlite3ErrorMsg(pParse, "too many terms in ORDER BY clause");
    return 1;
  }
  for(i=0; i<pOrderBy->nExpr; i++){
    pOrderBy->a[i].done = 0;
  }
  pSelect->pNext = 0;
  while( pSelect->pPrior ){
    pSelect->pPrior->pNext = pSelect;
    pSelect = pSelect->pPrior;
  }
  while( pSelect && moreToDo ){
    struct ExprList_item *pItem;
    moreToDo = 0;
    pEList = pSelect->pEList;
    assert( pEList!=0 );
    for(i=0, pItem=pOrderBy->a; i<pOrderBy->nExpr; i++, pItem++){
      int iCol = -1;
      Expr *pE, *pDup;
      if( pItem->done ) continue;
      pE = sqlite3ExprSkipCollateAndLikely(pItem->pExpr);
      if( sqlite3ExprIsInteger(pE, &iCol) ){
        if( iCol<=0 || iCol>pEList->nExpr ){
          resolveOutOfRangeError(pParse, "ORDER", i+1, pEList->nExpr);
          return 1;
        }
      }else{
        iCol = resolveAsName(pParse, pEList, pE);
        if( iCol==0 ){
          /* Now test if expression pE matches one of the values returned
          ** by pSelect. In the usual case this is done by duplicating the 
          ** expression, resolving any symbols in it, and then comparing
          ** it against each expression returned by the SELECT statement.
          ** Once the comparisons are finished, the duplicate expression
          ** is deleted.
          **
          ** Or, if this is running as part of an ALTER TABLE operation,
          ** resolve the symbols in the actual expression, not a duplicate.
          ** And, if one of the comparisons is successful, leave the expression
          ** as is instead of transforming it to an integer as in the usual
          ** case. This allows the code in alter.c to modify column
          ** refererences within the ORDER BY expression as required.  */
          if( IN_RENAME_OBJECT ){
            pDup = pE;
          }else{
            pDup = sqlite3ExprDup(db, pE, 0);
          }
          if( !db->mallocFailed ){
            assert(pDup);
            iCol = resolveOrderByTermToExprList(pParse, pSelect, pDup);
          }
          if( !IN_RENAME_OBJECT ){
            sqlite3ExprDelete(db, pDup);
          }
        }
      }
      if( iCol>0 ){
        /* Convert the ORDER BY term into an integer column number iCol,
        ** taking care to preserve the COLLATE clause if it exists */
        if( !IN_RENAME_OBJECT ){
          Expr *pNew = sqlite3Expr(db, TK_INTEGER, 0);
          if( pNew==0 ) return 1;
          pNew->flags |= EP_IntValue;
          pNew->u.iValue = iCol;
          if( pItem->pExpr==pE ){
            pItem->pExpr = pNew;
          }else{
            Expr *pParent = pItem->pExpr;
            assert( pParent->op==TK_COLLATE );
            while( pParent->pLeft->op==TK_COLLATE ) pParent = pParent->pLeft;
            assert( pParent->pLeft==pE );
            pParent->pLeft = pNew;
          }
          sqlite3ExprDelete(db, pE);
          pItem->u.x.iOrderByCol = (u16)iCol;
        }
        pItem->done = 1;
      }else{
        moreToDo = 1;
      }
    }
    pSelect = pSelect->pNext;
  }
  for(i=0; i<pOrderBy->nExpr; i++){
    if( pOrderBy->a[i].done==0 ){
      sqlite3ErrorMsg(pParse, "%r ORDER BY term does not match any "
            "column in the result set", i+1);
      return 1;
    }
  }
  return 0;
}