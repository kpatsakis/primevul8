static RtreeDValue cellArea(Rtree *pRtree, RtreeCell *p){
  RtreeDValue area = (RtreeDValue)1;
  assert( pRtree->nDim>=1 && pRtree->nDim<=5 );
#ifndef SQLITE_RTREE_INT_ONLY
  if( pRtree->eCoordType==RTREE_COORD_REAL32 ){
    switch( pRtree->nDim ){
      case 5:  area  = p->aCoord[9].f - p->aCoord[8].f;
      case 4:  area *= p->aCoord[7].f - p->aCoord[6].f;
      case 3:  area *= p->aCoord[5].f - p->aCoord[4].f;
      case 2:  area *= p->aCoord[3].f - p->aCoord[2].f;
      default: area *= p->aCoord[1].f - p->aCoord[0].f;
    }
  }else
#endif
  {
    switch( pRtree->nDim ){
      case 5:  area  = (i64)p->aCoord[9].i - (i64)p->aCoord[8].i;
      case 4:  area *= (i64)p->aCoord[7].i - (i64)p->aCoord[6].i;
      case 3:  area *= (i64)p->aCoord[5].i - (i64)p->aCoord[4].i;
      case 2:  area *= (i64)p->aCoord[3].i - (i64)p->aCoord[2].i;
      default: area *= (i64)p->aCoord[1].i - (i64)p->aCoord[0].i;
    }
  }
  return area;
}