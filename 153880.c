static int analyzeAggregatesInSelect(Walker *pWalker, Select *pSelect){
  UNUSED_PARAMETER(pSelect);
  pWalker->walkerDepth++;
  return WRC_Continue;
}