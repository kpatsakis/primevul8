MagickExport void ImageToCustomStream(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  MagickBooleanType
    blob_support,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image_info->custom_stream != (CustomStreamInfo *) NULL);
  assert(image_info->custom_stream->signature == MagickCoreSignature);
  assert(image_info->custom_stream->writer != (CustomStreamHandler) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  clone_info=CloneImageInfo(image_info);
  clone_info->adjoin=MagickFalse;
  (void) SetImageInfo(clone_info,1,exception);
  if (*clone_info->magick != '\0')
    (void) CopyMagickString(image->magick,clone_info->magick,MagickPathExtent);
  magick_info=GetMagickInfo(image->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoEncodeDelegateForThisImageFormat","`%s'",
        image->magick);
      clone_info=DestroyImageInfo(clone_info);
      return;
    }
  (void) CopyMagickString(clone_info->magick,image->magick,MagickPathExtent);
  blob_support=GetMagickBlobSupport(magick_info);
  if ((blob_support != MagickFalse) &&
      (GetMagickEncoderSeekableStream(magick_info) != MagickFalse))
    {
      if ((clone_info->custom_stream->seeker == (CustomStreamSeeker) NULL) ||
          (clone_info->custom_stream->teller == (CustomStreamTeller) NULL))
        blob_support=MagickFalse;
    }
  if (blob_support != MagickFalse)
    {
      /*
        Native blob support for this image format.
      */
      (void) CloseBlob(image);
      *image->filename='\0';
      (void) WriteImage(clone_info,image,exception);
      (void) CloseBlob(image);
    }
  else
    {
      char
        unique[MagickPathExtent];

      int
        file;

      unsigned char
        *blob;

      /*
        Write file to disk in blob image format.
      */
      clone_info->custom_stream=(CustomStreamInfo *) NULL;
      blob=(unsigned char *) AcquireQuantumMemory(MagickMaxBufferExtent,
        sizeof(*blob));
      if (blob == (unsigned char *) NULL)
        {
          ThrowFileException(exception,BlobError,"UnableToWriteBlob",
            image_info->filename);
          clone_info=DestroyImageInfo(clone_info);
          return;
        }
      file=AcquireUniqueFileResource(unique);
      if (file == -1)
        {
          ThrowFileException(exception,BlobError,"UnableToWriteBlob",
            image_info->filename);
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          clone_info=DestroyImageInfo(clone_info);
          return;
        }
      clone_info->file=fdopen(file,"wb+");
      if (clone_info->file != (FILE *) NULL)
        {
          ssize_t
            count;

          (void) FormatLocaleString(image->filename,MagickPathExtent,
            "%s:%s",image->magick,unique);
          status=WriteImage(clone_info,image,exception);
          (void) CloseBlob(image);
          if (status != MagickFalse)
            {
              (void) fseek(clone_info->file,0,SEEK_SET);
              count=(ssize_t) MagickMaxBufferExtent;
              while (count == (ssize_t) MagickMaxBufferExtent)
              {
                count=(ssize_t) fread(blob,sizeof(*blob),MagickMaxBufferExtent,
                  clone_info->file);
                (void) image_info->custom_stream->writer(blob,(size_t) count,
                  image_info->custom_stream->data);
              }
            }
          (void) fclose(clone_info->file);
        }
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      (void) RelinquishUniqueFileResource(unique);
    }
  clone_info=DestroyImageInfo(clone_info);
}