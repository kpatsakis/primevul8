MagickExport Image *CustomStreamToImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  Image
    *image;

  ImageInfo
    *blob_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image_info->custom_stream != (CustomStreamInfo *) NULL);
  assert(image_info->custom_stream->signature == MagickCoreSignature);
  assert(image_info->custom_stream->reader != (CustomStreamHandler) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  blob_info=CloneImageInfo(image_info);
  if (*blob_info->magick == '\0')
    (void) SetImageInfo(blob_info,0,exception);
  magick_info=GetMagickInfo(blob_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
        blob_info->magick);
      blob_info=DestroyImageInfo(blob_info);
      return((Image *) NULL);
    }
  image=(Image *) NULL;
  if ((GetMagickBlobSupport(magick_info) != MagickFalse) ||
      (*blob_info->filename != '\0'))
    {
      char
        filename[MagickPathExtent];

      /*
        Native blob support for this image format or SetImageInfo changed the
        blob to a file.
      */
      (void) CopyMagickString(filename,blob_info->filename,MagickPathExtent);
      (void) FormatLocaleString(blob_info->filename,MagickPathExtent,"%s:%s",
        blob_info->magick,filename);
      image=ReadImage(blob_info,exception);
      if (image != (Image *) NULL)
        (void) CloseBlob(image);
    }
  else
    {
      char
        unique[MagickPathExtent];

      int
        file;

      ImageInfo
        *clone_info;

      unsigned char
        *blob;

      /*
        Write data to file on disk.
      */
      blob_info->custom_stream=(CustomStreamInfo *) NULL;
      blob=(unsigned char *) AcquireQuantumMemory(MagickMaxBufferExtent,
        sizeof(*blob));
      if (blob == (unsigned char *) NULL)
        {
          ThrowFileException(exception,BlobError,"UnableToReadBlob",
            image_info->filename);
          blob_info=DestroyImageInfo(blob_info);
          return((Image *) NULL);
        }
      file=AcquireUniqueFileResource(unique);
      if (file == -1)
        {
          ThrowFileException(exception,BlobError,"UnableToReadBlob",
            image_info->filename);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          blob_info=DestroyImageInfo(blob_info);
          return((Image *) NULL);
        }
      clone_info=CloneImageInfo(blob_info);
      blob_info->file=fdopen(file,"wb+");
      if (blob_info->file != (FILE *) NULL)
        {
          ssize_t
            count;

          count=(ssize_t) MagickMaxBufferExtent;
          while (count == (ssize_t) MagickMaxBufferExtent)
          {
            count=image_info->custom_stream->reader(blob,MagickMaxBufferExtent,
              image_info->custom_stream->data);
            count=(ssize_t) write(file,(const char *) blob,(size_t) count);
          }
          (void) fclose(blob_info->file);
          (void) FormatLocaleString(clone_info->filename,MagickPathExtent,
            "%s:%s",blob_info->magick,unique);
          image=ReadImage(clone_info,exception);
          if (image != (Image *) NULL)
            {
              Image
                *images;

              /*
                Restore original filenames and image format.
              */
              for (images=GetFirstImageInList(image); images != (Image *) NULL; )
              {
                (void) CopyMagickString(images->filename,image_info->filename,
                  MagickPathExtent);
                (void) CopyMagickString(images->magick_filename,
                  image_info->filename,MagickPathExtent);
                (void) CopyMagickString(images->magick,magick_info->name,
                  MagickPathExtent);
                (void) CloseBlob(images);
                images=GetNextImageInList(images);
              }
            }
        }
      clone_info=DestroyImageInfo(clone_info);
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      (void) RelinquishUniqueFileResource(unique);
    }
  blob_info=DestroyImageInfo(blob_info);
  return(image);
}