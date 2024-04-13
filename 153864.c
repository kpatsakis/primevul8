static int gatherSelectWindowsSelectCallback(Walker *pWalker, Select *p){
  return p==pWalker->u.pSelect ? WRC_Continue : WRC_Prune;
}