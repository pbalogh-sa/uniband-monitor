#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "main.h"

static int channel_5g[24] = {36,40,44,48,52,56,60,64,100,104,108,112,116, \
							120,124,128,132,136,140,149,153,157,161,165};
gboolean activate = TRUE;

void
horst_delete (GtkWidget *self, gpointer horst_window)
{
	reconf_cards = FALSE;
#ifdef DEBUG
	printf("Horst window destroyed ------------ %d\n", reconf_cards);
#endif
}

void
start_horst_win (GtkButton *self, gpointer chooser)
{
#ifdef DEBUG
	printf("----------------- CHAN DETAIL -------------> %d\n", chan_detail);
#endif
	horst_window = create_horst_window ();
	gtk_widget_destroy (GTK_WIDGET(chooser));
	gtk_widget_show_all (GTK_WIDGET(horst_window));

}

void
del_horst_dialog (void)
{
	GtkWidget *main_window;
	
	main_window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(main_window), TRUE);
}

void
select_details (GtkRadioButton *self, gpointer chan_num)
{
	chan_detail = atoi(chan_num);
#ifdef DEBUG
	printf("Select card %d for detailed view!\n", chan_detail);
#endif
}

void
signal_details_clicked_cb (GtkButton *self)
{
	GtkWidget *window, *channel_warn, *hors_warn;
	
	if (count_channels ()) {
		if (!(kismet_active || record_active)) {
			reconf_cards = TRUE;
			hors_warn = GTK_WIDGET(gtk_builder_get_object (builder, "hors_warn"));
			gtk_widget_show_all (GTK_WIDGET(hors_warn));
			window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
			gtk_widget_set_sensitive (GTK_WIDGET(window), FALSE);
#ifdef DEBUG
			printf("Now reconf -------------- %d\n", reconf_cards);
#endif
		} else {
			create_horst_dialog ();
		}
	} else {
		channel_warn = GTK_WIDGET(gtk_builder_get_object (builder, "channel_warn"));
		gtk_widget_show_all (GTK_WIDGET(channel_warn));
		window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
		gtk_widget_set_sensitive (GTK_WIDGET(window), FALSE);
	}
}

void
scanenable_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		scan_enabled = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		if (!record_active) {
			cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
			sprintf(remain_chan, "Remaining monitor interfaces: %d", wifi_numbers - count_channels() - scan_enabled);
			gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);
		}
	} else {
		scan_enabled = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		if (!record_active) {
			cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
			gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
		}
	}
}

void
start_record_toggled_cb (GtkToggleButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	int i;
	GtkWidget *bandselect, *channel_warn, *window;
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		gtk_button_set_label(GTK_BUTTON(self), " Stop record ");
#ifdef DEBUG
		print_channels ();
#endif
		if (count_channels ()) {
			record_active = TRUE;
			if (!kismet_active) {
				suid_card_thread ();
			}
			bandselect = GTK_WIDGET(gtk_builder_get_object (builder, "bandselect"));
			gtk_widget_set_sensitive (GTK_WIDGET(bandselect), FALSE);			
			cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
			sprintf(remain_chan, "Monitoring started with %d cards", start_record_threads ());
			gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);
		} else {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
			channel_warn = GTK_WIDGET(gtk_builder_get_object (builder, "channel_warn"));
			gtk_widget_show_all (GTK_WIDGET(channel_warn));
			window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
			gtk_widget_set_sensitive (GTK_WIDGET(window), FALSE);
		}
#ifdef DEBUG
		printf("COUNT -------> %d PID ------------> %d\n", dumper.j, dumper.pid[0]);
#endif
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		gtk_button_set_label(GTK_BUTTON(self), " Start record ");
		record_active = FALSE;
		for (i = 0; i < dumper.j; i++) {
			kill(dumper.pid[i], SIGTERM);
		}
		if (!kismet_active) {
			bandselect = GTK_WIDGET(gtk_builder_get_object (builder, "bandselect"));
			gtk_widget_set_sensitive (GTK_WIDGET(bandselect), TRUE);
		}
		if (count_channels () > 0) {
			cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
			gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
		}
	}
}

void
hors_warn_destroy_cb (GtkWidget *self)
{
	GtkWidget *window;
	
	gtk_widget_hide (self);
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(window), TRUE);
}

void
not_monit_clicked_cb (GtkButton *self, gpointer hors_warn)
{
	GtkWidget *window;
	
#ifdef DEBUG
	fprintf (stderr, "Cannot find any configured wireless devices for kismet!\n");
#endif
	gtk_widget_hide (GTK_WIDGET(hors_warn));
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(window), TRUE);
}

