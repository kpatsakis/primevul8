int sqlite3GetTempReg(Parse *pParse){
  if( pParse->nTempReg==0 ){
    return ++pParse->nMem;
  }
  return pParse->aTempReg[--pParse->nTempReg];
}