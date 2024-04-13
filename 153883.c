static void whereIndexExprTrans(
  Index *pIdx,      /* The Index */
  int iTabCur,      /* Cursor of the table that is being indexed */
  int iIdxCur,      /* Cursor of the index itself */
  WhereInfo *pWInfo /* Transform expressions in this WHERE clause */
){
  int iIdxCol;               /* Column number of the index */
  ExprList *aColExpr;        /* Expressions that are indexed */
  Table *pTab;
  Walker w;
  IdxExprTrans x;
  aColExpr = pIdx->aColExpr;
  if( aColExpr==0 && !pIdx->bHasVCol ){
    /* The index does not reference any expressions or virtual columns
    ** so no translations are needed. */
    return;
  }
  pTab = pIdx->pTable;
  memset(&w, 0, sizeof(w));
  w.u.pIdxTrans = &x;
  x.iTabCur = iTabCur;
  x.iIdxCur = iIdxCur;
  for(iIdxCol=0; iIdxCol<pIdx->nColumn; iIdxCol++){
    i16 iRef = pIdx->aiColumn[iIdxCol];
    if( iRef==XN_EXPR ){
      assert( aColExpr->a[iIdxCol].pExpr!=0 );
      x.pIdxExpr = aColExpr->a[iIdxCol].pExpr;
      w.xExprCallback = whereIndexExprTransNode;
#ifndef SQLITE_OMIT_GENERATED_COLUMNS
    }else if( iRef>=0 && (pTab->aCol[iRef].colFlags & COLFLAG_VIRTUAL)!=0 ){
      x.iTabCol = iRef;
      w.xExprCallback = whereIndexExprTransColumn;
#endif /* SQLITE_OMIT_GENERATED_COLUMNS */
    }else{
      continue;
    }
    x.iIdxCol = iIdxCol;
    sqlite3WalkExpr(&w, pWInfo->pWhere);
    sqlite3WalkExprList(&w, pWInfo->pOrderBy);
    sqlite3WalkExprList(&w, pWInfo->pResultSet);
  }
}