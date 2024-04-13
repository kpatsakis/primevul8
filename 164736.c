MagickExport void ImagesToCustomStream(const ImageInfo *image_info,
  Image *images,ExceptionInfo *exception)
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
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  assert(image_info->custom_stream != (CustomStreamInfo *) NULL);
  assert(image_info->custom_stream->signature == MagickCoreSignature);
  assert(image_info->custom_stream->reader != (CustomStreamHandler) NULL);
  assert(image_info->custom_stream->writer != (CustomStreamHandler) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  clone_info=CloneImageInfo(image_info);
  (void) SetImageInfo(clone_info,(unsigned int) GetImageListLength(images),
    exception);
  if (*clone_info->magick != '\0')
    (void) CopyMagickString(images->magick,clone_info->magick,MagickPathExtent);
  magick_info=GetMagickInfo(images->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoEncodeDelegateForThisImageFormat","`%s'",
        images->magick);
      clone_info=DestroyImageInfo(clone_info);
      return;
    }
  (void) CopyMagickString(clone_info->magick,images->magick,MagickPathExtent);
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
      (void) CloseBlob(images);
      *images->filename='\0';
      (void) WriteImages(clone_info,images,images->filename,exception);
      (void) CloseBlob(images);
    }
  else
    {
      char
        filename[MagickPathExtent],
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

          (void) FormatLocaleString(filename,MagickPathExtent,"%s:%s",
            images->magick,unique);
          status=WriteImages(clone_info,images,filename,exception);
          (void) CloseBlob(images);
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