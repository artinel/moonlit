#include <libadwaita-1/adwaita.h>
#include <string.h>
#include <stdbool.h>
//local includes
#include <ui/main_window.h>
#include <utils.h>
#include <db/db.h>
#include <db/music_dir.h>
#include <db/music.h>
#include <db/like.h>
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
static GObject* position;
static GObject* repeat;
static GObject* like;

static music_t music_list[MUSIC_LIST_SIZE];
static GtkWidget* btn_list[MUSIC_LIST_SIZE];
static int list_size = 0;
static int cur_index = 0;
static bool is_repeat = false;

static pthread_t prog_thread;
static double prog_position;

static void playpause_callback();
static void previous_callback();
static void next_callback();
static void* progress_thread(void* arg);
static gboolean progress_update();
static void progress_start();
static void progress_stop();
static void progress_bar_callback();
static void repeat_callback();
static void like_callback();

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
	position = get_object(builder, "position");
	repeat = get_object(builder, "repeat");
	like = get_object(builder, "like");

	g_signal_connect(GTK_BUTTON(playpause), "clicked", G_CALLBACK(playpause_callback), NULL);
	g_signal_connect(GTK_BUTTON(previous), "clicked", G_CALLBACK(previous_callback), NULL);
	g_signal_connect(GTK_BUTTON(next), "clicked", G_CALLBACK(next_callback), NULL);
	g_signal_connect(GTK_SCALE(progress_bar), "change-value", G_CALLBACK(progress_bar_callback), NULL);
	g_signal_connect(GTK_BUTTON(repeat), "clicked", G_CALLBACK(repeat_callback), NULL);
	g_signal_connect(GTK_BUTTON(like), "clicked", G_CALLBACK(like_callback), NULL);

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
	progress_stop();
	gtk_image_set_from_resource(GTK_IMAGE(playpause_image), "/icon/pause");
	if(strlen(title) > 0){
		gtk_label_set_label(GTK_LABEL(lbl_title), title);
	}else{
		gtk_label_set_label(GTK_LABEL(lbl_title), path);
	}

	progress_start();
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

void set_playing_like(bool is_like){
	if(is_like == true){
		gtk_widget_add_css_class(GTK_WIDGET(like), "destructive-action");
	}else{
		gtk_widget_remove_css_class(GTK_WIDGET(like), "destructive-action");
	}
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

GtkWidget* get_btn_list(int index){
	return btn_list[index];
}

void set_current_index(int index){
	cur_index = index;
}

int get_current_index(){
	return cur_index;
}

static void playpause_callback(){
	if(sound_is_playing() == true){
		gtk_image_set_from_resource(GTK_IMAGE(playpause_image), "/icon/play");
		sound_pause();
		progress_stop();
	}else{
		gtk_image_set_from_resource(GTK_IMAGE(playpause_image), "/icon/pause");
		sound_resume();
		progress_start();
	}
}

static void previous_callback(){
	if(cur_index > 0){
		playsong(NULL, GINT_TO_POINTER(cur_index - 1));
	}
}

static void next_callback(){
	if(cur_index < MUSIC_LIST_SIZE - 1){
		playsong(NULL, GINT_TO_POINTER(cur_index + 1));
	}
}


static void* progress_thread(){
	while(sound_is_playing() == true){
		prog_position = sound_get_position();
		progress_update();
		sleep(1);
	}
	return NULL;
}

static void progress_start(){
	sound_set_is_playing(true);
	pthread_create(&prog_thread, NULL, progress_thread, NULL);
	g_idle_add(progress_update, NULL);
}

static void progress_stop(){
	if(prog_thread != 0){
		sound_set_is_playing(false);
		pthread_join(prog_thread, NULL);
	}
}

static gboolean progress_update(){
	char buf[6];
	parse_time(buf, prog_position);
	gtk_label_set_label(GTK_LABEL(position), buf);
	gtk_range_set_value(GTK_RANGE(progress_bar), prog_position);
	gtk_range_set_fill_level(GTK_RANGE(progress_bar), prog_position);
	return G_SOURCE_REMOVE;
}

static void progress_bar_callback(gdouble value){
	gtk_range_set_value(GTK_RANGE(progress_bar), value);
	gtk_range_set_fill_level(GTK_RANGE(progress_bar), value);
	sound_set_position(value);
}

void music_finish_callback(){
	if(is_repeat == false){
		next_callback();
	}else{
		playsong(NULL, GINT_TO_POINTER(cur_index));
	}	
}

static void repeat_callback(){
	if(is_repeat == false){
		gtk_widget_add_css_class(GTK_WIDGET(repeat), "suggested-action");
	}else{
		gtk_widget_remove_css_class(GTK_WIDGET(repeat), "suggested-action");
	}
	
	is_repeat = !is_repeat;
}

static void like_callback(){
	if(db_like_exists(music_list[cur_index].id) == DB_EXISTS){
		gtk_widget_remove_css_class(GTK_WIDGET(like), "destructive-action");
		db_like_remove(music_list[cur_index].id);
	}else{
		gtk_widget_add_css_class(GTK_WIDGET(like), "destructive-action");
		db_like_add(music_list[cur_index].id);
	}
}
