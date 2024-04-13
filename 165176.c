MagickPrivate MagickBooleanType OpenCLThrowMagickException(
  MagickCLDevice device,ExceptionInfo *exception,const char *module,
  const char *function,const size_t line,const ExceptionType severity,
  const char *tag,const char *format,...)
{
  MagickBooleanType
    status;

  assert(device != (MagickCLDevice) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);

  status=MagickTrue;
  if (severity != 0)
  {
    if (device->type == CL_DEVICE_TYPE_CPU)
    {
      /* Workaround for Intel OpenCL CPU runtime bug */
      /* Turn off OpenCL when a problem is detected! */
      if (strncmp(device->platform_name, "Intel",5) == 0)
        default_CLEnv->enabled=MagickFalse;
    }
  }

#ifdef OPENCLLOG_ENABLED
  {
    va_list
      operands;
    va_start(operands,format);
    status=ThrowMagickExceptionList(exception,module,function,line,severity,tag,
      format,operands);
    va_end(operands);
  }
#else
  magick_unreferenced(module);
  magick_unreferenced(function);
  magick_unreferenced(line);
  magick_unreferenced(tag);
  magick_unreferenced(format);
#endif

  return(status);
}