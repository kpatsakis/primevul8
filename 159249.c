int sqlite3ResolveExprListNames( 
  NameContext *pNC,       /* Namespace to resolve expressions in. */
  ExprList *pList         /* The expression list to be analyzed. */
){
  int i;
  if( pList ){
    for(i=0; i<pList->nExpr; i++){
      if( sqlite3ResolveExprNames(pNC, pList->a[i].pExpr) ) return WRC_Abort;
    }
  }
  return WRC_Continue;
}