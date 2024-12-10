#include "moon_consts.h"

#include <errno.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define MAX_PATH_SIZE	1024
#define MAX_FILENAME_SIZE	256
#define FILE_BUF_SIZE	(MAX_PATH_SIZE + MAX_FILENAME_SIZE)
#define PACKET_SIZE	3

static int delta_days(struct tm *, int *);

typedef struct update_packet_struct
{
	char day[PACKET_SIZE];
	char mon[PACKET_SIZE];
	char next_full[PACKET_SIZE];
	char part_of_day;
	char weekday;
	char moon_phase;
} update_packet_t;

/* TODO Flashing of moon?:	*/
gboolean        moon_shine_flag = FALSE;
update_packet_t update;
static char     file_buf[FILE_BUF_SIZE];

char sprite_loc[MAX_PATH_SIZE];
char moon_loc[MAX_PATH_SIZE];

#define GET_SPRITE(X) snprintf(file_buf, FILE_BUF_SIZE, "%s/%s", sprite_loc, X)

void make_pixbuf_comp(GdkPixbuf * pbuf, GdkPixbuf * output)
{
	gdk_pixbuf_composite(pbuf, output, 0, 0, gdk_pixbuf_get_width(pbuf),
	                     gdk_pixbuf_get_height(pbuf), 0, 0, 1, 1, GDK_INTERP_NEAREST, 150);
}

/* perform the drawing */
gboolean drawingAreaDraw(__attribute__((unused)) GtkWidget * da,
                         cairo_t *                           cr,
                         __attribute__((unused)) gpointer    data)
{
	cairo_scale(cr, 0.77, 0.6);
	GET_SPRITE("p_invs_L.png");
	GdkPixbuf * trans_comp = gdk_pixbuf_new_from_file(file_buf, NULL);

	GdkPixbuf * opacity_changed = trans_comp;
	GET_SPRITE("strip.png");
	GdkPixbuf * orig_pixbuf = gdk_pixbuf_new_from_file(file_buf,NULL);
	
	// Set the pixbuf as the Cairo source
	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 695, 0);

	cairo_paint(cr);
	g_object_unref(orig_pixbuf); /* need to do this everytime otherwise memory leaks occur	*/
	g_object_unref(opacity_changed);

	/* part of day indicator (morning, afternoon, etc.)	*/
	int part_x = 340;
	int part_y = 61;
	
	switch(update.part_of_day)
	{
		case 5:
			part_x = 350;
			part_y = 69;
			break;
		case 4:
			part_x = 350;
			part_y = 69; 
			break;
		case 3:
			part_x = 420;
			part_y = 67;
			break;
		case 2:
			part_x = 520;
			part_y=66;
			break;
		case 1:
			part_x = 325;
			part_y = 66;
			break;
	}

	snprintf(file_buf, FILE_BUF_SIZE, "%s/day_%d.png", sprite_loc, update.part_of_day);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);
	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - part_x, part_y);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * 25 = def, 324
	 * first month digit
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/%c.png", sprite_loc, update.mon[0]);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);
	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 326, 30);

	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * second month digit (if 1 digit month, this is the only digit)
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/%c.png", sprite_loc, update.mon[1]);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);
	//orig_pixbuf    = gdk_pixbuf_add_alpha(orig_pixbuf, FALSE, 0, 0, 0);

	//	gdk_pixbuf_composite(orig_pixbuf, new, 0, 0, gdk_pixbuf_get_width(orig_pixbuf),
	//	                     gdk_pixbuf_get_height(orig_pixbuf), 0, 0, 1, 1, GDK_INTERP_NEAREST, 50);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 283, 30);

	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	// delta = 54 px
	/*
	 * slash separating the month & date
	 */
	GET_SPRITE("slash.png");
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 244, 30);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * first day digit (in the original game, if the day only had one digit,
	 * there would be a large space separating the slash and the digit, but
	 * here I think I'll stick a zero separating them (TODO), I always though it looked a little strange)
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/%c.png", sprite_loc, update.day[0]);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 208, 30);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * second day digit (if 1 digit day, this is the only digit)
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/%c.png", sprite_loc, update.day[1]);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 161, 30);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * day name (m, tu, w, thu, etc.)
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/week_day_%d.png", sprite_loc, update.weekday);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);
	
	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 108, 26);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * moon
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/moon_%d.png", sprite_loc, update.moon_phase);
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 100, 152);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	// TODO moon shining effect -- should be run every couple'a seconds or something
	// gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 120, 140);
	// cairo_paint(cr);

	/*
	 * next full moon
	 */
	GET_SPRITE("next_alt.png");
	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	int next_x;
	if (update.next_full[0] == 'p')
	{
		next_x = 330;
	}
	else
	{
		next_x = 380;
	}

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - next_x, 155);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	/*
	 * next full moon number digit 1
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/%c_invs.png", sprite_loc, update.next_full[0]);

	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 215, 167);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	//#8ccaea
	/*
	 * next full moon number digit 2
	 */
	snprintf(file_buf, FILE_BUF_SIZE, "%s/%c_invs.png", sprite_loc, update.next_full[1]);

	orig_pixbuf = gdk_pixbuf_new_from_file(file_buf, NULL);

	gdk_cairo_set_source_pixbuf(cr, orig_pixbuf, gtk_widget_get_allocated_width(da) - 167, 167);
	cairo_paint(cr);
	g_object_unref(orig_pixbuf);

	//cairo_destroy(cr);

	return FALSE;
}

