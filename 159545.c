MagickExport ChannelMoments *GetImageChannelMoments(const Image *image,
  ExceptionInfo *exception)
{
#define MaxNumberImageMoments  8

  ChannelMoments
    *channel_moments;

  double
    M00[CompositeChannels+1],
    M01[CompositeChannels+1],
    M02[CompositeChannels+1],
    M03[CompositeChannels+1],
    M10[CompositeChannels+1],
    M11[CompositeChannels+1],
    M12[CompositeChannels+1],
    M20[CompositeChannels+1],
    M21[CompositeChannels+1],
    M22[CompositeChannels+1],
    M30[CompositeChannels+1];

  MagickPixelPacket
    pixel;

  PointInfo
    centroid[CompositeChannels+1];

  ssize_t
    channel,
    channels,
    y;

  size_t
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  length=CompositeChannels+1UL;
  channel_moments=(ChannelMoments *) AcquireQuantumMemory(length,
    sizeof(*channel_moments));
  if (channel_moments == (ChannelMoments *) NULL)
    return(channel_moments);
  (void) memset(channel_moments,0,length*sizeof(*channel_moments));
  (void) memset(centroid,0,sizeof(centroid));
  (void) memset(M00,0,sizeof(M00));
  (void) memset(M01,0,sizeof(M01));
  (void) memset(M02,0,sizeof(M02));
  (void) memset(M03,0,sizeof(M03));
  (void) memset(M10,0,sizeof(M10));
  (void) memset(M11,0,sizeof(M11));
  (void) memset(M12,0,sizeof(M12));
  (void) memset(M20,0,sizeof(M20));
  (void) memset(M21,0,sizeof(M21));
  (void) memset(M22,0,sizeof(M22));
  (void) memset(M30,0,sizeof(M30));
  GetMagickPixelPacket(image,&pixel);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    /*
      Compute center of mass (centroid).
    */
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetMagickPixelPacket(image,p,indexes+x,&pixel);
      M00[RedChannel]+=QuantumScale*pixel.red;
      M10[RedChannel]+=x*QuantumScale*pixel.red;
      M01[RedChannel]+=y*QuantumScale*pixel.red;
      M00[GreenChannel]+=QuantumScale*pixel.green;
      M10[GreenChannel]+=x*QuantumScale*pixel.green;
      M01[GreenChannel]+=y*QuantumScale*pixel.green;
      M00[BlueChannel]+=QuantumScale*pixel.blue;
      M10[BlueChannel]+=x*QuantumScale*pixel.blue;
      M01[BlueChannel]+=y*QuantumScale*pixel.blue;
      if (image->matte != MagickFalse)
        {
          M00[OpacityChannel]+=QuantumScale*pixel.opacity;
          M10[OpacityChannel]+=x*QuantumScale*pixel.opacity;
          M01[OpacityChannel]+=y*QuantumScale*pixel.opacity;
        }
      if (image->colorspace == CMYKColorspace)
        {
          M00[IndexChannel]+=QuantumScale*pixel.index;
          M10[IndexChannel]+=x*QuantumScale*pixel.index;
          M01[IndexChannel]+=y*QuantumScale*pixel.index;
        }
      p++;
    }
  }
  for (channel=0; channel <= CompositeChannels; channel++)
  {
    /*
      Compute center of mass (centroid).
    */
    if (M00[channel] < MagickEpsilon)
      {
        M00[channel]+=MagickEpsilon;
        centroid[channel].x=(double) image->columns/2.0;
        centroid[channel].y=(double) image->rows/2.0;
        continue;
      }
    M00[channel]+=MagickEpsilon;
    centroid[channel].x=M10[channel]/M00[channel];
    centroid[channel].y=M01[channel]/M00[channel];
  }
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    /*
      Compute the image moments.
    */
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetMagickPixelPacket(image,p,indexes+x,&pixel);
      M11[RedChannel]+=(x-centroid[RedChannel].x)*(y-
        centroid[RedChannel].y)*QuantumScale*pixel.red;
      M20[RedChannel]+=(x-centroid[RedChannel].x)*(x-
        centroid[RedChannel].x)*QuantumScale*pixel.red;
      M02[RedChannel]+=(y-centroid[RedChannel].y)*(y-
        centroid[RedChannel].y)*QuantumScale*pixel.red;
      M21[RedChannel]+=(x-centroid[RedChannel].x)*(x-
        centroid[RedChannel].x)*(y-centroid[RedChannel].y)*QuantumScale*
        pixel.red;
      M12[RedChannel]+=(x-centroid[RedChannel].x)*(y-
        centroid[RedChannel].y)*(y-centroid[RedChannel].y)*QuantumScale*
        pixel.red;
      M22[RedChannel]+=(x-centroid[RedChannel].x)*(x-
        centroid[RedChannel].x)*(y-centroid[RedChannel].y)*(y-
        centroid[RedChannel].y)*QuantumScale*pixel.red;
      M30[RedChannel]+=(x-centroid[RedChannel].x)*(x-
        centroid[RedChannel].x)*(x-centroid[RedChannel].x)*QuantumScale*
        pixel.red;
      M03[RedChannel]+=(y-centroid[RedChannel].y)*(y-
        centroid[RedChannel].y)*(y-centroid[RedChannel].y)*QuantumScale*
        pixel.red;
      M11[GreenChannel]+=(x-centroid[GreenChannel].x)*(y-
        centroid[GreenChannel].y)*QuantumScale*pixel.green;
      M20[GreenChannel]+=(x-centroid[GreenChannel].x)*(x-
        centroid[GreenChannel].x)*QuantumScale*pixel.green;
      M02[GreenChannel]+=(y-centroid[GreenChannel].y)*(y-
        centroid[GreenChannel].y)*QuantumScale*pixel.green;
      M21[GreenChannel]+=(x-centroid[GreenChannel].x)*(x-
        centroid[GreenChannel].x)*(y-centroid[GreenChannel].y)*QuantumScale*
        pixel.green;
      M12[GreenChannel]+=(x-centroid[GreenChannel].x)*(y-
        centroid[GreenChannel].y)*(y-centroid[GreenChannel].y)*QuantumScale*
        pixel.green;
      M22[GreenChannel]+=(x-centroid[GreenChannel].x)*(x-
        centroid[GreenChannel].x)*(y-centroid[GreenChannel].y)*(y-
        centroid[GreenChannel].y)*QuantumScale*pixel.green;
      M30[GreenChannel]+=(x-centroid[GreenChannel].x)*(x-
        centroid[GreenChannel].x)*(x-centroid[GreenChannel].x)*QuantumScale*
        pixel.green;
      M03[GreenChannel]+=(y-centroid[GreenChannel].y)*(y-
        centroid[GreenChannel].y)*(y-centroid[GreenChannel].y)*QuantumScale*
        pixel.green;
      M11[BlueChannel]+=(x-centroid[BlueChannel].x)*(y-
        centroid[BlueChannel].y)*QuantumScale*pixel.blue;
      M20[BlueChannel]+=(x-centroid[BlueChannel].x)*(x-
        centroid[BlueChannel].x)*QuantumScale*pixel.blue;
      M02[BlueChannel]+=(y-centroid[BlueChannel].y)*(y-
        centroid[BlueChannel].y)*QuantumScale*pixel.blue;
      M21[BlueChannel]+=(x-centroid[BlueChannel].x)*(x-
        centroid[BlueChannel].x)*(y-centroid[BlueChannel].y)*QuantumScale*
        pixel.blue;
      M12[BlueChannel]+=(x-centroid[BlueChannel].x)*(y-
        centroid[BlueChannel].y)*(y-centroid[BlueChannel].y)*QuantumScale*
        pixel.blue;
      M22[BlueChannel]+=(x-centroid[BlueChannel].x)*(x-
        centroid[BlueChannel].x)*(y-centroid[BlueChannel].y)*(y-
        centroid[BlueChannel].y)*QuantumScale*pixel.blue;
      M30[BlueChannel]+=(x-centroid[BlueChannel].x)*(x-
        centroid[BlueChannel].x)*(x-centroid[BlueChannel].x)*QuantumScale*
        pixel.blue;
      M03[BlueChannel]+=(y-centroid[BlueChannel].y)*(y-
        centroid[BlueChannel].y)*(y-centroid[BlueChannel].y)*QuantumScale*
        pixel.blue;
      if (image->matte != MagickFalse)
        {
          M11[OpacityChannel]+=(x-centroid[OpacityChannel].x)*(y-
            centroid[OpacityChannel].y)*QuantumScale*pixel.opacity;
          M20[OpacityChannel]+=(x-centroid[OpacityChannel].x)*(x-
            centroid[OpacityChannel].x)*QuantumScale*pixel.opacity;
          M02[OpacityChannel]+=(y-centroid[OpacityChannel].y)*(y-
            centroid[OpacityChannel].y)*QuantumScale*pixel.opacity;
          M21[OpacityChannel]+=(x-centroid[OpacityChannel].x)*(x-
            centroid[OpacityChannel].x)*(y-centroid[OpacityChannel].y)*
            QuantumScale*pixel.opacity;
          M12[OpacityChannel]+=(x-centroid[OpacityChannel].x)*(y-
            centroid[OpacityChannel].y)*(y-centroid[OpacityChannel].y)*
            QuantumScale*pixel.opacity;
          M22[OpacityChannel]+=(x-centroid[OpacityChannel].x)*(x-
            centroid[OpacityChannel].x)*(y-centroid[OpacityChannel].y)*(y-
            centroid[OpacityChannel].y)*QuantumScale*pixel.opacity;
          M30[OpacityChannel]+=(x-centroid[OpacityChannel].x)*(x-
            centroid[OpacityChannel].x)*(x-centroid[OpacityChannel].x)*
            QuantumScale*pixel.opacity;
          M03[OpacityChannel]+=(y-centroid[OpacityChannel].y)*(y-
            centroid[OpacityChannel].y)*(y-centroid[OpacityChannel].y)*
            QuantumScale*pixel.opacity;
        }
      if (image->colorspace == CMYKColorspace)
        {
          M11[IndexChannel]+=(x-centroid[IndexChannel].x)*(y-
            centroid[IndexChannel].y)*QuantumScale*pixel.index;
          M20[IndexChannel]+=(x-centroid[IndexChannel].x)*(x-
            centroid[IndexChannel].x)*QuantumScale*pixel.index;
          M02[IndexChannel]+=(y-centroid[IndexChannel].y)*(y-
            centroid[IndexChannel].y)*QuantumScale*pixel.index;
          M21[IndexChannel]+=(x-centroid[IndexChannel].x)*(x-
            centroid[IndexChannel].x)*(y-centroid[IndexChannel].y)*
            QuantumScale*pixel.index;
          M12[IndexChannel]+=(x-centroid[IndexChannel].x)*(y-
            centroid[IndexChannel].y)*(y-centroid[IndexChannel].y)*
            QuantumScale*pixel.index;
          M22[IndexChannel]+=(x-centroid[IndexChannel].x)*(x-
            centroid[IndexChannel].x)*(y-centroid[IndexChannel].y)*(y-
            centroid[IndexChannel].y)*QuantumScale*pixel.index;
          M30[IndexChannel]+=(x-centroid[IndexChannel].x)*(x-
            centroid[IndexChannel].x)*(x-centroid[IndexChannel].x)*
            QuantumScale*pixel.index;
          M03[IndexChannel]+=(y-centroid[IndexChannel].y)*(y-
            centroid[IndexChannel].y)*(y-centroid[IndexChannel].y)*
            QuantumScale*pixel.index;
        }
      p++;
    }
  }
  channels=3;
  M00[CompositeChannels]+=(M00[RedChannel]+M00[GreenChannel]+M00[BlueChannel]);
  M01[CompositeChannels]+=(M01[RedChannel]+M01[GreenChannel]+M01[BlueChannel]);
  M02[CompositeChannels]+=(M02[RedChannel]+M02[GreenChannel]+M02[BlueChannel]);
  M03[CompositeChannels]+=(M03[RedChannel]+M03[GreenChannel]+M03[BlueChannel]);
  M10[CompositeChannels]+=(M10[RedChannel]+M10[GreenChannel]+M10[BlueChannel]);
  M11[CompositeChannels]+=(M11[RedChannel]+M11[GreenChannel]+M11[BlueChannel]);
  M12[CompositeChannels]+=(M12[RedChannel]+M12[GreenChannel]+M12[BlueChannel]);
  M20[CompositeChannels]+=(M20[RedChannel]+M20[GreenChannel]+M20[BlueChannel]);
  M21[CompositeChannels]+=(M21[RedChannel]+M21[GreenChannel]+M21[BlueChannel]);
  M22[CompositeChannels]+=(M22[RedChannel]+M22[GreenChannel]+M22[BlueChannel]);
  M30[CompositeChannels]+=(M30[RedChannel]+M30[GreenChannel]+M30[BlueChannel]);
  if (image->matte != MagickFalse)
    {
      channels+=1;
      M00[CompositeChannels]+=M00[OpacityChannel];
      M01[CompositeChannels]+=M01[OpacityChannel];
      M02[CompositeChannels]+=M02[OpacityChannel];
      M03[CompositeChannels]+=M03[OpacityChannel];
      M10[CompositeChannels]+=M10[OpacityChannel];
      M11[CompositeChannels]+=M11[OpacityChannel];
      M12[CompositeChannels]+=M12[OpacityChannel];
      M20[CompositeChannels]+=M20[OpacityChannel];
      M21[CompositeChannels]+=M21[OpacityChannel];
      M22[CompositeChannels]+=M22[OpacityChannel];
      M30[CompositeChannels]+=M30[OpacityChannel];
    }
  if (image->colorspace == CMYKColorspace)
    {
      channels+=1;
      M00[CompositeChannels]+=M00[IndexChannel];
      M01[CompositeChannels]+=M01[IndexChannel];
      M02[CompositeChannels]+=M02[IndexChannel];
      M03[CompositeChannels]+=M03[IndexChannel];
      M10[CompositeChannels]+=M10[IndexChannel];
      M11[CompositeChannels]+=M11[IndexChannel];
      M12[CompositeChannels]+=M12[IndexChannel];
      M20[CompositeChannels]+=M20[IndexChannel];
      M21[CompositeChannels]+=M21[IndexChannel];
      M22[CompositeChannels]+=M22[IndexChannel];
      M30[CompositeChannels]+=M30[IndexChannel];
    }
  M00[CompositeChannels]/=(double) channels;
  M01[CompositeChannels]/=(double) channels;
  M02[CompositeChannels]/=(double) channels;
  M03[CompositeChannels]/=(double) channels;
  M10[CompositeChannels]/=(double) channels;
  M11[CompositeChannels]/=(double) channels;
  M12[CompositeChannels]/=(double) channels;
  M20[CompositeChannels]/=(double) channels;
  M21[CompositeChannels]/=(double) channels;
  M22[CompositeChannels]/=(double) channels;
  M30[CompositeChannels]/=(double) channels;
  for (channel=0; channel <= CompositeChannels; channel++)
  {
    /*
      Compute elliptical angle, major and minor axes, eccentricity, & intensity.
    */
    channel_moments[channel].centroid=centroid[channel];
    channel_moments[channel].ellipse_axis.x=sqrt((2.0/M00[channel])*
      ((M20[channel]+M02[channel])+sqrt(4.0*M11[channel]*M11[channel]+
      (M20[channel]-M02[channel])*(M20[channel]-M02[channel]))));
    channel_moments[channel].ellipse_axis.y=sqrt((2.0/M00[channel])*
      ((M20[channel]+M02[channel])-sqrt(4.0*M11[channel]*M11[channel]+
      (M20[channel]-M02[channel])*(M20[channel]-M02[channel]))));
    channel_moments[channel].ellipse_angle=RadiansToDegrees(0.5*atan(2.0*
      M11[channel]/(M20[channel]-M02[channel]+MagickEpsilon)));
    if (fabs(M11[channel]) < MagickEpsilon)
      {
        if (fabs(M20[channel]-M02[channel]) < MagickEpsilon)
          channel_moments[channel].ellipse_angle+=0.0;
        else
          if ((M20[channel]-M02[channel]) < 0.0)
            channel_moments[channel].ellipse_angle+=90.0;
          else
            channel_moments[channel].ellipse_angle+=0.0;
      }
    else
      if (M11[channel] < 0.0)
        {
          if (fabs(M20[channel]-M02[channel]) < MagickEpsilon)
            channel_moments[channel].ellipse_angle+=0.0;
          else
            if ((M20[channel]-M02[channel]) < 0.0)
              channel_moments[channel].ellipse_angle+=90.0;
            else
              channel_moments[channel].ellipse_angle+=180.0;
        }
      else
        {
          if (fabs(M20[channel]-M02[channel]) < MagickEpsilon)
            channel_moments[channel].ellipse_angle+=0.0;
          else
            if ((M20[channel]-M02[channel]) < 0.0)
              channel_moments[channel].ellipse_angle+=90.0;
            else
              channel_moments[channel].ellipse_angle+=0.0;
       }
    channel_moments[channel].ellipse_eccentricity=sqrt(1.0-(
      channel_moments[channel].ellipse_axis.y/
      (channel_moments[channel].ellipse_axis.x+MagickEpsilon)));
    channel_moments[channel].ellipse_intensity=M00[channel]/
      (MagickPI*channel_moments[channel].ellipse_axis.x*
      channel_moments[channel].ellipse_axis.y+MagickEpsilon);
  }
  for (channel=0; channel <= CompositeChannels; channel++)
  {
    /*
      Normalize image moments.
    */
    M10[channel]=0.0;
    M01[channel]=0.0;
    M11[channel]/=pow(M00[channel],1.0+(1.0+1.0)/2.0);
    M20[channel]/=pow(M00[channel],1.0+(2.0+0.0)/2.0);
    M02[channel]/=pow(M00[channel],1.0+(0.0+2.0)/2.0);
    M21[channel]/=pow(M00[channel],1.0+(2.0+1.0)/2.0);
    M12[channel]/=pow(M00[channel],1.0+(1.0+2.0)/2.0);
    M22[channel]/=pow(M00[channel],1.0+(2.0+2.0)/2.0);
    M30[channel]/=pow(M00[channel],1.0+(3.0+0.0)/2.0);
    M03[channel]/=pow(M00[channel],1.0+(0.0+3.0)/2.0);
    M00[channel]=1.0;
  }
  for (channel=0; channel <= CompositeChannels; channel++)
  {
    /*
      Compute Hu invariant moments.
    */
    channel_moments[channel].I[0]=M20[channel]+M02[channel];
    channel_moments[channel].I[1]=(M20[channel]-M02[channel])*
      (M20[channel]-M02[channel])+4.0*M11[channel]*M11[channel];
    channel_moments[channel].I[2]=(M30[channel]-3.0*M12[channel])*
      (M30[channel]-3.0*M12[channel])+(3.0*M21[channel]-M03[channel])*
      (3.0*M21[channel]-M03[channel]);
    channel_moments[channel].I[3]=(M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])+(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]);
    channel_moments[channel].I[4]=(M30[channel]-3.0*M12[channel])*
      (M30[channel]+M12[channel])*((M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-3.0*(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]))+(3.0*M21[channel]-M03[channel])*
      (M21[channel]+M03[channel])*(3.0*(M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]));
    channel_moments[channel].I[5]=(M20[channel]-M02[channel])*
      ((M30[channel]+M12[channel])*(M30[channel]+M12[channel])-
      (M21[channel]+M03[channel])*(M21[channel]+M03[channel]))+
      4.0*M11[channel]*(M30[channel]+M12[channel])*(M21[channel]+M03[channel]);
    channel_moments[channel].I[6]=(3.0*M21[channel]-M03[channel])*
      (M30[channel]+M12[channel])*((M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-3.0*(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]))-(M30[channel]-3*M12[channel])*
      (M21[channel]+M03[channel])*(3.0*(M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]));
    channel_moments[channel].I[7]=M11[channel]*((M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-(M03[channel]+M21[channel])*
      (M03[channel]+M21[channel]))-(M20[channel]-M02[channel])*
      (M30[channel]+M12[channel])*(M03[channel]+M21[channel]);
  }
  if (y < (ssize_t) image->rows)
    channel_moments=(ChannelMoments *) RelinquishMagickMemory(channel_moments);
  return(channel_moments);
}