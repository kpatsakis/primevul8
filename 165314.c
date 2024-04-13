gs_window_xevent (GSWindow  *window,
                  GdkXEvent *xevent)
{
        XEvent *ev;

        ev = xevent;

        /* MapNotify is used to tell us when new windows are mapped.
           ConfigureNofify is used to tell us when windows are raised. */
        switch (ev->xany.type) {
        case MapNotify:
                {
                        XMapEvent *xme = &ev->xmap;

                        if (! x11_window_is_ours (xme->window)) {
                                gs_window_raise (window);
                        } else {
                                gs_debug ("not raising our windows");
                        }

                        break;
                }
        case ConfigureNotify:
                {
                        XConfigureEvent *xce = &ev->xconfigure;

                        if (! x11_window_is_ours (xce->window)) {
                                gs_window_raise (window);
                        } else {
                                gs_debug ("not raising our windows");
                        }

                        break;
                }
        default:
                /* extension events */
#ifdef HAVE_SHAPE_EXT
                if (ev->xany.type == (window->priv->shape_event_base + ShapeNotify)) {
                        /*XShapeEvent *xse = (XShapeEvent *) ev;*/
                        unshape_window (window);
                        gs_debug ("Window was reshaped!");
                }
#endif

                break;
        }

}