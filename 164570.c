static MagickBooleanType InverseFourierTransformChannel(
  const Image *magnitude_image,const Image *phase_image,
  const PixelChannel channel,const MagickBooleanType modulus,
  Image *fourier_image,ExceptionInfo *exception)
{
  fftw_complex
    *inverse_pixels;

  FourierInfo
    fourier_info;

  MagickBooleanType
    status;

  MemoryInfo
    *inverse_info;

  fourier_info.width=magnitude_image->columns;
  fourier_info.height=magnitude_image->rows;
  if ((magnitude_image->columns != magnitude_image->rows) ||
      ((magnitude_image->columns % 2) != 0) ||
      ((magnitude_image->rows % 2) != 0))
    {
      size_t extent=magnitude_image->columns < magnitude_image->rows ?
        magnitude_image->rows : magnitude_image->columns;
      fourier_info.width=(extent & 0x01) == 1 ? extent+1UL : extent;
    }
  fourier_info.height=fourier_info.width;
  fourier_info.center=(ssize_t) (fourier_info.width/2L)+1L;
  fourier_info.channel=channel;
  fourier_info.modulus=modulus;
  inverse_info=AcquireVirtualMemory((size_t) fourier_info.width,
    (fourier_info.height/2+1)*sizeof(*inverse_pixels));
  if (inverse_info == (MemoryInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",
        magnitude_image->filename);
      return(MagickFalse);
    }
  inverse_pixels=(fftw_complex *) GetVirtualMemoryBlob(inverse_info);
  status=InverseFourier(&fourier_info,magnitude_image,phase_image,
    inverse_pixels,exception);
  if (status != MagickFalse)
    status=InverseFourierTransform(&fourier_info,inverse_pixels,fourier_image,
      exception);
  inverse_info=RelinquishVirtualMemory(inverse_info);
  return(status);
}