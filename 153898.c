static void disableTerm(WhereLevel *pLevel, WhereTerm *pTerm){
  int nLoop = 0;
  assert( pTerm!=0 );
  while( (pTerm->wtFlags & TERM_CODED)==0
      && (pLevel->iLeftJoin==0 || ExprHasProperty(pTerm->pExpr, EP_FromJoin))
      && (pLevel->notReady & pTerm->prereqAll)==0
  ){
    if( nLoop && (pTerm->wtFlags & TERM_LIKE)!=0 ){
      pTerm->wtFlags |= TERM_LIKECOND;
    }else{
      pTerm->wtFlags |= TERM_CODED;
    }
    if( pTerm->iParent<0 ) break;
    pTerm = &pTerm->pWC->a[pTerm->iParent];
    assert( pTerm!=0 );
    pTerm->nChild--;
    if( pTerm->nChild!=0 ) break;
    nLoop++;
  }
}