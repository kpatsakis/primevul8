void sqlite3ClearTempRegCache(Parse *pParse){
  pParse->nTempReg = 0;
  pParse->nRangeReg = 0;
}