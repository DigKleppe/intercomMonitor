/*
 * videoOverlay.c
 *
 *  Created on: Jun 13, 2023
 *      Author: dig
 */


 #include <gst/video/videooverlay.h>
 #include <gtk/gtk.h>
 #ifdef GDK_WINDOWING_X11
 #include <gdk/gdkx.h>  // for GDK_WINDOW_XID
 #endif
 #ifdef GDK_WINDOWING_WIN32
 #include <gdk/gdkwin32.h>  // for GDK_WINDOW_HWND
 #endif

#include "ports.h"
#include "videoThread.h"

 static guintptr video_window_handle = 0;

 static GstBusSyncReply
 bus_sync_handler (GstBus * bus, GstMessage * message, gpointer user_data)
 {
  // ignore anything but 'prepare-window-handle' element messages
  if (!gst_is_video_overlay_prepare_window_handle_message (message))
    return GST_BUS_PASS;

  if (video_window_handle != 0) {
    GstVideoOverlay *overlay;

    // GST_MESSAGE_SRC (message) will be the video sink element
    overlay = GST_VIDEO_OVERLAY (GST_MESSAGE_SRC (message));
    gst_video_overlay_set_window_handle (overlay, video_window_handle);
  } else {
    g_warning ("Should have obtained video_window_handle by now!");
  }

  gst_message_unref (message);
  return GST_BUS_DROP;
 }

 static void
 video_widget_realize_cb (GtkWidget * widget, gpointer data)
 {
 #if GTK_CHECK_VERSION(2,18,0)
   // Tell Gtk+/Gdk to create a native window for this widget instead of
   // drawing onto the parent widget.
   // This is here just for pedagogical purposes, GDK_WINDOW_XID will call
   // it as well in newer Gtk versions
//   if (!gdk_window_ensure_native (widget->window))
//     g_error ("Couldn't create native window needed for GstVideoOverlay!");
 #endif

 #ifdef GDK_WINDOWING_X11
   {
     gulong xid = GDK_WINDOW_XID (gtk_widget_get_window (widget));
     video_window_handle = xid;
   }
 #endif
 #ifdef GDK_WINDOWING_WIN32
   {
     HWND wnd = GDK_WINDOW_HWND (gtk_widget_get_window (video_window));
     video_window_handle = (guintptr) wnd;
   }
 #endif
 }

 void start ( void)
 {
   GtkWidget *video_window;
   GtkWidget *app_window;
   GstBus *bus;
	GstElement *pipeline = NULL;

   app_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

   video_window = gtk_drawing_area_new ();
   g_signal_connect (video_window, "realize",
       G_CALLBACK (video_widget_realize_cb), NULL);
   gtk_widget_set_double_buffered (video_window, FALSE);

   // usually the video_window will not be directly embedded into the
   // application window like this, but there will be many other widgets
   // and the video window will be embedded in one of them instead
   gtk_container_add (GTK_CONTAINER (app_window), video_window);
   // show the GUI
   gtk_widget_show_all (app_window);

   // realize window now so that the video window gets created and we can
   // obtain its XID/HWND before the pipeline is started up and the videosink
   // asks for the XID/HWND of the window to render onto
   gtk_widget_realize (video_window);

   // we should have the XID/HWND now
   g_assert (video_window_handle != 0);

   pipeline = startReceiving (VIDEOPORT1,"");

   // set up sync handler for setting the xid once the pipeline is started
   bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));

   gst_bus_set_sync_handler (bus, (GstBusSyncHandler) bus_sync_handler, NULL,
       NULL);
   gst_object_unref (bus);
   gst_element_set_state (pipeline, GST_STATE_PLAYING);

   gtk_main ();


 }
