#include <libadwaita-1/adwaita.h>
#include <string.h>
#include <stdbool.h>
//local includes
#include <ui/main_window.h>
#include <utils.h>
#include <db/db.h>
#include <db/music_dir.h>
#include <db/music.h>
#include <ui/dir_empty.h>
#include <ui/home.h>
#include <sound.h>

#define MUSIC_LIST_SIZE 1024

static GObject* window;
static GObject* main_view;
static GObject* lbl_title;
static GObject* lbl_artist;
static GObject* lbl_duration;
static GObject* progress_bar;
static GObject* playpause;
static GObject* playpause_image;
static GObject* previous;
static GObject* next;

static music_t music_list[MUSIC_LIST_SIZE];
static GtkWidget* btn_list[MUSIC_LIST_SIZE];
static int list_size = 0;
static int cur_index = 0;


static void playpause_callback();
static void previous_callback();
static void next_callback();

void main_window_activate(AdwApplication* app){
	GtkBuilder* builder = load_ui("/ui/main");
	window = get_object(builder, "window");
	main_view = get_object(builder, "main_view");
	lbl_title = get_object(builder, "title");
	lbl_artist = get_object(builder, "artist");
	lbl_duration = get_object(builder, "duration");
	progress_bar = get_object(builder, "progress_bar");
	playpause = get_object(builder, "playpause");
	playpause_image = get_object(builder, "playpause_image");
	previous = get_object(builder, "previous");
	next = get_object(builder, "next");

	g_signal_connect(GTK_BUTTON(playpause), "clicked", G_CALLBACK(playpause_callback), NULL);
	g_signal_connect(GTK_BUTTON(previous), "clicked", G_CALLBACK(previous_callback), NULL);
	g_signal_connect(GTK_BUTTON(next), "clicked", G_CALLBACK(next_callback), NULL);

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	load_css(GTK_WIDGET(window), "/css/global-style");
	gtk_widget_set_visible(GTK_WIDGET(window), true);


	if(db_table_count(MUSIC_DIR) > 0){
		home_init(window);
	}else{
		dir_empty_init(window);
	}
}

void set_main_view(GObject* widget){
	adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(main_view), GTK_WIDGET(widget));
}

void set_playing_title(const char* title, const char* path){
	gtk_image_set_from_resource(GTK_IMAGE(playpause_image), "/icon/pause");
	if(strlen(title) > 0){
		gtk_label_set_label(GTK_LABEL(lbl_title), title);
	}else{
		gtk_label_set_label(GTK_LABEL(lbl_title), path);
	}
}

void set_playing_artist(const char* artist){
	if(strlen(artist) > 0){
		gtk_label_set_label(GTK_LABEL(lbl_artist), artist);
	}else{
		gtk_label_set_label(GTK_LABEL(lbl_artist), "Unknown artist");
	}
}

void set_playing_duration(const double duration){
	char buf[6];
	parse_time(buf, duration);
	gtk_label_set_label(GTK_LABEL(lbl_duration), buf);
	gtk_range_set_range(GTK_RANGE(progress_bar), 0, duration);
}

void add_to_music_list(music_t music, GtkWidget* btn){
	if(list_size < MUSIC_LIST_SIZE - 1){
		music_list[list_size] = music;
		btn_list[list_size] = btn;
		list_size++;
	}
}

music_t get_music_list(int index){
	return music_list[index];	
}

void set_current_index(int index){
	cur_index = index;
}

static void playpause_callback(){
	if(sound_is_playing() == true){
		sound_pause();
		gtk_image_set_from_resource(GTK_IMAGE(playpause_image), "/icon/play");
	}else{
		gtk_image_set_from_resource(GTK_IMAGE(playpause_image), "/icon/pause");
		sound_resume();
	}
}

static void previous_callback(){
	if(cur_index > 0){
		gtk_widget_remove_css_class(btn_list[cur_index], "suggested-action");
		cur_index--;
		gtk_widget_add_css_class(btn_list[cur_index], "suggested-action");
		sound_set((const char*)music_list[cur_index].path);
		sound_play();
		set_playing_duration(sound_get_duration());
	}
}

static void next_callback(){
	if(cur_index < MUSIC_LIST_SIZE - 1){
		gtk_widget_remove_css_class(btn_list[cur_index], "suggested-action");
		cur_index++;
		gtk_widget_add_css_class(btn_list[cur_index], "suggested-action");
		sound_set((const char*)music_list[cur_index].path);
		sound_play();
		set_playing_duration(sound_get_duration());
	}
}
