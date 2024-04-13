MagickPrivate MagickBooleanType RecordProfileData(MagickCLDevice device,
  cl_kernel kernel,cl_event event)
{
  char
    *name;

  cl_int
    status;

  cl_ulong
    elapsed,
    end,
    start;

  KernelProfileRecord
    profile_record;

  size_t
    i,
    length;

  if (device->profile_kernels == MagickFalse)
    return(MagickFalse);
  status=openCL_library->clWaitForEvents(1,&event);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  status=openCL_library->clGetKernelInfo(kernel,CL_KERNEL_FUNCTION_NAME,0,NULL,
    &length);
  if (status != CL_SUCCESS)
    return(MagickTrue);
  name=AcquireQuantumMemory(length,sizeof(*name));
  if (name == (char *) NULL)
    return(MagickTrue);
  start=end=elapsed=0;
  status=openCL_library->clGetKernelInfo(kernel,CL_KERNEL_FUNCTION_NAME,length,
    name,(size_t *) NULL);
  status|=openCL_library->clGetEventProfilingInfo(event,
    CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&start,NULL);
  status|=openCL_library->clGetEventProfilingInfo(event,
    CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&end,NULL);
  if (status != CL_SUCCESS)
    {
      name=DestroyString(name);
      return(MagickTrue);
    }
  start/=1000; // usecs
  end/=1000;   // usecs
  elapsed=end-start;
  LockSemaphoreInfo(device->lock);
  i=0;
  profile_record=(KernelProfileRecord) NULL;
  if (device->profile_records != (KernelProfileRecord *) NULL)
    {
      while (device->profile_records[i] != (KernelProfileRecord) NULL)
      {
        if (LocaleCompare(device->profile_records[i]->kernel_name,name) == 0)
          {
            profile_record=device->profile_records[i];
            break;
          }
        i++;
      }
    }
  if (profile_record != (KernelProfileRecord) NULL)
    name=DestroyString(name);
  else
    {
      profile_record=AcquireMagickMemory(sizeof(*profile_record));
      (void) ResetMagickMemory(profile_record,0,sizeof(*profile_record));
      profile_record->kernel_name=name;
      device->profile_records=ResizeMagickMemory(device->profile_records,(i+2)*
        sizeof(*device->profile_records));
      device->profile_records[i]=profile_record;
      device->profile_records[i+1]=(KernelProfileRecord) NULL;
    }
  if ((elapsed < profile_record->min) || (profile_record->count == 0))
    profile_record->min=elapsed;
  if (elapsed > profile_record->max)
    profile_record->max=elapsed;
  profile_record->total+=elapsed;
  profile_record->count+=1;
  UnlockSemaphoreInfo(device->lock);
  return(MagickTrue);
}