void
channel_warn_destroy_cb (GtkWidget *self)
{
	GtkWidget *window;
	
	gtk_widget_hide (self);
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(window), TRUE);
}

void
not_chan_clicked_cb (GtkButton *self, gpointer channel_warn)
{
	GtkWidget *window;
	
#ifdef DEBUG
	fprintf (stderr, "Cannot find any configured wireless devices for kismet!\n");
#endif
	gtk_widget_hide (GTK_WIDGET(channel_warn));
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(window), TRUE);
}

void
kismet_warn_destroy_cb (GtkWidget *self)
{
	GtkWidget *window;
	
	gtk_widget_hide (self);
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(window), TRUE);
}

void
not_kismet_clicked_cb (GtkButton *self, gpointer kismet_warn)
{
	GtkWidget *window;
	
#ifdef DEBUG
	fprintf (stderr, "Cannot find any configured wireless devices for kismet!\n");
#endif
	gtk_widget_hide (GTK_WIDGET(kismet_warn));
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	gtk_widget_set_sensitive (GTK_WIDGET(window), TRUE);
}

void
not_wifi_clicked_cb (GtkButton *self)
{
#ifdef DEBUG
	fprintf (stderr, "Cannot find any wireless devices!\n");
#endif
	exit(EXIT_SUCCESS);
}

void
start_kismet_toggled_cb (GtkToggleButton *self, gpointer kismet_term)
{
	GtkWidget *window, *kismet_warn, *bandselect, *scanenable;
	char *envs[2];
	char *params[] = {kisclipath};
	envs[0] = strdup(environments);
	envs[1] = NULL;
	VtePtyFlags pty_flags = 0;
	GSpawnFlags spawn_flags = 0;
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		gtk_button_set_label(GTK_BUTTON(self), " Hide networks ");
		scanenable = GTK_WIDGET(gtk_builder_get_object (builder, "scanenable"));
		gtk_widget_set_sensitive (GTK_WIDGET(scanenable), FALSE);
		if (scan_enabled || count_channels ()) {
			if (!scan_enabled) {
				bandselect = GTK_WIDGET(gtk_builder_get_object (builder, "bandselect"));
				gtk_widget_set_sensitive (GTK_WIDGET(bandselect), FALSE);
				kismet_active = TRUE;
				if (!record_active) {
					suid_card_thread ();
				}
			} else if (!suid_runned) {
				system("set_devices.sh");
			} 
			if (kismet_server_pid == 0) {
				start_kismet_server ();
			}			
			sleep(1);
			gtk_widget_set_visible (GTK_WIDGET(kismet_term), TRUE);
			vte_terminal_fork_command_full (kismet_term, pty_flags, kislogdir , params, envs, spawn_flags, NULL, NULL, NULL, NULL);
			gtk_widget_grab_focus(kismet_term);
		} else {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
			kismet_warn = GTK_WIDGET(gtk_builder_get_object (builder, "kismet_warn"));
			gtk_widget_show_all (GTK_WIDGET(kismet_warn));
			window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
			gtk_widget_set_sensitive (GTK_WIDGET(window), FALSE);
		}
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		gtk_button_set_label(GTK_BUTTON(self), " Show networks ");
		gtk_widget_set_visible (GTK_WIDGET(kismet_term), FALSE);
		window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
		gtk_window_resize(GTK_WINDOW(window), 1, 1);
		if (kismet_server_pid) {
			kill(kismet_server_pid, SIGTERM);
			kismet_server_pid = 0;
		}
		kismet_active = FALSE;
		if ((wifi_numbers - count_channels ()) > 0) {
			scanenable = GTK_WIDGET(gtk_builder_get_object (builder, "scanenable"));
			gtk_widget_set_sensitive (GTK_WIDGET(scanenable), TRUE);
		}
		if (!record_active) {
			bandselect = GTK_WIDGET(gtk_builder_get_object (builder, "bandselect"));
			gtk_widget_set_sensitive (GTK_WIDGET(bandselect), TRUE);
		}
	}	
}

