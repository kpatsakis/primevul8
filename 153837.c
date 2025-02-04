static int codeAllEqualityTerms(
  Parse *pParse,        /* Parsing context */
  WhereLevel *pLevel,   /* Which nested loop of the FROM we are coding */
  int bRev,             /* Reverse the order of IN operators */
  int nExtraReg,        /* Number of extra registers to allocate */
  char **pzAff          /* OUT: Set to point to affinity string */
){
  u16 nEq;                      /* The number of == or IN constraints to code */
  u16 nSkip;                    /* Number of left-most columns to skip */
  Vdbe *v = pParse->pVdbe;      /* The vm under construction */
  Index *pIdx;                  /* The index being used for this loop */
  WhereTerm *pTerm;             /* A single constraint term */
  WhereLoop *pLoop;             /* The WhereLoop object */
  int j;                        /* Loop counter */
  int regBase;                  /* Base register */
  int nReg;                     /* Number of registers to allocate */
  char *zAff;                   /* Affinity string to return */

  /* This module is only called on query plans that use an index. */
  pLoop = pLevel->pWLoop;
  assert( (pLoop->wsFlags & WHERE_VIRTUALTABLE)==0 );
  nEq = pLoop->u.btree.nEq;
  nSkip = pLoop->nSkip;
  pIdx = pLoop->u.btree.pIndex;
  assert( pIdx!=0 );

  /* Figure out how many memory cells we will need then allocate them.
  */
  regBase = pParse->nMem + 1;
  nReg = pLoop->u.btree.nEq + nExtraReg;
  pParse->nMem += nReg;

  zAff = sqlite3DbStrDup(pParse->db,sqlite3IndexAffinityStr(pParse->db,pIdx));
  assert( zAff!=0 || pParse->db->mallocFailed );

  if( nSkip ){
    int iIdxCur = pLevel->iIdxCur;
    sqlite3VdbeAddOp1(v, (bRev?OP_Last:OP_Rewind), iIdxCur);
    VdbeCoverageIf(v, bRev==0);
    VdbeCoverageIf(v, bRev!=0);
    VdbeComment((v, "begin skip-scan on %s", pIdx->zName));
    j = sqlite3VdbeAddOp0(v, OP_Goto);
    pLevel->addrSkip = sqlite3VdbeAddOp4Int(v, (bRev?OP_SeekLT:OP_SeekGT),
                            iIdxCur, 0, regBase, nSkip);
    VdbeCoverageIf(v, bRev==0);
    VdbeCoverageIf(v, bRev!=0);
    sqlite3VdbeJumpHere(v, j);
    for(j=0; j<nSkip; j++){
      sqlite3VdbeAddOp3(v, OP_Column, iIdxCur, j, regBase+j);
      testcase( pIdx->aiColumn[j]==XN_EXPR );
      VdbeComment((v, "%s", explainIndexColumnName(pIdx, j)));
    }
  }    

  /* Evaluate the equality constraints
  */
  assert( zAff==0 || (int)strlen(zAff)>=nEq );
  for(j=nSkip; j<nEq; j++){
    int r1;
    pTerm = pLoop->aLTerm[j];
    assert( pTerm!=0 );
    /* The following testcase is true for indices with redundant columns. 
    ** Ex: CREATE INDEX i1 ON t1(a,b,a); SELECT * FROM t1 WHERE a=0 AND b=0; */
    testcase( (pTerm->wtFlags & TERM_CODED)!=0 );
    testcase( pTerm->wtFlags & TERM_VIRTUAL );
    r1 = codeEqualityTerm(pParse, pTerm, pLevel, j, bRev, regBase+j);
    if( r1!=regBase+j ){
      if( nReg==1 ){
        sqlite3ReleaseTempReg(pParse, regBase);
        regBase = r1;
      }else{
        sqlite3VdbeAddOp2(v, OP_SCopy, r1, regBase+j);
      }
    }
    if( pTerm->eOperator & WO_IN ){
      if( pTerm->pExpr->flags & EP_xIsSelect ){
        /* No affinity ever needs to be (or should be) applied to a value
        ** from the RHS of an "? IN (SELECT ...)" expression. The 
        ** sqlite3FindInIndex() routine has already ensured that the 
        ** affinity of the comparison has been applied to the value.  */
        if( zAff ) zAff[j] = SQLITE_AFF_BLOB;
      }
    }else if( (pTerm->eOperator & WO_ISNULL)==0 ){
      Expr *pRight = pTerm->pExpr->pRight;
      if( (pTerm->wtFlags & TERM_IS)==0 && sqlite3ExprCanBeNull(pRight) ){
        sqlite3VdbeAddOp2(v, OP_IsNull, regBase+j, pLevel->addrBrk);
        VdbeCoverage(v);
      }
      if( zAff ){
        if( sqlite3CompareAffinity(pRight, zAff[j])==SQLITE_AFF_BLOB ){
          zAff[j] = SQLITE_AFF_BLOB;
        }
        if( sqlite3ExprNeedsNoAffinityChange(pRight, zAff[j]) ){
          zAff[j] = SQLITE_AFF_BLOB;
        }
      }
    }
  }
  *pzAff = zAff;
  return regBase;
}