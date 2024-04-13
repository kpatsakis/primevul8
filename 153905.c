static int exprImpliesNotNull(
  Parse *pParse,      /* Parsing context */
  Expr *p,            /* The expression to be checked */
  Expr *pNN,          /* The expression that is NOT NULL */
  int iTab,           /* Table being evaluated */
  int seenNot         /* Return true only if p can be any non-NULL value */
){
  assert( p );
  assert( pNN );
  if( sqlite3ExprCompare(pParse, p, pNN, iTab)==0 ){
    return pNN->op!=TK_NULL;
  }
  switch( p->op ){
    case TK_IN: {
      if( seenNot && ExprHasProperty(p, EP_xIsSelect) ) return 0;
      assert( ExprHasProperty(p,EP_xIsSelect)
           || (p->x.pList!=0 && p->x.pList->nExpr>0) );
      return exprImpliesNotNull(pParse, p->pLeft, pNN, iTab, 1);
    }
    case TK_BETWEEN: {
      ExprList *pList = p->x.pList;
      assert( pList!=0 );
      assert( pList->nExpr==2 );
      if( seenNot ) return 0;
      if( exprImpliesNotNull(pParse, pList->a[0].pExpr, pNN, iTab, 1)
       || exprImpliesNotNull(pParse, pList->a[1].pExpr, pNN, iTab, 1)
      ){
        return 1;
      }
      return exprImpliesNotNull(pParse, p->pLeft, pNN, iTab, 1);
    }
    case TK_EQ:
    case TK_NE:
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_PLUS:
    case TK_MINUS:
    case TK_BITOR:
    case TK_LSHIFT:
    case TK_RSHIFT: 
    case TK_CONCAT: 
      seenNot = 1;
      /* Fall thru */
    case TK_STAR:
    case TK_REM:
    case TK_BITAND:
    case TK_SLASH: {
      if( exprImpliesNotNull(pParse, p->pRight, pNN, iTab, seenNot) ) return 1;
      /* Fall thru into the next case */
    }
    case TK_SPAN:
    case TK_COLLATE:
    case TK_UPLUS:
    case TK_UMINUS: {
      return exprImpliesNotNull(pParse, p->pLeft, pNN, iTab, seenNot);
    }
    case TK_TRUTH: {
      if( seenNot ) return 0;
      if( p->op2!=TK_IS ) return 0;
      return exprImpliesNotNull(pParse, p->pLeft, pNN, iTab, 1);
    }
    case TK_BITNOT:
    case TK_NOT: {
      return exprImpliesNotNull(pParse, p->pLeft, pNN, iTab, 1);
    }
  }
  return 0;
}