/* Handle and ignore the window close button */
gboolean window_delete(__attribute__((unused)) GtkWidget * window)
{
	gtk_main_quit(); /* quit	*/
	return FALSE;
}

/* force the drawing area to update	*/
gboolean force_update_hud(gpointer data)
{
	time_t curr_time = time(NULL);
	char   helper_output[6];
	FILE * cmd_fp;
	struct tm * loc_time = localtime(&curr_time);
	/* EST, TODO do this the proper way*/
	if (loc_time->tm_hour > 50000)
	{
		loc_time->tm_mday -= 1;
		loc_time->tm_hour -= 5;
		if (loc_time->tm_wday == 0)
		{
			loc_time->tm_wday = 6;
		}
	}

	snprintf(file_buf, FILE_BUF_SIZE, "python3 %s/moon3.py", moon_loc);

	/* one argument for all args?	*/
	if ((cmd_fp = popen(file_buf, "r")) == NULL)
	{
		fprintf(stderr, "Couldn't open pipe\nError: %s", strerror(errno));
		exit(EXIT_FAILURE); /* treat as fatal, because if we can't open now, we won't be able in 5 mins	*/
	}

	helper_output[0] = fgetc(cmd_fp);
	if (helper_output[0] == EOF)
	{
		fprintf(stderr, "No output from moon helper!\n");
		exit(EXIT_FAILURE);	// fatal
	}

	helper_output[1] = fgetc(cmd_fp); /* moon file number can only be two digits	*/
	helper_output[2] = fgetc(cmd_fp);
	if (helper_output[2] != ',')
	{
		fprintf(stderr, "moon helper gave nonsense result (no comma): %s\n", helper_output);
		exit(EXIT_FAILURE);
	}
	/* read the moon phase digit first, parse it, and then do the next	*/
	helper_output[2] = '\0';

	int helper_moon_file_num = atoi(helper_output);

	if (helper_moon_file_num == 0 || helper_moon_file_num < 0 || helper_moon_file_num > 16)
	{
		fprintf(stderr, "moon helper gave nonsense result (NaN): %s\n", helper_output);
		exit(EXIT_FAILURE);
	}
	update.moon_phase = helper_moon_file_num;

	if (loc_time->tm_hour >= 22) /* note 24 hour time	*/	/* Late night	*/
	{
		update.part_of_day = 3;
	}
	else if (loc_time->tm_hour >= 17)	/* Evening 5:00 to 10:00	*/
	{
		update.part_of_day = 4;
	}
	else if (loc_time->tm_hour >= 11)	/* Daytime 11:00 to 5:00	*/
	{
		update.part_of_day = 5;
	}
	else if (loc_time->tm_hour >= 6)	/* morning	6:00 to 11:00 */
	{
		update.part_of_day = 1;
	}
	else if (loc_time->tm_hour >= 3)	/* Before Dawn 3:00 to 6:00	*/
	{
		update.part_of_day = 2;
	}
	else
	{
		update.part_of_day = 1;			/* catch-all	*/
	}

	update.weekday = loc_time->tm_wday;

	/* make all of these zero so that we can easily use atoi()	*/
	helper_output[0] = '0';
	helper_output[1] = '0';
	helper_output[2] = '0';

	/* parse the next full moon number	*/
	helper_output[3] = fgetc(cmd_fp);
	helper_output[4] = fgetc(cmd_fp);
	helper_output[5] = '\0';

	int helper_next_full_moon = atoi(helper_output);
	if (helper_next_full_moon == 0 || helper_next_full_moon < 0 || helper_next_full_moon > 32)
	{
		fprintf(stderr, "moon helper gave nonsense result (NaN): %s\n", helper_output);
		exit(EXIT_FAILURE);
	}

	int actual_next_full_moon = delta_days(loc_time, &helper_next_full_moon); /* add a thing for when full	*/

	printf("output of next full moon %d\n", helper_next_full_moon);
	printf("adjusted next full moon %d\n", actual_next_full_moon);

	if (loc_time->tm_mday < 10)
	{
		snprintf(update.day, PACKET_SIZE, "p%d", loc_time->tm_mday);
	}
	else
	{
		snprintf(update.day, PACKET_SIZE, "%d", loc_time->tm_mday);
	}

	if ((loc_time->tm_mon + 1) < 10)
	{
		snprintf(update.mon, PACKET_SIZE, "p%d", loc_time->tm_mon + 1);
	}
	else
	{
		snprintf(update.mon, PACKET_SIZE, "%d", loc_time->tm_mon + 1);
	}

	if (actual_next_full_moon < 10)
	{
		snprintf(update.next_full, PACKET_SIZE, "p%d", actual_next_full_moon);
	}
	else
	{
		snprintf(update.next_full, PACKET_SIZE, "%d", actual_next_full_moon);
	}

	printf("update-day : %s\n", update.mon);
	printf("DAY : %d\n", loc_time->tm_min);
	pclose(cmd_fp);

	if (data != NULL) { gtk_widget_queue_draw((GtkWidget *)data); }

	// static int run_count = 0;
	// if (run_count > 8000)
	// {
	// 	gtk_main_quit();
	// 	exit(1);
	// }
	// run_count++;

	return TRUE;
}

