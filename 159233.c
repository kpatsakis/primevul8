static int resolveExprStep(Walker *pWalker, Expr *pExpr){
  NameContext *pNC;
  Parse *pParse;

  pNC = pWalker->u.pNC;
  assert( pNC!=0 );
  pParse = pNC->pParse;
  assert( pParse==pWalker->pParse );

#ifndef NDEBUG
  if( pNC->pSrcList && pNC->pSrcList->nAlloc>0 ){
    SrcList *pSrcList = pNC->pSrcList;
    int i;
    for(i=0; i<pNC->pSrcList->nSrc; i++){
      assert( pSrcList->a[i].iCursor>=0 && pSrcList->a[i].iCursor<pParse->nTab);
    }
  }
#endif
  switch( pExpr->op ){

#if defined(SQLITE_ENABLE_UPDATE_DELETE_LIMIT) && !defined(SQLITE_OMIT_SUBQUERY)
    /* The special operator TK_ROW means use the rowid for the first
    ** column in the FROM clause.  This is used by the LIMIT and ORDER BY
    ** clause processing on UPDATE and DELETE statements.
    */
    case TK_ROW: {
      SrcList *pSrcList = pNC->pSrcList;
      struct SrcList_item *pItem;
      assert( pSrcList && pSrcList->nSrc==1 );
      pItem = pSrcList->a;
      assert( HasRowid(pItem->pTab) && pItem->pTab->pSelect==0 );
      pExpr->op = TK_COLUMN;
      pExpr->y.pTab = pItem->pTab;
      pExpr->iTable = pItem->iCursor;
      pExpr->iColumn = -1;
      pExpr->affExpr = SQLITE_AFF_INTEGER;
      break;
    }
#endif /* defined(SQLITE_ENABLE_UPDATE_DELETE_LIMIT)
          && !defined(SQLITE_OMIT_SUBQUERY) */

    /* A column name:                    ID
    ** Or table name and column name:    ID.ID
    ** Or a database, table and column:  ID.ID.ID
    **
    ** The TK_ID and TK_OUT cases are combined so that there will only
    ** be one call to lookupName().  Then the compiler will in-line 
    ** lookupName() for a size reduction and performance increase.
    */
    case TK_ID:
    case TK_DOT: {
      const char *zColumn;
      const char *zTable;
      const char *zDb;
      Expr *pRight;

      if( pExpr->op==TK_ID ){
        zDb = 0;
        zTable = 0;
        zColumn = pExpr->u.zToken;
      }else{
        Expr *pLeft = pExpr->pLeft;
        notValid(pParse, pNC, "the \".\" operator", NC_IdxExpr|NC_GenCol);
        pRight = pExpr->pRight;
        if( pRight->op==TK_ID ){
          zDb = 0;
        }else{
          assert( pRight->op==TK_DOT );
          zDb = pLeft->u.zToken;
          pLeft = pRight->pLeft;
          pRight = pRight->pRight;
        }
        zTable = pLeft->u.zToken;
        zColumn = pRight->u.zToken;
        if( IN_RENAME_OBJECT ){
          sqlite3RenameTokenRemap(pParse, (void*)pExpr, (void*)pRight);
          sqlite3RenameTokenRemap(pParse, (void*)&pExpr->y.pTab, (void*)pLeft);
        }
      }
      return lookupName(pParse, zDb, zTable, zColumn, pNC, pExpr);
    }

    /* Resolve function names
    */
    case TK_FUNCTION: {
      ExprList *pList = pExpr->x.pList;    /* The argument list */
      int n = pList ? pList->nExpr : 0;    /* Number of arguments */
      int no_such_func = 0;       /* True if no such function exists */
      int wrong_num_args = 0;     /* True if wrong number of arguments */
      int is_agg = 0;             /* True if is an aggregate function */
      int nId;                    /* Number of characters in function name */
      const char *zId;            /* The function name. */
      FuncDef *pDef;              /* Information about the function */
      u8 enc = ENC(pParse->db);   /* The database encoding */
      int savedAllowFlags = (pNC->ncFlags & (NC_AllowAgg | NC_AllowWin));
#ifndef SQLITE_OMIT_WINDOWFUNC
      Window *pWin = (IsWindowFunc(pExpr) ? pExpr->y.pWin : 0);
#endif
      assert( !ExprHasProperty(pExpr, EP_xIsSelect) );
      zId = pExpr->u.zToken;
      nId = sqlite3Strlen30(zId);
      pDef = sqlite3FindFunction(pParse->db, zId, n, enc, 0);
      if( pDef==0 ){
        pDef = sqlite3FindFunction(pParse->db, zId, -2, enc, 0);
        if( pDef==0 ){
          no_such_func = 1;
        }else{
          wrong_num_args = 1;
        }
      }else{
        is_agg = pDef->xFinalize!=0;
        if( pDef->funcFlags & SQLITE_FUNC_UNLIKELY ){
          ExprSetProperty(pExpr, EP_Unlikely);
          if( n==2 ){
            pExpr->iTable = exprProbability(pList->a[1].pExpr);
            if( pExpr->iTable<0 ){
              sqlite3ErrorMsg(pParse,
                "second argument to likelihood() must be a "
                "constant between 0.0 and 1.0");
              pNC->nErr++;
            }
          }else{
            /* EVIDENCE-OF: R-61304-29449 The unlikely(X) function is
            ** equivalent to likelihood(X, 0.0625).
            ** EVIDENCE-OF: R-01283-11636 The unlikely(X) function is
            ** short-hand for likelihood(X,0.0625).
            ** EVIDENCE-OF: R-36850-34127 The likely(X) function is short-hand
            ** for likelihood(X,0.9375).
            ** EVIDENCE-OF: R-53436-40973 The likely(X) function is equivalent
            ** to likelihood(X,0.9375). */
            /* TUNING: unlikely() probability is 0.0625.  likely() is 0.9375 */
            pExpr->iTable = pDef->zName[0]=='u' ? 8388608 : 125829120;
          }             
        }
#ifndef SQLITE_OMIT_AUTHORIZATION
        {
          int auth = sqlite3AuthCheck(pParse, SQLITE_FUNCTION, 0,pDef->zName,0);
          if( auth!=SQLITE_OK ){
            if( auth==SQLITE_DENY ){
              sqlite3ErrorMsg(pParse, "not authorized to use function: %s",
                                      pDef->zName);
              pNC->nErr++;
            }
            pExpr->op = TK_NULL;
            return WRC_Prune;
          }
        }
#endif
        if( pDef->funcFlags & (SQLITE_FUNC_CONSTANT|SQLITE_FUNC_SLOCHNG) ){
          /* For the purposes of the EP_ConstFunc flag, date and time
          ** functions and other functions that change slowly are considered
          ** constant because they are constant for the duration of one query.
          ** This allows them to be factored out of inner loops. */
          ExprSetProperty(pExpr,EP_ConstFunc);
        }
        if( (pDef->funcFlags & SQLITE_FUNC_CONSTANT)==0 ){
          /* Date/time functions that use 'now', and other functions like
          ** sqlite_version() that might change over time cannot be used
          ** in an index. */
          notValid(pParse, pNC, "non-deterministic functions", NC_SelfRef);
        }else{
          assert( (NC_SelfRef & 0xff)==NC_SelfRef ); /* Must fit in 8 bits */
          pExpr->op2 = pNC->ncFlags & NC_SelfRef;
        }
        if( (pDef->funcFlags & SQLITE_FUNC_INTERNAL)!=0
         && pParse->nested==0
         && sqlite3Config.bInternalFunctions==0
        ){
          /* Internal-use-only functions are disallowed unless the
          ** SQL is being compiled using sqlite3NestedParse() */
          no_such_func = 1;
          pDef = 0;
        }else
        if( (pDef->funcFlags & SQLITE_FUNC_DIRECT)!=0
         && ExprHasProperty(pExpr, EP_Indirect)
         && !IN_RENAME_OBJECT
        ){
          /* Functions tagged with SQLITE_DIRECTONLY may not be used
          ** inside of triggers and views */
          sqlite3ErrorMsg(pParse, "%s() prohibited in triggers and views",
                          pDef->zName);
        }
      }

      if( 0==IN_RENAME_OBJECT ){
#ifndef SQLITE_OMIT_WINDOWFUNC
        assert( is_agg==0 || (pDef->funcFlags & SQLITE_FUNC_MINMAX)
          || (pDef->xValue==0 && pDef->xInverse==0)
          || (pDef->xValue && pDef->xInverse && pDef->xSFunc && pDef->xFinalize)
        );
        if( pDef && pDef->xValue==0 && pWin ){
          sqlite3ErrorMsg(pParse, 
              "%.*s() may not be used as a window function", nId, zId
          );
          pNC->nErr++;
        }else if( 
              (is_agg && (pNC->ncFlags & NC_AllowAgg)==0)
           || (is_agg && (pDef->funcFlags&SQLITE_FUNC_WINDOW) && !pWin)
           || (is_agg && pWin && (pNC->ncFlags & NC_AllowWin)==0)
        ){
          const char *zType;
          if( (pDef->funcFlags & SQLITE_FUNC_WINDOW) || pWin ){
            zType = "window";
          }else{
            zType = "aggregate";
          }
          sqlite3ErrorMsg(pParse, "misuse of %s function %.*s()",zType,nId,zId);
          pNC->nErr++;
          is_agg = 0;
        }
#else
        if( (is_agg && (pNC->ncFlags & NC_AllowAgg)==0) ){
          sqlite3ErrorMsg(pParse,"misuse of aggregate function %.*s()",nId,zId);
          pNC->nErr++;
          is_agg = 0;
        }
#endif
        else if( no_such_func && pParse->db->init.busy==0
#ifdef SQLITE_ENABLE_UNKNOWN_SQL_FUNCTION
                  && pParse->explain==0
#endif
        ){
          sqlite3ErrorMsg(pParse, "no such function: %.*s", nId, zId);
          pNC->nErr++;
        }else if( wrong_num_args ){
          sqlite3ErrorMsg(pParse,"wrong number of arguments to function %.*s()",
               nId, zId);
          pNC->nErr++;
        }
#ifndef SQLITE_OMIT_WINDOWFUNC
        else if( is_agg==0 && ExprHasProperty(pExpr, EP_WinFunc) ){
          sqlite3ErrorMsg(pParse, 
              "FILTER may not be used with non-aggregate %.*s()", 
              nId, zId
          );
          pNC->nErr++;
        }
#endif
        if( is_agg ){
          /* Window functions may not be arguments of aggregate functions.
          ** Or arguments of other window functions. But aggregate functions
          ** may be arguments for window functions.  */
#ifndef SQLITE_OMIT_WINDOWFUNC
          pNC->ncFlags &= ~(NC_AllowWin | (!pWin ? NC_AllowAgg : 0));
#else
          pNC->ncFlags &= ~NC_AllowAgg;
#endif
        }
      }
#ifndef SQLITE_OMIT_WINDOWFUNC
      else if( ExprHasProperty(pExpr, EP_WinFunc) ){
        is_agg = 1;
      }
#endif
      sqlite3WalkExprList(pWalker, pList);
      if( is_agg ){
#ifndef SQLITE_OMIT_WINDOWFUNC
        if( pWin ){
          Select *pSel = pNC->pWinSelect;
          assert( pWin==pExpr->y.pWin );
          if( IN_RENAME_OBJECT==0 ){
            sqlite3WindowUpdate(pParse, pSel->pWinDefn, pWin, pDef);
          }
          sqlite3WalkExprList(pWalker, pWin->pPartition);
          sqlite3WalkExprList(pWalker, pWin->pOrderBy);
          sqlite3WalkExpr(pWalker, pWin->pFilter);
          sqlite3WindowLink(pSel, pWin);
          pNC->ncFlags |= NC_HasWin;
        }else
#endif /* SQLITE_OMIT_WINDOWFUNC */
        {
          NameContext *pNC2 = pNC;
          pExpr->op = TK_AGG_FUNCTION;
          pExpr->op2 = 0;
#ifndef SQLITE_OMIT_WINDOWFUNC
          if( ExprHasProperty(pExpr, EP_WinFunc) ){
            sqlite3WalkExpr(pWalker, pExpr->y.pWin->pFilter);
          }
#endif
          while( pNC2 && !sqlite3FunctionUsesThisSrc(pExpr, pNC2->pSrcList) ){
            pExpr->op2++;
            pNC2 = pNC2->pNext;
          }
          assert( pDef!=0 || IN_RENAME_OBJECT );
          if( pNC2 && pDef ){
            assert( SQLITE_FUNC_MINMAX==NC_MinMaxAgg );
            testcase( (pDef->funcFlags & SQLITE_FUNC_MINMAX)!=0 );
            pNC2->ncFlags |= NC_HasAgg | (pDef->funcFlags & SQLITE_FUNC_MINMAX);

          }
        }
        pNC->ncFlags |= savedAllowFlags;
      }
      /* FIX ME:  Compute pExpr->affinity based on the expected return
      ** type of the function 
      */
      return WRC_Prune;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_SELECT:
    case TK_EXISTS:  testcase( pExpr->op==TK_EXISTS );
#endif
    case TK_IN: {
      testcase( pExpr->op==TK_IN );
      if( ExprHasProperty(pExpr, EP_xIsSelect) ){
        int nRef = pNC->nRef;
        notValid(pParse, pNC, "subqueries", 
                 NC_IsCheck|NC_PartIdx|NC_IdxExpr|NC_GenCol);
        sqlite3WalkSelect(pWalker, pExpr->x.pSelect);
        assert( pNC->nRef>=nRef );
        if( nRef!=pNC->nRef ){
          ExprSetProperty(pExpr, EP_VarSelect);
          pNC->ncFlags |= NC_VarSelect;
        }
      }
      break;
    }
    case TK_VARIABLE: {
      notValid(pParse, pNC, "parameters",
               NC_IsCheck|NC_PartIdx|NC_IdxExpr|NC_GenCol);
      break;
    }
    case TK_IS:
    case TK_ISNOT: {
      Expr *pRight = sqlite3ExprSkipCollateAndLikely(pExpr->pRight);
      assert( !ExprHasProperty(pExpr, EP_Reduced) );
      /* Handle special cases of "x IS TRUE", "x IS FALSE", "x IS NOT TRUE",
      ** and "x IS NOT FALSE". */
      if( pRight->op==TK_ID ){
        int rc = resolveExprStep(pWalker, pRight);
        if( rc==WRC_Abort ) return WRC_Abort;
        if( pRight->op==TK_TRUEFALSE ){
          pExpr->op2 = pExpr->op;
          pExpr->op = TK_TRUTH;
          return WRC_Continue;
        }
      }
      /* Fall thru */
    }
    case TK_BETWEEN:
    case TK_EQ:
    case TK_NE:
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE: {
      int nLeft, nRight;
      if( pParse->db->mallocFailed ) break;
      assert( pExpr->pLeft!=0 );
      nLeft = sqlite3ExprVectorSize(pExpr->pLeft);
      if( pExpr->op==TK_BETWEEN ){
        nRight = sqlite3ExprVectorSize(pExpr->x.pList->a[0].pExpr);
        if( nRight==nLeft ){
          nRight = sqlite3ExprVectorSize(pExpr->x.pList->a[1].pExpr);
        }
      }else{
        assert( pExpr->pRight!=0 );
        nRight = sqlite3ExprVectorSize(pExpr->pRight);
      }
      if( nLeft!=nRight ){
        testcase( pExpr->op==TK_EQ );
        testcase( pExpr->op==TK_NE );
        testcase( pExpr->op==TK_LT );
        testcase( pExpr->op==TK_LE );
        testcase( pExpr->op==TK_GT );
        testcase( pExpr->op==TK_GE );
        testcase( pExpr->op==TK_IS );
        testcase( pExpr->op==TK_ISNOT );
        testcase( pExpr->op==TK_BETWEEN );
        sqlite3ErrorMsg(pParse, "row value misused");
      }
      break; 
    }
  }
  return (pParse->nErr || pParse->db->mallocFailed) ? WRC_Abort : WRC_Continue;
}