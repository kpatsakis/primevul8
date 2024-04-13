int sqlite3ExprNeedsNoAffinityChange(const Expr *p, char aff){
  u8 op;
  int unaryMinus = 0;
  if( aff==SQLITE_AFF_BLOB ) return 1;
  while( p->op==TK_UPLUS || p->op==TK_UMINUS ){
    if( p->op==TK_UMINUS ) unaryMinus = 1;
    p = p->pLeft;
  }
  op = p->op;
  if( op==TK_REGISTER ) op = p->op2;
  switch( op ){
    case TK_INTEGER: {
      return aff>=SQLITE_AFF_NUMERIC;
    }
    case TK_FLOAT: {
      return aff>=SQLITE_AFF_NUMERIC;
    }
    case TK_STRING: {
      return !unaryMinus && aff==SQLITE_AFF_TEXT;
    }
    case TK_BLOB: {
      return !unaryMinus;
    }
    case TK_COLUMN: {
      assert( p->iTable>=0 );  /* p cannot be part of a CHECK constraint */
      return aff>=SQLITE_AFF_NUMERIC && p->iColumn<0;
    }
    default: {
      return 0;
    }
  }
}