// TODO use proper library calls for this instead of magic numbers
static int delta_days(struct tm * loc_time, int * next)
{
	int days_in_month;
	int mon = loc_time->tm_mon + 1;

	if (loc_time->tm_mday > *next)
	{
		if (mon == 1 || mon == 3 || mon == 5
		    || mon == 7 || mon == 8 || mon == 10
		    || mon == 12)
		{
			days_in_month = 31;
		}

		else if (mon == 4 || mon == 6
		         || mon == 9 || mon == 11)
		{
			days_in_month = 30;
		}

		else if (mon == 2)
		{
			/* check for leap year	*/
			if ((loc_time->tm_year % 4 == 0 && loc_time->tm_year % 100 != 0) || loc_time->tm_year % 400 == 0)
				days_in_month = 28;
			else
				days_in_month = 29;
		}
		printf("days in month = %d\n", days_in_month);
		return (days_in_month - loc_time->tm_mday) + *next;
	}

	return (*next - loc_time->tm_mday);
}

int main(int argc, char ** argv)
{
	GtkWidget * window;
	GtkWidget * drawing_area;

	GError * error = NULL;
	int      width, height;

	if (argc != 3) 
	{
		fprintf(stderr, "Usage: %s <moon.py location> <sprites folder location>\n", argv[0]);
		exit(EXIT_FAILURE);	
	}

	memcpy(moon_loc, argv[1], strlen(argv[1]));
	memcpy(sprite_loc, argv[2], strlen(argv[2]));

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/* turn transparent (need a compositor for this	*/
	gtk_widget_set_app_paintable(window, TRUE);
	GdkScreen * screen = gtk_widget_get_screen(window);
	GdkVisual * visual = gdk_screen_get_rgba_visual(screen);
	gtk_widget_set_visual(window, visual);

	/* allow the window to be closed normally (Though it doesn't matter since you can't click on the window	*/
	g_signal_connect(window, "delete-event", (GCallback)window_delete, NULL);

	/* Use a drawing area to dispay the images. */
	drawing_area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), drawing_area);
	g_signal_connect(drawing_area, "draw", (GCallback)drawingAreaDraw, NULL);

	gtk_widget_add_events(drawing_area, gtk_widget_get_events(drawing_area) | GDK_BUTTON_PRESS_MASK);

	gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_SOUTH_EAST);
	gtk_window_move(GTK_WINDOW(window), gdk_screen_width()  - 550, 0);
	
	// to move monitors, simply subtract by horiz. resolution of the laptop monitor (1920) and however many other monitors 'in front' of the one you want
	gtk_widget_show_all(window);

	force_update_hud(NULL);

	/* update every 5 mins	*/
	g_timeout_add(500000, force_update_hud, (gpointer)drawing_area);

	/* all this to prevent window manager from interferering and clicks from not registering	*/
	GdkWindow * gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

	gdk_window_hide(GDK_WINDOW(gdk_window)); /* hide the window to set a bunch of flags	*/

	/* set all the flags in the world	*/
	gdk_window_set_override_redirect(GDK_WINDOW(gdk_window), TRUE);
	gdk_window_set_override_redirect(GDK_WINDOW(gdk_window), TRUE);
	gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
	gtk_window_set_accept_focus(GTK_WINDOW(window), TRUE);
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 300);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	/* prevent the window from registering clicks and input	*/
	cairo_rectangle_int_t attached_inspector_input_rect = {0, 0, 0, 0};
	cairo_region_t *      shape                         = cairo_region_create_rectangle(&attached_inspector_input_rect);
	gdk_window_input_shape_combine_region(GDK_WINDOW(gdk_window), shape, 0, 0);

	/* now show the untouchable window	*/
	gdk_window_show(GDK_WINDOW(gdk_window));
	gtk_main();

	return EXIT_SUCCESS;
}
