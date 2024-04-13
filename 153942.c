int sqlite3FindInIndex(
  Parse *pParse,             /* Parsing context */
  Expr *pX,                  /* The IN expression */
  u32 inFlags,               /* IN_INDEX_LOOP, _MEMBERSHIP, and/or _NOOP_OK */
  int *prRhsHasNull,         /* Register holding NULL status.  See notes */
  int *aiMap,                /* Mapping from Index fields to RHS fields */
  int *piTab                 /* OUT: index to use */
){
  Select *p;                            /* SELECT to the right of IN operator */
  int eType = 0;                        /* Type of RHS table. IN_INDEX_* */
  int iTab = pParse->nTab++;            /* Cursor of the RHS table */
  int mustBeUnique;                     /* True if RHS must be unique */
  Vdbe *v = sqlite3GetVdbe(pParse);     /* Virtual machine being coded */

  assert( pX->op==TK_IN );
  mustBeUnique = (inFlags & IN_INDEX_LOOP)!=0;

  /* If the RHS of this IN(...) operator is a SELECT, and if it matters 
  ** whether or not the SELECT result contains NULL values, check whether
  ** or not NULL is actually possible (it may not be, for example, due 
  ** to NOT NULL constraints in the schema). If no NULL values are possible,
  ** set prRhsHasNull to 0 before continuing.  */
  if( prRhsHasNull && (pX->flags & EP_xIsSelect) ){
    int i;
    ExprList *pEList = pX->x.pSelect->pEList;
    for(i=0; i<pEList->nExpr; i++){
      if( sqlite3ExprCanBeNull(pEList->a[i].pExpr) ) break;
    }
    if( i==pEList->nExpr ){
      prRhsHasNull = 0;
    }
  }

  /* Check to see if an existing table or index can be used to
  ** satisfy the query.  This is preferable to generating a new 
  ** ephemeral table.  */
  if( pParse->nErr==0 && (p = isCandidateForInOpt(pX))!=0 ){
    sqlite3 *db = pParse->db;              /* Database connection */
    Table *pTab;                           /* Table <table>. */
    i16 iDb;                               /* Database idx for pTab */
    ExprList *pEList = p->pEList;
    int nExpr = pEList->nExpr;

    assert( p->pEList!=0 );             /* Because of isCandidateForInOpt(p) */
    assert( p->pEList->a[0].pExpr!=0 ); /* Because of isCandidateForInOpt(p) */
    assert( p->pSrc!=0 );               /* Because of isCandidateForInOpt(p) */
    pTab = p->pSrc->a[0].pTab;

    /* Code an OP_Transaction and OP_TableLock for <table>. */
    iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
    sqlite3CodeVerifySchema(pParse, iDb);
    sqlite3TableLock(pParse, iDb, pTab->tnum, 0, pTab->zName);

    assert(v);  /* sqlite3GetVdbe() has always been previously called */
    if( nExpr==1 && pEList->a[0].pExpr->iColumn<0 ){
      /* The "x IN (SELECT rowid FROM table)" case */
      int iAddr = sqlite3VdbeAddOp0(v, OP_Once);
      VdbeCoverage(v);

      sqlite3OpenTable(pParse, iTab, iDb, pTab, OP_OpenRead);
      eType = IN_INDEX_ROWID;
      ExplainQueryPlan((pParse, 0,
            "USING ROWID SEARCH ON TABLE %s FOR IN-OPERATOR",pTab->zName));
      sqlite3VdbeJumpHere(v, iAddr);
    }else{
      Index *pIdx;                         /* Iterator variable */
      int affinity_ok = 1;
      int i;

      /* Check that the affinity that will be used to perform each 
      ** comparison is the same as the affinity of each column in table
      ** on the RHS of the IN operator.  If it not, it is not possible to
      ** use any index of the RHS table.  */
      for(i=0; i<nExpr && affinity_ok; i++){
        Expr *pLhs = sqlite3VectorFieldSubexpr(pX->pLeft, i);
        int iCol = pEList->a[i].pExpr->iColumn;
        char idxaff = sqlite3TableColumnAffinity(pTab,iCol); /* RHS table */
        char cmpaff = sqlite3CompareAffinity(pLhs, idxaff);
        testcase( cmpaff==SQLITE_AFF_BLOB );
        testcase( cmpaff==SQLITE_AFF_TEXT );
        switch( cmpaff ){
          case SQLITE_AFF_BLOB:
            break;
          case SQLITE_AFF_TEXT:
            /* sqlite3CompareAffinity() only returns TEXT if one side or the
            ** other has no affinity and the other side is TEXT.  Hence,
            ** the only way for cmpaff to be TEXT is for idxaff to be TEXT
            ** and for the term on the LHS of the IN to have no affinity. */
            assert( idxaff==SQLITE_AFF_TEXT );
            break;
          default:
            affinity_ok = sqlite3IsNumericAffinity(idxaff);
        }
      }

      if( affinity_ok ){
        /* Search for an existing index that will work for this IN operator */
        for(pIdx=pTab->pIndex; pIdx && eType==0; pIdx=pIdx->pNext){
          Bitmask colUsed;      /* Columns of the index used */
          Bitmask mCol;         /* Mask for the current column */
          if( pIdx->nColumn<nExpr ) continue;
          if( pIdx->pPartIdxWhere!=0 ) continue;
          /* Maximum nColumn is BMS-2, not BMS-1, so that we can compute
          ** BITMASK(nExpr) without overflowing */
          testcase( pIdx->nColumn==BMS-2 );
          testcase( pIdx->nColumn==BMS-1 );
          if( pIdx->nColumn>=BMS-1 ) continue;
          if( mustBeUnique ){
            if( pIdx->nKeyCol>nExpr
             ||(pIdx->nColumn>nExpr && !IsUniqueIndex(pIdx))
            ){
              continue;  /* This index is not unique over the IN RHS columns */
            }
          }
  
          colUsed = 0;   /* Columns of index used so far */
          for(i=0; i<nExpr; i++){
            Expr *pLhs = sqlite3VectorFieldSubexpr(pX->pLeft, i);
            Expr *pRhs = pEList->a[i].pExpr;
            CollSeq *pReq = sqlite3BinaryCompareCollSeq(pParse, pLhs, pRhs);
            int j;
  
            assert( pReq!=0 || pRhs->iColumn==XN_ROWID || pParse->nErr );
            for(j=0; j<nExpr; j++){
              if( pIdx->aiColumn[j]!=pRhs->iColumn ) continue;
              assert( pIdx->azColl[j] );
              if( pReq!=0 && sqlite3StrICmp(pReq->zName, pIdx->azColl[j])!=0 ){
                continue;
              }
              break;
            }
            if( j==nExpr ) break;
            mCol = MASKBIT(j);
            if( mCol & colUsed ) break; /* Each column used only once */
            colUsed |= mCol;
            if( aiMap ) aiMap[i] = j;
          }
  
          assert( i==nExpr || colUsed!=(MASKBIT(nExpr)-1) );
          if( colUsed==(MASKBIT(nExpr)-1) ){
            /* If we reach this point, that means the index pIdx is usable */
            int iAddr = sqlite3VdbeAddOp0(v, OP_Once); VdbeCoverage(v);
            ExplainQueryPlan((pParse, 0,
                              "USING INDEX %s FOR IN-OPERATOR",pIdx->zName));
            sqlite3VdbeAddOp3(v, OP_OpenRead, iTab, pIdx->tnum, iDb);
            sqlite3VdbeSetP4KeyInfo(pParse, pIdx);
            VdbeComment((v, "%s", pIdx->zName));
            assert( IN_INDEX_INDEX_DESC == IN_INDEX_INDEX_ASC+1 );
            eType = IN_INDEX_INDEX_ASC + pIdx->aSortOrder[0];
  
            if( prRhsHasNull ){
#ifdef SQLITE_ENABLE_COLUMN_USED_MASK
              i64 mask = (1<<nExpr)-1;
              sqlite3VdbeAddOp4Dup8(v, OP_ColumnsUsed, 
                  iTab, 0, 0, (u8*)&mask, P4_INT64);
#endif
              *prRhsHasNull = ++pParse->nMem;
              if( nExpr==1 ){
                sqlite3SetHasNullFlag(v, iTab, *prRhsHasNull);
              }
            }
            sqlite3VdbeJumpHere(v, iAddr);
          }
        } /* End loop over indexes */
      } /* End if( affinity_ok ) */
    } /* End if not an rowid index */
  } /* End attempt to optimize using an index */

  /* If no preexisting index is available for the IN clause
  ** and IN_INDEX_NOOP is an allowed reply
  ** and the RHS of the IN operator is a list, not a subquery
  ** and the RHS is not constant or has two or fewer terms,
  ** then it is not worth creating an ephemeral table to evaluate
  ** the IN operator so return IN_INDEX_NOOP.
  */
  if( eType==0
   && (inFlags & IN_INDEX_NOOP_OK)
   && !ExprHasProperty(pX, EP_xIsSelect)
   && (!sqlite3InRhsIsConstant(pX) || pX->x.pList->nExpr<=2)
  ){
    eType = IN_INDEX_NOOP;
  }

  if( eType==0 ){
    /* Could not find an existing table or index to use as the RHS b-tree.
    ** We will have to generate an ephemeral table to do the job.
    */
    u32 savedNQueryLoop = pParse->nQueryLoop;
    int rMayHaveNull = 0;
    eType = IN_INDEX_EPH;
    if( inFlags & IN_INDEX_LOOP ){
      pParse->nQueryLoop = 0;
    }else if( prRhsHasNull ){
      *prRhsHasNull = rMayHaveNull = ++pParse->nMem;
    }
    assert( pX->op==TK_IN );
    sqlite3CodeRhsOfIN(pParse, pX, iTab);
    if( rMayHaveNull ){
      sqlite3SetHasNullFlag(v, iTab, rMayHaveNull);
    }
    pParse->nQueryLoop = savedNQueryLoop;
  }

  if( aiMap && eType!=IN_INDEX_INDEX_ASC && eType!=IN_INDEX_INDEX_DESC ){
    int i, n;
    n = sqlite3ExprVectorSize(pX->pLeft);
    for(i=0; i<n; i++) aiMap[i] = i;
  }
  *piTab = iTab;
  return eType;
}