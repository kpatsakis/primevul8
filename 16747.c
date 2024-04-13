run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam   values[2];
  GimpRunMode        run_mode;
  GimpPDBStatusType  status = GIMP_PDB_SUCCESS;
  gint32             image_ID;
  gint32             drawable_ID;
  GimpExportReturn   export = GIMP_EXPORT_CANCEL;
  GError            *error  = NULL;

  run_mode = param[0].data.d_int32;

  INIT_I18N ();

  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;

  if (strcmp (name, LOAD_PROC) == 0)
    {
      image_ID = load_image (param[1].data.d_string, &error);

      if (image_ID != -1)
        {
          *nreturn_vals = 2;
          values[1].type         = GIMP_PDB_IMAGE;
          values[1].data.d_image = image_ID;
        }
      else
        {
          status = GIMP_PDB_EXECUTION_ERROR;
        }
    }
  else if (strcmp (name, SAVE_PROC) == 0)
    {
      GimpParasite *parasite;
      gint32        orig_image_ID;

      image_ID    = param[1].data.d_int32;
      drawable_ID = param[2].data.d_int32;

      orig_image_ID = image_ID;

      switch (run_mode)
        {
        case GIMP_RUN_INTERACTIVE:
        case GIMP_RUN_WITH_LAST_VALS:
          gimp_ui_init (PLUG_IN_BINARY, FALSE);

          export = gimp_export_image (&image_ID, &drawable_ID, "GBR",
                                      GIMP_EXPORT_CAN_HANDLE_GRAY |
                                      GIMP_EXPORT_CAN_HANDLE_RGB  |
                                      GIMP_EXPORT_CAN_HANDLE_ALPHA);

          if (export == GIMP_EXPORT_CANCEL)
            {
              values[0].data.d_status = GIMP_PDB_CANCEL;
              return;
            }

          /*  Possibly retrieve data  */
          gimp_get_data (SAVE_PROC, &info);
          break;

        default:
          break;
        }

      parasite = gimp_image_get_parasite (orig_image_ID, "gimp-brush-name");
      if (parasite)
        {
          gchar *name = g_strndup (gimp_parasite_data (parasite),
                                   gimp_parasite_data_size (parasite));
          gimp_parasite_free (parasite);

          strncpy (info.description, name, sizeof (info.description));
          info.description[sizeof (info.description) - 1] = '\0';

          g_free (name);
        }

      switch (run_mode)
        {
        case GIMP_RUN_INTERACTIVE:
          if (! save_dialog ())
            status = GIMP_PDB_CANCEL;
          break;

        case GIMP_RUN_NONINTERACTIVE:
          if (nparams != 7)
            {
              status = GIMP_PDB_CALLING_ERROR;
            }
          else
            {
              info.spacing = (param[5].data.d_int32);
              strncpy (info.description, param[6].data.d_string,
                       sizeof (info.description));
              info.description[sizeof (info.description) - 1] = '\0';
            }
          break;

        default:
          break;
        }

      if (status == GIMP_PDB_SUCCESS)
        {
          if (save_image (param[3].data.d_string, image_ID, drawable_ID,
                          &error))
            {
              gimp_set_data (SAVE_PROC, &info, sizeof (info));
            }
          else
            {
              status = GIMP_PDB_EXECUTION_ERROR;
            }
        }

      if (export == GIMP_EXPORT_EXPORT)
        gimp_image_delete (image_ID);

      if (info.description && strlen (info.description))
        {
          GimpParasite *parasite;

          parasite = gimp_parasite_new ("gimp-brush-name",
                                        GIMP_PARASITE_PERSISTENT,
                                        strlen (info.description) + 1,
                                        info.description);
          gimp_image_attach_parasite (orig_image_ID, parasite);
          gimp_parasite_free (parasite);
        }
      else
        {
          gimp_image_detach_parasite (orig_image_ID, "gimp-brush-name");
        }
    }
  else
    {
      status = GIMP_PDB_CALLING_ERROR;
    }

  if (status != GIMP_PDB_SUCCESS && error)
    {
      *nreturn_vals = 2;
      values[1].type          = GIMP_PDB_STRING;
      values[1].data.d_string = error->message;
    }

  values[0].data.d_status = status;
}