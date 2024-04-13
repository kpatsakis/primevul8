void  _cmsAllocOptimizationPluginChunk(struct _cmsContext_struct* ctx, 
                                         const struct _cmsContext_struct* src)
{
  if (src != NULL) {

        // Copy all linked list
       DupPluginOptimizationList(ctx, src);
    }
    else {
        static _cmsOptimizationPluginChunkType OptimizationPluginChunkType = { NULL };
        ctx ->chunks[OptimizationPlugin] = _cmsSubAllocDup(ctx ->MemPool, &OptimizationPluginChunkType, sizeof(_cmsOptimizationPluginChunkType));
    }
}