void sqlite3ReleaseTempRange(Parse *pParse, int iReg, int nReg){
  if( nReg==1 ){
    sqlite3ReleaseTempReg(pParse, iReg);
    return;
  }
  if( nReg>pParse->nRangeReg ){
    pParse->nRangeReg = nReg;
    pParse->iRangeReg = iReg;
  }
}