dynamicPutbuf (struct gdIOCtx *ctx, const void *buf, int size)
{
  dpIOCtx *dctx;
  dctx = (dpIOCtx *) ctx;

  appendDynamic (dctx->dp, buf, size);

  if (dctx->dp->dataGood)
    {
      return size;
    }
  else
    {
      return -1;
    };

}