int sqlite3NoTempsInRange(Parse *pParse, int iFirst, int iLast){
  int i;
  if( pParse->nRangeReg>0
   && pParse->iRangeReg+pParse->nRangeReg > iFirst
   && pParse->iRangeReg <= iLast
  ){
     return 0;
  }
  for(i=0; i<pParse->nTempReg; i++){
    if( pParse->aTempReg[i]>=iFirst && pParse->aTempReg[i]<=iLast ){
      return 0;
    }
  }
  return 1;
}