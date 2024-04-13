int sqlite3ExprCanBeNull(const Expr *p){
  u8 op;
  while( p->op==TK_UPLUS || p->op==TK_UMINUS ){
    p = p->pLeft;
  }
  op = p->op;
  if( op==TK_REGISTER ) op = p->op2;
  switch( op ){
    case TK_INTEGER:
    case TK_STRING:
    case TK_FLOAT:
    case TK_BLOB:
      return 0;
    case TK_COLUMN:
      return ExprHasProperty(p, EP_CanBeNull) ||
             p->y.pTab==0 ||  /* Reference to column of index on expression */
             (p->iColumn>=0 && p->y.pTab->aCol[p->iColumn].notNull==0);
    default:
      return 1;
  }
}