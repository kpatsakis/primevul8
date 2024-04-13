void sqlite3ReleaseTempReg(Parse *pParse, int iReg){
  if( iReg && pParse->nTempReg<ArraySize(pParse->aTempReg) ){
    pParse->aTempReg[pParse->nTempReg++] = iReg;
  }
}