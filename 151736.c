void DupPluginOptimizationList(struct _cmsContext_struct* ctx, 
                               const struct _cmsContext_struct* src)
{
   _cmsOptimizationPluginChunkType newHead = { NULL };
   _cmsOptimizationCollection*  entry;
   _cmsOptimizationCollection*  Anterior = NULL;
   _cmsOptimizationPluginChunkType* head = (_cmsOptimizationPluginChunkType*) src->chunks[OptimizationPlugin];

    _cmsAssert(ctx != NULL);
    _cmsAssert(head != NULL);

    // Walk the list copying all nodes
   for (entry = head->OptimizationCollection;
        entry != NULL;
        entry = entry ->Next) {

            _cmsOptimizationCollection *newEntry = ( _cmsOptimizationCollection *) _cmsSubAllocDup(ctx ->MemPool, entry, sizeof(_cmsOptimizationCollection));
   
            if (newEntry == NULL) 
                return;

            // We want to keep the linked list order, so this is a little bit tricky
            newEntry -> Next = NULL;
            if (Anterior)
                Anterior -> Next = newEntry;
     
            Anterior = newEntry;

            if (newHead.OptimizationCollection == NULL)
                newHead.OptimizationCollection = newEntry;
    }

  ctx ->chunks[OptimizationPlugin] = _cmsSubAllocDup(ctx->MemPool, &newHead, sizeof(_cmsOptimizationPluginChunkType));
}