int sqlite3ExprCodeExprList(
  Parse *pParse,     /* Parsing context */
  ExprList *pList,   /* The expression list to be coded */
  int target,        /* Where to write results */
  int srcReg,        /* Source registers if SQLITE_ECEL_REF */
  u8 flags           /* SQLITE_ECEL_* flags */
){
  struct ExprList_item *pItem;
  int i, j, n;
  u8 copyOp = (flags & SQLITE_ECEL_DUP) ? OP_Copy : OP_SCopy;
  Vdbe *v = pParse->pVdbe;
  assert( pList!=0 );
  assert( target>0 );
  assert( pParse->pVdbe!=0 );  /* Never gets this far otherwise */
  n = pList->nExpr;
  if( !ConstFactorOk(pParse) ) flags &= ~SQLITE_ECEL_FACTOR;
  for(pItem=pList->a, i=0; i<n; i++, pItem++){
    Expr *pExpr = pItem->pExpr;
#ifdef SQLITE_ENABLE_SORTER_REFERENCES
    if( pItem->bSorterRef ){
      i--;
      n--;
    }else
#endif
    if( (flags & SQLITE_ECEL_REF)!=0 && (j = pItem->u.x.iOrderByCol)>0 ){
      if( flags & SQLITE_ECEL_OMITREF ){
        i--;
        n--;
      }else{
        sqlite3VdbeAddOp2(v, copyOp, j+srcReg-1, target+i);
      }
    }else if( (flags & SQLITE_ECEL_FACTOR)!=0
           && sqlite3ExprIsConstantNotJoin(pExpr)
    ){
      sqlite3ExprCodeAtInit(pParse, pExpr, target+i);
    }else{
      int inReg = sqlite3ExprCodeTarget(pParse, pExpr, target+i);
      if( inReg!=target+i ){
        VdbeOp *pOp;
        if( copyOp==OP_Copy
         && (pOp=sqlite3VdbeGetOp(v, -1))->opcode==OP_Copy
         && pOp->p1+pOp->p3+1==inReg
         && pOp->p2+pOp->p3+1==target+i
        ){
          pOp->p3++;
        }else{
          sqlite3VdbeAddOp2(v, copyOp, inReg, target+i);
        }
      }
    }
  }
  return n;
}