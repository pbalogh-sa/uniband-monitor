/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) 2013 <p.balogh.sa@gmail.com>
 * 
 * uniband-monitor is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * uniband-monitor is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib/gi18n.h>

#include "main.h"

typedef struct _Private Private;
static struct _Private
{
	/* ANJUTA: Widgets declaration for uniband_monitor.ui - DO NOT REMOVE */
	GtkWidget *statusbar;
	GtkWidget *not_wifi; 	
} widgets;

static struct Private* priv = NULL;

/* For testing propose use the local (not installed) ui file */
#define UI_FILE PACKAGE_DATA_DIR"/ui/uniband_monitor.ui"
//#define UI_FILE "src/uniband_monitor.ui"
#define TOP_WINDOW "window"

/* Signal handlers */
/* Note: These may not be declared static because signal autoconnection
 * only works with non-static methods
 */

/* Called when the window is closed */
void
destroy (GtkWidget *widget, gpointer data)
{
	gtk_main_quit ();
}

static GtkWidget*
create_window (void)
{
	GtkWidget *window;
	guint cont;
	char remain_chan[32];

	GError* error = NULL;

	/* Load UI from file */
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
	{
		g_critical ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals (builder, NULL);

	/* Get the window object from the ui file */
	window = GTK_WIDGET (gtk_builder_get_object (builder, TOP_WINDOW));
        if (!window)
        {
                g_critical ("Widget \"%s\" is missing in file %s.",
				TOP_WINDOW,
				UI_FILE);
        }
	priv = g_malloc (sizeof (struct _Private));
	/* ANJUTA: Widgets initialization for uniband_monitor.ui - DO NOT REMOVE */
	widgets.statusbar = GTK_WIDGET(gtk_builder_get_object (builder, "statusbar_bottom"));
	cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(widgets.statusbar), "context");
	sprintf(remain_chan, "Remaining monitor interfaces: %d", wifi_numbers - count_channels());
	gtk_statusbar_push (GTK_STATUSBAR(widgets.statusbar), cont, remain_chan);	
	return window;
}

int
main (int argc, char *argv[])
{
 	GtkWidget *window;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	scan_enabled = 0;
	read_options ();
	open_pidfile ();
	wifi_numbers = get_ints ();
	
	gtk_init (&argc, &argv);
	window = create_window ();
/*	if (wifi_numbers < 1) {
		widgets.not_wifi = GTK_WIDGET (gtk_builder_get_object (builder, "messagedialog"));
		gtk_widget_show (widgets.not_wifi);
		gtk_widget_set_sensitive (GTK_WIDGET(window), FALSE);
	} */
	gtk_widget_show (window);
	gtk_main ();

	g_object_unref (builder);

	g_free (priv);
	close_pidfile ();
	
	return 0;
}
