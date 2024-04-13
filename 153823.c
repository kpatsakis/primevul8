void sqlite3ExprCode(Parse *pParse, Expr *pExpr, int target){
  int inReg;

  assert( target>0 && target<=pParse->nMem );
  inReg = sqlite3ExprCodeTarget(pParse, pExpr, target);
  assert( pParse->pVdbe!=0 || pParse->db->mallocFailed );
  if( inReg!=target && pParse->pVdbe ){
    sqlite3VdbeAddOp2(pParse->pVdbe, OP_SCopy, inReg, target);
  }
}