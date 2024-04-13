static int codeEqualityTerm(
  Parse *pParse,      /* The parsing context */
  WhereTerm *pTerm,   /* The term of the WHERE clause to be coded */
  WhereLevel *pLevel, /* The level of the FROM clause we are working on */
  int iEq,            /* Index of the equality term within this level */
  int bRev,           /* True for reverse-order IN operations */
  int iTarget         /* Attempt to leave results in this register */
){
  Expr *pX = pTerm->pExpr;
  Vdbe *v = pParse->pVdbe;
  int iReg;                  /* Register holding results */

  assert( pLevel->pWLoop->aLTerm[iEq]==pTerm );
  assert( iTarget>0 );
  if( pX->op==TK_EQ || pX->op==TK_IS ){
    iReg = sqlite3ExprCodeTarget(pParse, pX->pRight, iTarget);
  }else if( pX->op==TK_ISNULL ){
    iReg = iTarget;
    sqlite3VdbeAddOp2(v, OP_Null, 0, iReg);
#ifndef SQLITE_OMIT_SUBQUERY
  }else{
    int eType = IN_INDEX_NOOP;
    int iTab;
    struct InLoop *pIn;
    WhereLoop *pLoop = pLevel->pWLoop;
    int i;
    int nEq = 0;
    int *aiMap = 0;

    if( (pLoop->wsFlags & WHERE_VIRTUALTABLE)==0
      && pLoop->u.btree.pIndex!=0
      && pLoop->u.btree.pIndex->aSortOrder[iEq]
    ){
      testcase( iEq==0 );
      testcase( bRev );
      bRev = !bRev;
    }
    assert( pX->op==TK_IN );
    iReg = iTarget;

    for(i=0; i<iEq; i++){
      if( pLoop->aLTerm[i] && pLoop->aLTerm[i]->pExpr==pX ){
        disableTerm(pLevel, pTerm);
        return iTarget;
      }
    }
    for(i=iEq;i<pLoop->nLTerm; i++){
      assert( pLoop->aLTerm[i]!=0 );
      if( pLoop->aLTerm[i]->pExpr==pX ) nEq++;
    }

    iTab = 0;
    if( (pX->flags & EP_xIsSelect)==0 || pX->x.pSelect->pEList->nExpr==1 ){
      eType = sqlite3FindInIndex(pParse, pX, IN_INDEX_LOOP, 0, 0, &iTab);
    }else{
      sqlite3 *db = pParse->db;
      pX = removeUnindexableInClauseTerms(pParse, iEq, pLoop, pX);

      if( !db->mallocFailed ){
        aiMap = (int*)sqlite3DbMallocZero(pParse->db, sizeof(int)*nEq);
        eType = sqlite3FindInIndex(pParse, pX, IN_INDEX_LOOP, 0, aiMap, &iTab);
        pTerm->pExpr->iTable = iTab;
      }
      sqlite3ExprDelete(db, pX);
      pX = pTerm->pExpr;
    }

    if( eType==IN_INDEX_INDEX_DESC ){
      testcase( bRev );
      bRev = !bRev;
    }
    sqlite3VdbeAddOp2(v, bRev ? OP_Last : OP_Rewind, iTab, 0);
    VdbeCoverageIf(v, bRev);
    VdbeCoverageIf(v, !bRev);
    assert( (pLoop->wsFlags & WHERE_MULTI_OR)==0 );

    pLoop->wsFlags |= WHERE_IN_ABLE;
    if( pLevel->u.in.nIn==0 ){
      pLevel->addrNxt = sqlite3VdbeMakeLabel(pParse);
    }

    i = pLevel->u.in.nIn;
    pLevel->u.in.nIn += nEq;
    pLevel->u.in.aInLoop =
       sqlite3DbReallocOrFree(pParse->db, pLevel->u.in.aInLoop,
                              sizeof(pLevel->u.in.aInLoop[0])*pLevel->u.in.nIn);
    pIn = pLevel->u.in.aInLoop;
    if( pIn ){
      int iMap = 0;               /* Index in aiMap[] */
      pIn += i;
      for(i=iEq;i<pLoop->nLTerm; i++){
        if( pLoop->aLTerm[i]->pExpr==pX ){
          int iOut = iReg + i - iEq;
          if( eType==IN_INDEX_ROWID ){
            pIn->addrInTop = sqlite3VdbeAddOp2(v, OP_Rowid, iTab, iOut);
          }else{
            int iCol = aiMap ? aiMap[iMap++] : 0;
            pIn->addrInTop = sqlite3VdbeAddOp3(v,OP_Column,iTab, iCol, iOut);
          }
          sqlite3VdbeAddOp1(v, OP_IsNull, iOut); VdbeCoverage(v);
          if( i==iEq ){
            pIn->iCur = iTab;
            pIn->eEndLoopOp = bRev ? OP_Prev : OP_Next;
            if( iEq>0 && (pLoop->wsFlags & WHERE_VIRTUALTABLE)==0 ){
              pIn->iBase = iReg - i;
              pIn->nPrefix = i;
              pLoop->wsFlags |= WHERE_IN_EARLYOUT;
            }else{
              pIn->nPrefix = 0;
            }
          }else{
            pIn->eEndLoopOp = OP_Noop;
          }
          pIn++;
        }
      }
    }else{
      pLevel->u.in.nIn = 0;
    }
    sqlite3DbFree(pParse->db, aiMap);
#endif
  }
  disableTerm(pLevel, pTerm);
  return iReg;
}