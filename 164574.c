MagickExport Image *ForwardFourierTransformImage(const Image *image,
  const MagickBooleanType modulus,ExceptionInfo *exception)
{
  Image
    *fourier_image;

  fourier_image=NewImageList();
#if !defined(MAGICKCORE_FFTW_DELEGATE)
  (void) modulus;
  (void) ThrowMagickException(exception,GetMagickModule(),
    MissingDelegateWarning,"DelegateLibrarySupportNotBuiltIn","`%s' (FFTW)",
    image->filename);
#else
  {
    Image
      *magnitude_image;

    size_t
      height,
      width;

    width=image->columns;
    height=image->rows;
    if ((image->columns != image->rows) || ((image->columns % 2) != 0) ||
        ((image->rows % 2) != 0))
      {
        size_t extent=image->columns < image->rows ? image->rows :
          image->columns;
        width=(extent & 0x01) == 1 ? extent+1UL : extent;
      }
    height=width;
    magnitude_image=CloneImage(image,width,height,MagickTrue,exception);
    if (magnitude_image != (Image *) NULL)
      {
        Image
          *phase_image;

        magnitude_image->storage_class=DirectClass;
        magnitude_image->depth=32UL;
        phase_image=CloneImage(image,width,height,MagickTrue,exception);
        if (phase_image == (Image *) NULL)
          magnitude_image=DestroyImage(magnitude_image);
        else
          {
            MagickBooleanType
              is_gray,
              status;

            phase_image->storage_class=DirectClass;
            phase_image->depth=32UL;
            AppendImageToList(&fourier_image,magnitude_image);
            AppendImageToList(&fourier_image,phase_image);
            status=MagickTrue;
            is_gray=IsImageGray(image);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
            #pragma omp parallel sections
#endif
            {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp section
#endif
              {
                MagickBooleanType
                  thread_status;

                if (is_gray != MagickFalse)
                  thread_status=ForwardFourierTransformChannel(image,
                    GrayPixelChannel,modulus,fourier_image,exception);
                else
                  thread_status=ForwardFourierTransformChannel(image,
                    RedPixelChannel,modulus,fourier_image,exception);
                if (thread_status == MagickFalse)
                  status=thread_status;
              }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp section
#endif
              {
                MagickBooleanType
                  thread_status;

                thread_status=MagickTrue;
                if (is_gray == MagickFalse)
                  thread_status=ForwardFourierTransformChannel(image,
                    GreenPixelChannel,modulus,fourier_image,exception);
                if (thread_status == MagickFalse)
                  status=thread_status;
              }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp section
#endif
              {
                MagickBooleanType
                  thread_status;

                thread_status=MagickTrue;
                if (is_gray == MagickFalse)
                  thread_status=ForwardFourierTransformChannel(image,
                    BluePixelChannel,modulus,fourier_image,exception);
                if (thread_status == MagickFalse)
                  status=thread_status;
              }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp section
#endif
              {
                MagickBooleanType
                  thread_status;

                thread_status=MagickTrue;
                if (image->colorspace == CMYKColorspace)
                  thread_status=ForwardFourierTransformChannel(image,
                    BlackPixelChannel,modulus,fourier_image,exception);
                if (thread_status == MagickFalse)
                  status=thread_status;
              }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp section
#endif
              {
                MagickBooleanType
                  thread_status;

                thread_status=MagickTrue;
                if (image->alpha_trait != UndefinedPixelTrait)
                  thread_status=ForwardFourierTransformChannel(image,
                    AlphaPixelChannel,modulus,fourier_image,exception);
                if (thread_status == MagickFalse)
                  status=thread_status;
              }
            }
            if (status == MagickFalse)
              fourier_image=DestroyImageList(fourier_image);
            fftw_cleanup();
          }
      }
  }
#endif
  return(fourier_image);
}