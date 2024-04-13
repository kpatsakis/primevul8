static MagickBooleanType ForwardFourierTransformChannel(const Image *image,
  const PixelChannel channel,const MagickBooleanType modulus,
  Image *fourier_image,ExceptionInfo *exception)
{
  double
    *magnitude_pixels,
    *phase_pixels;

  FourierInfo
    fourier_info;

  MagickBooleanType
    status;

  MemoryInfo
    *magnitude_info,
    *phase_info;

  fourier_info.width=image->columns;
  fourier_info.height=image->rows;
  if ((image->columns != image->rows) || ((image->columns % 2) != 0) ||
      ((image->rows % 2) != 0))
    {
      size_t extent=image->columns < image->rows ? image->rows : image->columns;
      fourier_info.width=(extent & 0x01) == 1 ? extent+1UL : extent;
    }
  fourier_info.height=fourier_info.width;
  fourier_info.center=(ssize_t) (fourier_info.width/2L)+1L;
  fourier_info.channel=channel;
  fourier_info.modulus=modulus;
  magnitude_info=AcquireVirtualMemory((size_t) fourier_info.width,
    (fourier_info.height/2+1)*sizeof(*magnitude_pixels));
  phase_info=AcquireVirtualMemory((size_t) fourier_info.width,
    (fourier_info.height/2+1)*sizeof(*phase_pixels));
  if ((magnitude_info == (MemoryInfo *) NULL) ||
      (phase_info == (MemoryInfo *) NULL))
    {
      if (phase_info != (MemoryInfo *) NULL)
        phase_info=RelinquishVirtualMemory(phase_info);
      if (magnitude_info == (MemoryInfo *) NULL)
        magnitude_info=RelinquishVirtualMemory(magnitude_info);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(MagickFalse);
    }
  magnitude_pixels=(double *) GetVirtualMemoryBlob(magnitude_info);
  phase_pixels=(double *) GetVirtualMemoryBlob(phase_info);
  status=ForwardFourierTransform(&fourier_info,image,magnitude_pixels,
    phase_pixels,exception);
  if (status != MagickFalse)
    status=ForwardFourier(&fourier_info,fourier_image,magnitude_pixels,
      phase_pixels,exception);
  phase_info=RelinquishVirtualMemory(phase_info);
  magnitude_info=RelinquishVirtualMemory(magnitude_info);
  return(status);
}