static int impliesNotNullRow(Walker *pWalker, Expr *pExpr){
  testcase( pExpr->op==TK_AGG_COLUMN );
  testcase( pExpr->op==TK_AGG_FUNCTION );
  if( ExprHasProperty(pExpr, EP_FromJoin) ) return WRC_Prune;
  switch( pExpr->op ){
    case TK_ISNOT:
    case TK_ISNULL:
    case TK_NOTNULL:
    case TK_IS:
    case TK_OR:
    case TK_VECTOR:
    case TK_CASE:
    case TK_IN:
    case TK_FUNCTION:
    case TK_TRUTH:
      testcase( pExpr->op==TK_ISNOT );
      testcase( pExpr->op==TK_ISNULL );
      testcase( pExpr->op==TK_NOTNULL );
      testcase( pExpr->op==TK_IS );
      testcase( pExpr->op==TK_OR );
      testcase( pExpr->op==TK_VECTOR );
      testcase( pExpr->op==TK_CASE );
      testcase( pExpr->op==TK_IN );
      testcase( pExpr->op==TK_FUNCTION );
      testcase( pExpr->op==TK_TRUTH );
      return WRC_Prune;
    case TK_COLUMN:
      if( pWalker->u.iCur==pExpr->iTable ){
        pWalker->eCode = 1;
        return WRC_Abort;
      }
      return WRC_Prune;

    case TK_AND:
      assert( pWalker->eCode==0 );
      sqlite3WalkExpr(pWalker, pExpr->pLeft);
      if( pWalker->eCode ){
        pWalker->eCode = 0;
        sqlite3WalkExpr(pWalker, pExpr->pRight);
      }
      return WRC_Prune;

    case TK_BETWEEN:
      sqlite3WalkExpr(pWalker, pExpr->pLeft);
      return WRC_Prune;

    /* Virtual tables are allowed to use constraints like x=NULL.  So
    ** a term of the form x=y does not prove that y is not null if x
    ** is the column of a virtual table */
    case TK_EQ:
    case TK_NE:
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
      testcase( pExpr->op==TK_EQ );
      testcase( pExpr->op==TK_NE );
      testcase( pExpr->op==TK_LT );
      testcase( pExpr->op==TK_LE );
      testcase( pExpr->op==TK_GT );
      testcase( pExpr->op==TK_GE );
      if( (pExpr->pLeft->op==TK_COLUMN && IsVirtual(pExpr->pLeft->y.pTab))
       || (pExpr->pRight->op==TK_COLUMN && IsVirtual(pExpr->pRight->y.pTab))
      ){
       return WRC_Prune;
      }

    default:
      return WRC_Continue;
  }
}