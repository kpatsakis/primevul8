MagickExport ChannelPerceptualHash *GetImageChannelPerceptualHash(
  const Image *image,ExceptionInfo *exception)
{
  ChannelMoments
    *moments;

  ChannelPerceptualHash
    *perceptual_hash;

  Image
    *hash_image;

  MagickBooleanType
    status;

  register ssize_t
    i;

  ssize_t
    channel;

  /*
    Blur then transform to sRGB colorspace.
  */
  hash_image=BlurImage(image,0.0,1.0,exception);
  if (hash_image == (Image *) NULL)
    return((ChannelPerceptualHash *) NULL);
  hash_image->depth=8;
  status=TransformImageColorspace(hash_image,sRGBColorspace);
  if (status == MagickFalse)
    return((ChannelPerceptualHash *) NULL);
  moments=GetImageChannelMoments(hash_image,exception);
  hash_image=DestroyImage(hash_image);
  if (moments == (ChannelMoments *) NULL)
    return((ChannelPerceptualHash *) NULL);
  perceptual_hash=(ChannelPerceptualHash *) AcquireQuantumMemory(
    CompositeChannels+1UL,sizeof(*perceptual_hash));
  if (perceptual_hash == (ChannelPerceptualHash *) NULL)
    return((ChannelPerceptualHash *) NULL);
  for (channel=0; channel <= CompositeChannels; channel++)
    for (i=0; i < MaximumNumberOfImageMoments; i++)
      perceptual_hash[channel].P[i]=(-MagickLog10(moments[channel].I[i]));
  moments=(ChannelMoments *) RelinquishMagickMemory(moments);
  /*
    Blur then transform to HCLp colorspace.
  */
  hash_image=BlurImage(image,0.0,1.0,exception);
  if (hash_image == (Image *) NULL)
    {
      perceptual_hash=(ChannelPerceptualHash *) RelinquishMagickMemory(
        perceptual_hash);
      return((ChannelPerceptualHash *) NULL);
    }
  hash_image->depth=8;
  status=TransformImageColorspace(hash_image,HCLpColorspace);
  if (status == MagickFalse)
    {
      perceptual_hash=(ChannelPerceptualHash *) RelinquishMagickMemory(
        perceptual_hash);
      return((ChannelPerceptualHash *) NULL);
    }
  moments=GetImageChannelMoments(hash_image,exception);
  hash_image=DestroyImage(hash_image);
  if (moments == (ChannelMoments *) NULL)
    {
      perceptual_hash=(ChannelPerceptualHash *) RelinquishMagickMemory(
        perceptual_hash);
      return((ChannelPerceptualHash *) NULL);
    }
  for (channel=0; channel <= CompositeChannels; channel++)
    for (i=0; i < MaximumNumberOfImageMoments; i++)
      perceptual_hash[channel].Q[i]=(-MagickLog10(moments[channel].I[i]));
  moments=(ChannelMoments *) RelinquishMagickMemory(moments);
  return(perceptual_hash);
}