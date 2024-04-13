void sqlite3CodeRhsOfIN(
  Parse *pParse,          /* Parsing context */
  Expr *pExpr,            /* The IN operator */
  int iTab                /* Use this cursor number */
){
  int addrOnce = 0;           /* Address of the OP_Once instruction at top */
  int addr;                   /* Address of OP_OpenEphemeral instruction */
  Expr *pLeft;                /* the LHS of the IN operator */
  KeyInfo *pKeyInfo = 0;      /* Key information */
  int nVal;                   /* Size of vector pLeft */
  Vdbe *v;                    /* The prepared statement under construction */

  v = pParse->pVdbe;
  assert( v!=0 );

  /* The evaluation of the IN must be repeated every time it
  ** is encountered if any of the following is true:
  **
  **    *  The right-hand side is a correlated subquery
  **    *  The right-hand side is an expression list containing variables
  **    *  We are inside a trigger
  **
  ** If all of the above are false, then we can compute the RHS just once
  ** and reuse it many names.
  */
  if( !ExprHasProperty(pExpr, EP_VarSelect) && pParse->iSelfTab==0 ){
    /* Reuse of the RHS is allowed */
    /* If this routine has already been coded, but the previous code
    ** might not have been invoked yet, so invoke it now as a subroutine. 
    */
    if( ExprHasProperty(pExpr, EP_Subrtn) ){
      addrOnce = sqlite3VdbeAddOp0(v, OP_Once); VdbeCoverage(v);
      if( ExprHasProperty(pExpr, EP_xIsSelect) ){
        ExplainQueryPlan((pParse, 0, "REUSE LIST SUBQUERY %d",
              pExpr->x.pSelect->selId));
      }
      sqlite3VdbeAddOp2(v, OP_Gosub, pExpr->y.sub.regReturn,
                        pExpr->y.sub.iAddr);
      sqlite3VdbeAddOp2(v, OP_OpenDup, iTab, pExpr->iTable);
      sqlite3VdbeJumpHere(v, addrOnce);
      return;
    }

    /* Begin coding the subroutine */
    ExprSetProperty(pExpr, EP_Subrtn);
    pExpr->y.sub.regReturn = ++pParse->nMem;
    pExpr->y.sub.iAddr =
      sqlite3VdbeAddOp2(v, OP_Integer, 0, pExpr->y.sub.regReturn) + 1;
    VdbeComment((v, "return address"));

    addrOnce = sqlite3VdbeAddOp0(v, OP_Once); VdbeCoverage(v);
  }

  /* Check to see if this is a vector IN operator */
  pLeft = pExpr->pLeft;
  nVal = sqlite3ExprVectorSize(pLeft);

  /* Construct the ephemeral table that will contain the content of
  ** RHS of the IN operator.
  */
  pExpr->iTable = iTab;
  addr = sqlite3VdbeAddOp2(v, OP_OpenEphemeral, pExpr->iTable, nVal);
#ifdef SQLITE_ENABLE_EXPLAIN_COMMENTS
  if( ExprHasProperty(pExpr, EP_xIsSelect) ){
    VdbeComment((v, "Result of SELECT %u", pExpr->x.pSelect->selId));
  }else{
    VdbeComment((v, "RHS of IN operator"));
  }
#endif
  pKeyInfo = sqlite3KeyInfoAlloc(pParse->db, nVal, 1);

  if( ExprHasProperty(pExpr, EP_xIsSelect) ){
    /* Case 1:     expr IN (SELECT ...)
    **
    ** Generate code to write the results of the select into the temporary
    ** table allocated and opened above.
    */
    Select *pSelect = pExpr->x.pSelect;
    ExprList *pEList = pSelect->pEList;

    ExplainQueryPlan((pParse, 1, "%sLIST SUBQUERY %d",
        addrOnce?"":"CORRELATED ", pSelect->selId
    ));
    /* If the LHS and RHS of the IN operator do not match, that
    ** error will have been caught long before we reach this point. */
    if( ALWAYS(pEList->nExpr==nVal) ){
      SelectDest dest;
      int i;
      sqlite3SelectDestInit(&dest, SRT_Set, iTab);
      dest.zAffSdst = exprINAffinity(pParse, pExpr);
      pSelect->iLimit = 0;
      testcase( pSelect->selFlags & SF_Distinct );
      testcase( pKeyInfo==0 ); /* Caused by OOM in sqlite3KeyInfoAlloc() */
      if( sqlite3Select(pParse, pSelect, &dest) ){
        sqlite3DbFree(pParse->db, dest.zAffSdst);
        sqlite3KeyInfoUnref(pKeyInfo);
        return;
      }
      sqlite3DbFree(pParse->db, dest.zAffSdst);
      assert( pKeyInfo!=0 ); /* OOM will cause exit after sqlite3Select() */
      assert( pEList!=0 );
      assert( pEList->nExpr>0 );
      assert( sqlite3KeyInfoIsWriteable(pKeyInfo) );
      for(i=0; i<nVal; i++){
        Expr *p = sqlite3VectorFieldSubexpr(pLeft, i);
        pKeyInfo->aColl[i] = sqlite3BinaryCompareCollSeq(
            pParse, p, pEList->a[i].pExpr
        );
      }
    }
  }else if( ALWAYS(pExpr->x.pList!=0) ){
    /* Case 2:     expr IN (exprlist)
    **
    ** For each expression, build an index key from the evaluation and
    ** store it in the temporary table. If <expr> is a column, then use
    ** that columns affinity when building index keys. If <expr> is not
    ** a column, use numeric affinity.
    */
    char affinity;            /* Affinity of the LHS of the IN */
    int i;
    ExprList *pList = pExpr->x.pList;
    struct ExprList_item *pItem;
    int r1, r2;
    affinity = sqlite3ExprAffinity(pLeft);
    if( affinity<=SQLITE_AFF_NONE ){
      affinity = SQLITE_AFF_BLOB;
    }
    if( pKeyInfo ){
      assert( sqlite3KeyInfoIsWriteable(pKeyInfo) );
      pKeyInfo->aColl[0] = sqlite3ExprCollSeq(pParse, pExpr->pLeft);
    }

    /* Loop through each expression in <exprlist>. */
    r1 = sqlite3GetTempReg(pParse);
    r2 = sqlite3GetTempReg(pParse);
    for(i=pList->nExpr, pItem=pList->a; i>0; i--, pItem++){
      Expr *pE2 = pItem->pExpr;

      /* If the expression is not constant then we will need to
      ** disable the test that was generated above that makes sure
      ** this code only executes once.  Because for a non-constant
      ** expression we need to rerun this code each time.
      */
      if( addrOnce && !sqlite3ExprIsConstant(pE2) ){
        sqlite3VdbeChangeToNoop(v, addrOnce);
        ExprClearProperty(pExpr, EP_Subrtn);
        addrOnce = 0;
      }

      /* Evaluate the expression and insert it into the temp table */
      sqlite3ExprCode(pParse, pE2, r1);
      sqlite3VdbeAddOp4(v, OP_MakeRecord, r1, 1, r2, &affinity, 1);
      sqlite3VdbeAddOp4Int(v, OP_IdxInsert, iTab, r2, r1, 1);
    }
    sqlite3ReleaseTempReg(pParse, r1);
    sqlite3ReleaseTempReg(pParse, r2);
  }
  if( pKeyInfo ){
    sqlite3VdbeChangeP4(v, addr, (void *)pKeyInfo, P4_KEYINFO);
  }
  if( addrOnce ){
    sqlite3VdbeJumpHere(v, addrOnce);
    /* Subroutine return */
    sqlite3VdbeAddOp1(v, OP_Return, pExpr->y.sub.regReturn);
    sqlite3VdbeChangeP1(v, pExpr->y.sub.iAddr-1, sqlite3VdbeCurrentAddr(v)-1);
    sqlite3ClearTempRegCache(pParse);
  }
}