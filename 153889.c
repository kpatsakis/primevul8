static int exprSrcCount(Walker *pWalker, Expr *pExpr){
  /* The NEVER() on the second term is because sqlite3FunctionUsesThisSrc()
  ** is always called before sqlite3ExprAnalyzeAggregates() and so the
  ** TK_COLUMNs have not yet been converted into TK_AGG_COLUMN.  If
  ** sqlite3FunctionUsesThisSrc() is used differently in the future, the
  ** NEVER() will need to be removed. */
  if( pExpr->op==TK_COLUMN || NEVER(pExpr->op==TK_AGG_COLUMN) ){
    int i;
    struct SrcCount *p = pWalker->u.pSrcCount;
    SrcList *pSrc = p->pSrc;
    int nSrc = pSrc ? pSrc->nSrc : 0;
    for(i=0; i<nSrc; i++){
      if( pExpr->iTable==pSrc->a[i].iCursor ) break;
    }
    if( i<nSrc ){
      p->nThis++;
    }else if( nSrc==0 || pExpr->iTable<pSrc->a[0].iCursor ){
      /* In a well-formed parse tree (no name resolution errors),
      ** TK_COLUMN nodes with smaller Expr.iTable values are in an
      ** outer context.  Those are the only ones to count as "other" */
      p->nOther++;
    }
  }
  return WRC_Continue;
}