int sqlite3CodeSubselect(Parse *pParse, Expr *pExpr){
  int addrOnce = 0;           /* Address of OP_Once at top of subroutine */
  int rReg = 0;               /* Register storing resulting */
  Select *pSel;               /* SELECT statement to encode */
  SelectDest dest;            /* How to deal with SELECT result */
  int nReg;                   /* Registers to allocate */
  Expr *pLimit;               /* New limit expression */

  Vdbe *v = pParse->pVdbe;
  assert( v!=0 );
  testcase( pExpr->op==TK_EXISTS );
  testcase( pExpr->op==TK_SELECT );
  assert( pExpr->op==TK_EXISTS || pExpr->op==TK_SELECT );
  assert( ExprHasProperty(pExpr, EP_xIsSelect) );
  pSel = pExpr->x.pSelect;

  /* The evaluation of the EXISTS/SELECT must be repeated every time it
  ** is encountered if any of the following is true:
  **
  **    *  The right-hand side is a correlated subquery
  **    *  The right-hand side is an expression list containing variables
  **    *  We are inside a trigger
  **
  ** If all of the above are false, then we can run this code just once
  ** save the results, and reuse the same result on subsequent invocations.
  */
  if( !ExprHasProperty(pExpr, EP_VarSelect) ){
    /* If this routine has already been coded, then invoke it as a
    ** subroutine. */
    if( ExprHasProperty(pExpr, EP_Subrtn) ){
      ExplainQueryPlan((pParse, 0, "REUSE SUBQUERY %d", pSel->selId));
      sqlite3VdbeAddOp2(v, OP_Gosub, pExpr->y.sub.regReturn,
                        pExpr->y.sub.iAddr);
      return pExpr->iTable;
    }

    /* Begin coding the subroutine */
    ExprSetProperty(pExpr, EP_Subrtn);
    pExpr->y.sub.regReturn = ++pParse->nMem;
    pExpr->y.sub.iAddr =
      sqlite3VdbeAddOp2(v, OP_Integer, 0, pExpr->y.sub.regReturn) + 1;
    VdbeComment((v, "return address"));

    addrOnce = sqlite3VdbeAddOp0(v, OP_Once); VdbeCoverage(v);
  }
  
  /* For a SELECT, generate code to put the values for all columns of
  ** the first row into an array of registers and return the index of
  ** the first register.
  **
  ** If this is an EXISTS, write an integer 0 (not exists) or 1 (exists)
  ** into a register and return that register number.
  **
  ** In both cases, the query is augmented with "LIMIT 1".  Any 
  ** preexisting limit is discarded in place of the new LIMIT 1.
  */
  ExplainQueryPlan((pParse, 1, "%sSCALAR SUBQUERY %d",
        addrOnce?"":"CORRELATED ", pSel->selId));
  nReg = pExpr->op==TK_SELECT ? pSel->pEList->nExpr : 1;
  sqlite3SelectDestInit(&dest, 0, pParse->nMem+1);
  pParse->nMem += nReg;
  if( pExpr->op==TK_SELECT ){
    dest.eDest = SRT_Mem;
    dest.iSdst = dest.iSDParm;
    dest.nSdst = nReg;
    sqlite3VdbeAddOp3(v, OP_Null, 0, dest.iSDParm, dest.iSDParm+nReg-1);
    VdbeComment((v, "Init subquery result"));
  }else{
    dest.eDest = SRT_Exists;
    sqlite3VdbeAddOp2(v, OP_Integer, 0, dest.iSDParm);
    VdbeComment((v, "Init EXISTS result"));
  }
  if( pSel->pLimit ){
    /* The subquery already has a limit.  If the pre-existing limit is X
    ** then make the new limit X<>0 so that the new limit is either 1 or 0 */
    sqlite3 *db = pParse->db;
    pLimit = sqlite3Expr(db, TK_INTEGER, "0");
    if( pLimit ){
      pLimit->affExpr = SQLITE_AFF_NUMERIC;
      pLimit = sqlite3PExpr(pParse, TK_NE,
                            sqlite3ExprDup(db, pSel->pLimit->pLeft, 0), pLimit);
    }
    sqlite3ExprDelete(db, pSel->pLimit->pLeft);
    pSel->pLimit->pLeft = pLimit;
  }else{
    /* If there is no pre-existing limit add a limit of 1 */
    pLimit = sqlite3Expr(pParse->db, TK_INTEGER, "1");
    pSel->pLimit = sqlite3PExpr(pParse, TK_LIMIT, pLimit, 0);
  }
  pSel->iLimit = 0;
  if( sqlite3Select(pParse, pSel, &dest) ){
    return 0;
  }
  pExpr->iTable = rReg = dest.iSDParm;
  ExprSetVVAProperty(pExpr, EP_NoReduce);
  if( addrOnce ){
    sqlite3VdbeJumpHere(v, addrOnce);

    /* Subroutine return */
    sqlite3VdbeAddOp1(v, OP_Return, pExpr->y.sub.regReturn);
    sqlite3VdbeChangeP1(v, pExpr->y.sub.iAddr-1, sqlite3VdbeCurrentAddr(v)-1);
    sqlite3ClearTempRegCache(pParse);
  }

  return rReg;
}