void 
checkbutton_unsensitive (void)
{
	GtkWidget *checkboxes, *scanenable;
	int i;
	char check_boxes[8];
	
	for (i = 0; i < 24; i++) {
		sprintf(check_boxes, "chan%d", channel_5g[i]);
		checkboxes = GTK_WIDGET(gtk_builder_get_object (builder, check_boxes));
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkboxes)) == FALSE) {
			gtk_widget_set_sensitive (GTK_WIDGET(checkboxes), FALSE);
		}
	}
	for (i = 1; i < 15; i++) {
		sprintf(check_boxes, "chan%d", i);
		checkboxes = GTK_WIDGET(gtk_builder_get_object (builder, check_boxes));
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkboxes)) == FALSE) {
			gtk_widget_set_sensitive (GTK_WIDGET(checkboxes), FALSE);
		}
	}
	scanenable = GTK_WIDGET(gtk_builder_get_object (builder, "scanenable"));
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(scanenable)) == FALSE) {
		gtk_widget_set_sensitive (GTK_WIDGET(scanenable), FALSE);
	}
}

void
checkbutton_sensitive (void)
{
	GtkWidget *checkboxes, *scanenable;
	int i;
	char check_boxes[8];
	
	for (i = 0; i < 24; i++) {
		sprintf(check_boxes, "chan%d", channel_5g[i]);	
		checkboxes = GTK_WIDGET(gtk_builder_get_object (builder, check_boxes));
		gtk_widget_set_sensitive (GTK_WIDGET(checkboxes), TRUE);
	}
	for (i = 1; i < 15; i++) {
		sprintf(check_boxes, "chan%d", i);
		checkboxes = GTK_WIDGET(gtk_builder_get_object (builder, check_boxes));
		gtk_widget_set_sensitive (GTK_WIDGET(checkboxes), TRUE);
	}
	scanenable = GTK_WIDGET(gtk_builder_get_object (builder, "scanenable"));
	if (!kismet_active) gtk_widget_set_sensitive (GTK_WIDGET(scanenable), TRUE);
}

void
statusbar_bottom_text_pushed_cb (GtkStatusbar *self)
{
	if ( wifi_numbers - count_channels () - scan_enabled < 1) {
		activate = FALSE;
		checkbutton_unsensitive ();
	}
#ifdef DEBUG	
	printf("Statusbar text pushed ----> %d %d\n", wifi_numbers - count_channels () - scan_enabled, activate);
#endif
}

void
statusbar_bottom_text_popped_cb (GtkStatusbar *self)
{
	if ( wifi_numbers - count_channels () - scan_enabled > 0) {
		activate = TRUE;
		checkbutton_sensitive ();
	}
#ifdef DEBUG	
	printf("Statusbar text popped ----> %d %d\n", wifi_numbers - count_channels () - scan_enabled, activate);
#endif 
}

void
bandselect_switch_page_cb (GtkNotebook *self)
{
	if (gtk_notebook_get_current_page (GTK_NOTEBOOK(self))) {
		printf("5GHz page activated\n");
	} else {
		printf("2.4GHz page activated\n");
	}
}

void
chan36_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[0] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d", wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[0] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan40_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[1] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[1] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan44_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[2] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[2] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan48_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[3] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[3] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan52_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[4] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[4] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan56_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[5] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[5] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan60_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[6] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[6] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan64_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[7] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[7] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan100_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[8] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[8] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan104_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[9] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[9] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan108_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[10] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[10] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan112_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[11] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[11] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan116_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[12] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[12] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan120_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[13] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[13] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan124_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[14] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[14] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan128_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[15] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[15] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan132_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[16] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[16] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan136_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[17] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[17] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan140_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[18] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[18] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan149_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[19] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[19] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan153_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[20] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[20] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan157_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[21] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[21] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan161_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[22] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[22] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan165_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan5[23] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan5[23] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

/************************ 2.4 GHz ****************************/

void
chan1_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[0] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[0] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan2_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[1] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[1] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan3_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[2] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[2] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan4_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[3] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[3] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan5_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[4] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[4] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan6_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[5] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[5] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan7_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[6] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[6] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan8_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[7] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[7] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan9_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[8] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[8] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan10_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[9] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[9] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan11_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[10] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[10] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan12_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[11] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[11] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan13_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[12] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[12] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}

void
chan14_toggled_cb (GtkCheckButton *self, gpointer statusbar_bottom)
{
	guint cont;
	char remain_chan[32];
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(self))) {
		chan24[13] = 1;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), TRUE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		sprintf(remain_chan, "Remaining monitor interfaces: %d",  wifi_numbers - count_channels() - scan_enabled);
		gtk_statusbar_push (GTK_STATUSBAR(statusbar_bottom), cont, remain_chan);	
	} else {
		chan24[13] = 0;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(self), FALSE);
		cont = gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar_bottom), "context");
		gtk_statusbar_pop (GTK_STATUSBAR(statusbar_bottom), cont);
	}
}
