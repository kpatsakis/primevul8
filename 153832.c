int sqlite3ExprCompare(Parse *pParse, Expr *pA, Expr *pB, int iTab){
  u32 combinedFlags;
  if( pA==0 || pB==0 ){
    return pB==pA ? 0 : 2;
  }
  if( pParse && pA->op==TK_VARIABLE && exprCompareVariable(pParse, pA, pB) ){
    return 0;
  }
  combinedFlags = pA->flags | pB->flags;
  if( combinedFlags & EP_IntValue ){
    if( (pA->flags&pB->flags&EP_IntValue)!=0 && pA->u.iValue==pB->u.iValue ){
      return 0;
    }
    return 2;
  }
  if( pA->op!=pB->op || pA->op==TK_RAISE ){
    if( pA->op==TK_COLLATE && sqlite3ExprCompare(pParse, pA->pLeft,pB,iTab)<2 ){
      return 1;
    }
    if( pB->op==TK_COLLATE && sqlite3ExprCompare(pParse, pA,pB->pLeft,iTab)<2 ){
      return 1;
    }
    return 2;
  }
  if( pA->op!=TK_COLUMN && pA->op!=TK_AGG_COLUMN && pA->u.zToken ){
    if( pA->op==TK_FUNCTION || pA->op==TK_AGG_FUNCTION ){
      if( sqlite3StrICmp(pA->u.zToken,pB->u.zToken)!=0 ) return 2;
#ifndef SQLITE_OMIT_WINDOWFUNC
      assert( pA->op==pB->op );
      if( ExprHasProperty(pA,EP_WinFunc)!=ExprHasProperty(pB,EP_WinFunc) ){
        return 2;
      }
      if( ExprHasProperty(pA,EP_WinFunc) ){
        if( sqlite3WindowCompare(pParse, pA->y.pWin, pB->y.pWin, 1)!=0 ){
          return 2;
        }
      }
#endif
    }else if( pA->op==TK_NULL ){
      return 0;
    }else if( pA->op==TK_COLLATE ){
      if( sqlite3_stricmp(pA->u.zToken,pB->u.zToken)!=0 ) return 2;
    }else if( ALWAYS(pB->u.zToken!=0) && strcmp(pA->u.zToken,pB->u.zToken)!=0 ){
      return 2;
    }
  }
  if( (pA->flags & (EP_Distinct|EP_Commuted))
     != (pB->flags & (EP_Distinct|EP_Commuted)) ) return 2;
  if( (combinedFlags & EP_TokenOnly)==0 ){
    if( combinedFlags & EP_xIsSelect ) return 2;
    if( (combinedFlags & EP_FixedCol)==0
     && sqlite3ExprCompare(pParse, pA->pLeft, pB->pLeft, iTab) ) return 2;
    if( sqlite3ExprCompare(pParse, pA->pRight, pB->pRight, iTab) ) return 2;
    if( sqlite3ExprListCompare(pA->x.pList, pB->x.pList, iTab) ) return 2;
    if( pA->op!=TK_STRING
     && pA->op!=TK_TRUEFALSE
     && (combinedFlags & EP_Reduced)==0
    ){
      if( pA->iColumn!=pB->iColumn ) return 2;
      if( pA->op2!=pB->op2 ){
        if( pA->op==TK_TRUTH ) return 2;
        if( pA->op==TK_FUNCTION && iTab<0 ){
          /* Ex: CREATE TABLE t1(a CHECK( a<julianday('now') ));
          **     INSERT INTO t1(a) VALUES(julianday('now')+10);
          ** Without this test, sqlite3ExprCodeAtInit() will run on the
          ** the julianday() of INSERT first, and remember that expression.
          ** Then sqlite3ExprCodeInit() will see the julianday() in the CHECK
          ** constraint as redundant, reusing the one from the INSERT, even
          ** though the julianday() in INSERT lacks the critical NC_IsCheck
          ** flag.  See ticket [830277d9db6c3ba1] (2019-10-30)
          */
          return 2;
        }
      }
      if( pA->op!=TK_IN && pA->iTable!=pB->iTable && pA->iTable!=iTab ){
        return 2;
      }
    }
  }
  return 0;
}