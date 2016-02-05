#include <config.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib/gi18n.h>

#include "main.h"

static int channel_5g[24] = {36,40,44,48,52,56,60,64,100,104,108,112,116, \
							120,124,128,132,136,140,149,153,157,161,165};

void
create_horst_dialog (void)
{
	GtkWidget *main_window, *chooser, *chooser_box, *buttons, *selector, *starth;
	char chan_num[4], check_labels[32];
	int i, j;

	main_window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(main_window), FALSE);
	
	chooser = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(chooser), "destroy",
					 G_CALLBACK(del_horst_dialog), NULL);
	gtk_window_set_title (GTK_WINDOW(chooser), (" signal details "));
	gtk_container_set_border_width(GTK_CONTAINER(chooser), 0);	

	chooser_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(chooser), chooser_box);
	gtk_container_set_border_width (GTK_CONTAINER(chooser_box), 0);

	buttons = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	j = 0;
	for (i = 0; i < 24; i++) {
		if (chan5[i]) {
			horst_chans[j++] = channel_5g[i];
		}
	}
	for (i = 1; i < 15; i++) {
		if (chan24[i-1]) {
			horst_chans[j++] = i;
		}
	}
	
	for (i = 0; i < j; i++) {
		GtkWidget *sel[i];
		sprintf(chan_num, "%d", i);
		sprintf(check_labels, "channel %d details", horst_chans[i]);
		if (i == 0) {
			selector = gtk_radio_button_new_with_label(NULL, check_labels);
			g_signal_connect(G_OBJECT(selector), "toggled",
						 	G_CALLBACK(select_details), strdup(chan_num));
			gtk_box_pack_start(GTK_BOX(buttons), selector, TRUE, TRUE, 0);
			chan_detail = i;
		} else {
			sel[i] = gtk_radio_button_new_with_label(gtk_radio_button_get_group (GTK_RADIO_BUTTON(selector)), check_labels);
			g_signal_connect(G_OBJECT(sel[i]), "toggled",
						 	G_CALLBACK(select_details), strdup(chan_num));
			gtk_box_pack_start(GTK_BOX(buttons), sel[i], TRUE, TRUE, 0);	
		}
#ifdef DEBUG
		printf("-----------------------------------%d %s\n", horst_chans[i], wificards[i]);
#endif
	}
	gtk_container_add (GTK_CONTAINER(chooser_box), buttons);
	gtk_container_set_border_width(GTK_CONTAINER(buttons), 5);	
	
	starth = gtk_button_new_with_label ("Show details");
	g_signal_connect(G_OBJECT(starth), "clicked",
					G_CALLBACK(start_horst_win), chooser);
	gtk_box_pack_start(GTK_BOX(chooser_box), starth, TRUE, TRUE, 0);
	gtk_widget_show_all(GTK_WIDGET(chooser));
}

GtkWidget*
create_horst_window(void)
{
	GtkWidget *horst_window, *horst_term;
	char *envs[2];
	char *params[] = {horstpath};
	envs[0] = strdup(environments);
	envs[1] = NULL;
	VtePtyFlags pty_flags = 0;
	GSpawnFlags spawn_flags = 0;
	char horst_title[24];
	
	params[0] = strdup(horstpath);
	params[1] = strdup("-c");
	params[2] = strdup("192.168.1.125");
	params[3] = NULL;
/*
	params[0] = strdup(horstpath);
	params[1] = strdup("-i");
	params[2] = strdup(wificards[chan_detail]);
	params[3] = NULL;
*/	
	horst_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	sprintf(horst_title, "channel %d signal details", horst_chans[chan_detail]);
	gtk_window_set_title (GTK_WINDOW(horst_window), strdup(horst_title));
	gtk_container_set_border_width(GTK_CONTAINER(horst_window), 0);

	horst_term = vte_terminal_new();
	gtk_container_add(GTK_CONTAINER(horst_window), horst_term);
	vte_terminal_set_size(VTE_TERMINAL(horst_term), 100, 50);
	gtk_widget_show(horst_term);
	gtk_widget_set_visible (GTK_WIDGET(horst_term), TRUE);
	vte_terminal_fork_command_full (VTE_TERMINAL(horst_term), pty_flags, kislogdir , params, envs, spawn_flags, NULL, NULL, NULL, NULL);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(horst_term), 1024);
	gtk_widget_grab_focus(horst_term);
	g_signal_connect(G_OBJECT(horst_window), "destroy",
					G_CALLBACK(horst_delete), horst_window);

	return horst_window;
}