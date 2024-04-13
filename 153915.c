static int dupedExprStructSize(Expr *p, int flags){
  int nSize;
  assert( flags==EXPRDUP_REDUCE || flags==0 ); /* Only one flag value allowed */
  assert( EXPR_FULLSIZE<=0xfff );
  assert( (0xfff & (EP_Reduced|EP_TokenOnly))==0 );
  if( 0==flags || p->op==TK_SELECT_COLUMN 
#ifndef SQLITE_OMIT_WINDOWFUNC
   || ExprHasProperty(p, EP_WinFunc)
#endif
  ){
    nSize = EXPR_FULLSIZE;
  }else{
    assert( !ExprHasProperty(p, EP_TokenOnly|EP_Reduced) );
    assert( !ExprHasProperty(p, EP_FromJoin) ); 
    assert( !ExprHasProperty(p, EP_MemToken) );
    assert( !ExprHasProperty(p, EP_NoReduce) );
    if( p->pLeft || p->x.pList ){
      nSize = EXPR_REDUCEDSIZE | EP_Reduced;
    }else{
      assert( p->pRight==0 );
      nSize = EXPR_TOKENONLYSIZE | EP_TokenOnly;
    }
  }
  return nSize;
}