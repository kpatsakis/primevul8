int sqlite3ExprIsInteger(Expr *p, int *pValue){
  int rc = 0;
  if( NEVER(p==0) ) return 0;  /* Used to only happen following on OOM */

  /* If an expression is an integer literal that fits in a signed 32-bit
  ** integer, then the EP_IntValue flag will have already been set */
  assert( p->op!=TK_INTEGER || (p->flags & EP_IntValue)!=0
           || sqlite3GetInt32(p->u.zToken, &rc)==0 );

  if( p->flags & EP_IntValue ){
    *pValue = p->u.iValue;
    return 1;
  }
  switch( p->op ){
    case TK_UPLUS: {
      rc = sqlite3ExprIsInteger(p->pLeft, pValue);
      break;
    }
    case TK_UMINUS: {
      int v;
      if( sqlite3ExprIsInteger(p->pLeft, &v) ){
        assert( v!=(-2147483647-1) );
        *pValue = -v;
        rc = 1;
      }
      break;
    }
    default: break;
  }
  return rc;
}