static void whereLikeOptimizationStringFixup(
  Vdbe *v,                /* prepared statement under construction */
  WhereLevel *pLevel,     /* The loop that contains the LIKE operator */
  WhereTerm *pTerm        /* The upper or lower bound just coded */
){
  if( pTerm->wtFlags & TERM_LIKEOPT ){
    VdbeOp *pOp;
    assert( pLevel->iLikeRepCntr>0 );
    pOp = sqlite3VdbeGetOp(v, -1);
    assert( pOp!=0 );
    assert( pOp->opcode==OP_String8 
            || pTerm->pWC->pWInfo->pParse->db->mallocFailed );
    pOp->p3 = (int)(pLevel->iLikeRepCntr>>1);  /* Register holding counter */
    pOp->p5 = (u8)(pLevel->iLikeRepCntr&1);    /* ASC or DESC */
  }
}