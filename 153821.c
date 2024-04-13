void sqlite3ExprCodeMove(Parse *pParse, int iFrom, int iTo, int nReg){
  assert( iFrom>=iTo+nReg || iFrom+nReg<=iTo );
  sqlite3VdbeAddOp3(pParse->pVdbe, OP_Move, iFrom, iTo, nReg);
}