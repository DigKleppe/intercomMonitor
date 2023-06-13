/**
 * GNOME/GTK+ Menu Demo Application using a GtkApplication class
 *
 * M. Horauer
 */
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>

typedef struct {
  GtkWidget *greeterlabel;
  GtkWidget *namelabel;
  GtkWidget *nameentry;
  GtkWidget *enterbutton;
  GtkWidget *clearbutton;
} appWidgets;

/***************************************************************** PROTOTYPES */
void enter_callback(GSimpleAction *action, GVariant *parameter, gpointer data);
void clear_callback(GSimpleAction *action, GVariant *parameter, gpointer data);
void entry_callback(GtkWidget *widget, gpointer data);

/****************************************************************** CALLBACKS */
void enter_callback(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  const gchar *name;
  gchar str[50];
  appWidgets *wid = (appWidgets*)data;

  name = gtk_entry_get_text(GTK_ENTRY(wid->nameentry));
  g_sprintf(str,"Hello %s!",name);
  gtk_widget_override_font(wid->greeterlabel,
                           pango_font_description_from_string("Tahoma 20"));
  gtk_label_set_text(GTK_LABEL(wid->greeterlabel),(const gchar *)str);

  name = NULL;
  wid = NULL;
}

void clear_callback(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  appWidgets *wid = (appWidgets*)data;

  gtk_label_set_text(GTK_LABEL(wid->greeterlabel),"Greeter Demo");
  gtk_entry_set_text(GTK_ENTRY(wid->nameentry),"");

  wid = NULL;
}

void entry_callback(GtkWidget *widget, gpointer data)
{
  enter_callback(NULL,NULL,data);
}
/***************************************************************** GUI THREAD */
static void
activate(GtkApplication *app, gpointer data)
{
  GtkWidget *window;
  GtkWidget *grid;
  appWidgets *wid = (appWidgets*)data;
  // map menu actions to callbacks
  const GActionEntry app_actions[] = {
    { "enter", enter_callback, NULL, NULL, NULL },
    { "clear", clear_callback, NULL, NULL, NULL }
  };

  // create a window with title, default size and icons
  window = gtk_application_window_new(app);
  gtk_window_set_application(GTK_WINDOW (window), GTK_APPLICATION (app));
  gtk_window_set_title(GTK_WINDOW(window),"GNOME Greeter Demo");
  gtk_window_set_default_icon_from_file("icon.png", NULL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
  grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window),grid);

  // create label and entry
  wid->greeterlabel = gtk_label_new_with_mnemonic("Greeter Demo");
  gtk_widget_override_font(wid->greeterlabel,
                           pango_font_description_from_string("Tahoma 20"));
  gtk_widget_set_size_request(wid->greeterlabel,260,40);
  gtk_grid_attach(GTK_GRID(grid),wid->greeterlabel,0,0,2,1);
  wid->namelabel = gtk_label_new_with_mnemonic("Name:");
  gtk_widget_set_size_request(wid->namelabel,130,40);
  gtk_grid_attach(GTK_GRID(grid),wid->namelabel,0,1,1,1);
  wid->nameentry = gtk_entry_new();
  gtk_widget_set_size_request(wid->nameentry,130,35);
  gtk_entry_set_placeholder_text(GTK_ENTRY(wid->nameentry),"Muster");
  gtk_grid_attach(GTK_GRID(grid),wid->nameentry,1,1,1,1);
  g_signal_connect (G_OBJECT(wid->nameentry), "activate",
                      G_CALLBACK (entry_callback), (gpointer)wid);

  // create Clear and Enter buttons
  wid->enterbutton = gtk_button_new_with_label("Enter");
  gtk_grid_attach(GTK_GRID(grid),wid->enterbutton,1,2,1,1);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(wid->enterbutton),"app.enter");
  wid->clearbutton = gtk_button_new_with_label("Clear");
  gtk_grid_attach(GTK_GRID(grid),wid->clearbutton,0,2,1,1);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(wid->clearbutton),"app.clear");

  // connect actions with callbacks
  g_action_map_add_action_entries(G_ACTION_MAP (app), app_actions,
                                  G_N_ELEMENTS (app_actions), wid);

  gtk_widget_show_all(GTK_WIDGET(window));
}

/**************************************************************** MAIN THREAD */
int
screenMain (int argc, char **argv)
{
  GtkApplication *app;
  int status;
  appWidgets *wid = g_malloc(sizeof(appWidgets));

  app = gtk_application_new("org.gtk.example",G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), (gpointer)wid);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  g_free(wid);
  wid = NULL;
  return status;
}
/** EOF */
