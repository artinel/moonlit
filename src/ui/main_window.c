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
static GObject* volume;
static GObject* volume_dialog;
static GObject* tab_home;
static GObject* tab_like;
static GObject* tab_dir;
static GObject* tab_search;
static GObject* dir_dialog;
static GObject* search_dialog;
static GObject* empty;

static music_t music_list[MUSIC_LIST_SIZE];
static GtkWidget* btn_list[MUSIC_LIST_SIZE];
static int list_size = 0;
static int cur_index = 0;
static bool is_repeat = false;
static int volume_level = 70;
static GObject* cur_tab_active;
static music_t cur_playing;

static pthread_t prog_thread;
static double prog_position;

static void playpause_callback();
static void previous_callback();
static void next_callback();
static void* progress_thread(void* arg);
static gboolean progress_update();
static void progress_start();
static void progress_stop();
static void progress_bar_callback(gdouble value);
static void repeat_callback();
static void like_callback();
static void volume_callback();
static void volume_bar_callback(GtkScale* self, gdouble value);
static void tab_like_callback();
static void tab_home_callback();
static void tab_dir_callback();
static void dir_dialog_close();
static void dir_dialog_add();
static void dir_delete_callback(GtkWidget* self, gpointer data);
static void tab_search_callback();
static void tab_search_apply_callback();

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
	volume = get_object(builder, "volume");
	tab_home = get_object(builder, "tab_home");
	tab_like = get_object(builder, "tab_like");
	tab_dir = get_object(builder, "tab_dir");
	tab_search = get_object(builder, "tab_search");

	GtkBuilder* empty_builder = load_ui("/ui/empty");
	empty = get_object(empty_builder, "empty");

	g_signal_connect(GTK_BUTTON(playpause), "clicked", G_CALLBACK(playpause_callback), NULL);
	g_signal_connect(GTK_BUTTON(previous), "clicked", G_CALLBACK(previous_callback), NULL);
	g_signal_connect(GTK_BUTTON(next), "clicked", G_CALLBACK(next_callback), NULL);
	g_signal_connect(GTK_SCALE(progress_bar), "change-value", G_CALLBACK(progress_bar_callback), NULL);
	g_signal_connect(GTK_BUTTON(repeat), "clicked", G_CALLBACK(repeat_callback), NULL);
	g_signal_connect(GTK_BUTTON(like), "clicked", G_CALLBACK(like_callback), NULL);
	g_signal_connect(GTK_BUTTON(volume), "clicked", G_CALLBACK(volume_callback), NULL);
	g_signal_connect(GTK_BUTTON(tab_like), "clicked", G_CALLBACK(tab_like_callback), NULL);
	g_signal_connect(GTK_BUTTON(tab_home), "clicked", G_CALLBACK(tab_home_callback), NULL);
	g_signal_connect(GTK_BUTTON(tab_dir), "clicked", G_CALLBACK(tab_dir_callback), NULL);
	g_signal_connect(GTK_BUTTON(tab_search), "clicked", G_CALLBACK(tab_search_callback), NULL);

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	load_css(GTK_WIDGET(window), "/css/global-style");
	gtk_widget_set_visible(GTK_WIDGET(window), true);


	if(db_table_count(MUSIC_DIR) > 0){
		home_init(window);
		sound_set_volume(volume_level);
		gtk_widget_add_css_class(GTK_WIDGET(tab_home), "suggested-action");
		cur_tab_active = tab_home;
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

void clear_music_list(){
	list_size = 0;
	cur_index = 0;
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

static void volume_callback(){
	GtkBuilder* vol_builder = load_ui("/ui/volume");
	volume_dialog = get_object(vol_builder, "volume_dialog");
	GObject* volume_bar = get_object(vol_builder, "volume_bar");

	gtk_range_set_range(GTK_RANGE(volume_bar),0, 100);
	gtk_range_set_value(GTK_RANGE(volume_bar), volume_level);
	gtk_range_set_fill_level(GTK_RANGE(volume_bar), volume_level);

	g_signal_connect(GTK_SCALE(volume_bar), "change-value", G_CALLBACK(volume_bar_callback), NULL);
	adw_dialog_present(ADW_DIALOG(volume_dialog), GTK_WIDGET(window));
}

static void volume_bar_callback(GtkScale* self, gdouble value){
	volume_level = value;
	gtk_range_set_value(GTK_RANGE(self), volume_level);
	gtk_range_set_fill_level(GTK_RANGE(self), volume_level);
	sound_set_volume(volume_level);
}

static void tab_like_callback(GtkWidget* self){
	if(GTK_WIDGET(cur_tab_active) != self){
		clear_music_list();
		clear_home_list();
		gtk_widget_remove_css_class(GTK_WIDGET(cur_tab_active), "suggested-action");
		gtk_widget_add_css_class(GTK_WIDGET(self), "suggested-action");
		cur_tab_active = (GObject*)self;

		music_t list[MUSIC_LIST_SIZE];
		int count = db_like_get_all(list, MUSIC_LIST_SIZE);
		if(count > 0){
			clear_home_list();
			clear_music_list();
			fill_listbox(GTK_LIST_BOX(get_home_list()), list, count);
			set_main_view(get_home_view());
		}else{
			set_view_empty();
		}

	}
}

static void tab_home_callback(GtkWidget* self){
	if(GTK_WIDGET(cur_tab_active) != self){
		clear_music_list();
		clear_home_list();
		gtk_widget_remove_css_class(GTK_WIDGET(cur_tab_active), "suggested-action");
		gtk_widget_add_css_class(GTK_WIDGET(self), "suggested-action");
		cur_tab_active = (GObject*)self;
		
		music_t list[MUSIC_LIST_SIZE];
		int count = db_music_get_all(list, MUSIC_LIST_SIZE);
		if(count > 0){
			clear_home_list();
			clear_music_list();
			fill_listbox(GTK_LIST_BOX(get_home_list()), list, count);
			set_main_view(get_home_view());
		}else{
			set_view_empty();
		}
	}
}

static void tab_dir_callback(GtkWidget* self){
	GtkBuilder* dir_builder = dir_builder = load_ui("/ui/dir_dialog");
	dir_dialog = get_object(dir_builder, "dir_dialog");
	GObject* close = get_object(dir_builder, "dir_dialog_close");
	GObject* add = get_object(dir_builder, "dir_dialog_add");
	GObject* list = get_object(dir_builder, "dir_dialog_list");

	g_signal_connect(GTK_BUTTON(close), "clicked", G_CALLBACK(dir_dialog_close), NULL);
	g_signal_connect(GTK_BUTTON(add), "clicked", G_CALLBACK(dir_dialog_add), NULL);
	
	music_dir_t data[MUSIC_LIST_SIZE];
	int count = db_music_dir_get_all(data, MUSIC_LIST_SIZE);

	for(int i = 0; i < count; i++){
		GtkWidget* row = adw_action_row_new();
		adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), (const char*)data[i].dir);

		gtk_widget_set_margin_top(GTK_WIDGET(row), 40);

		GtkWidget* delete = gtk_button_new_with_label("Delete");
		gtk_widget_add_css_class(GTK_WIDGET(delete), "destructive-action");
		adw_action_row_add_suffix(ADW_ACTION_ROW(row), GTK_WIDGET(delete));
		
		

		g_signal_connect_data(GTK_BUTTON(delete), "clicked", G_CALLBACK(dir_delete_callback), GINT_TO_POINTER(data[i].id), NULL, 0);

		gtk_list_box_append(GTK_LIST_BOX(list), GTK_WIDGET(row));
	}

	adw_dialog_present(ADW_DIALOG(dir_dialog), GTK_WIDGET(window));
}

static void dir_dialog_close(){
	adw_dialog_close(ADW_DIALOG(dir_dialog));
}

static void dir_dialog_add(){
	show_dir_add_dialog(GTK_WIDGET(window));
	dir_dialog_close();
}



static void dir_delete_callback(GtkWidget* self, gpointer data){
	int id = GPOINTER_TO_INT(data);
	

	db_music_dir_delete(id);
	db_music_delete(id);

	music_t list[MUSIC_LIST_SIZE];
	int count = db_music_get_all(list, MUSIC_LIST_SIZE);
	if(count > 0){
		clear_home_list();
		clear_music_list();
		fill_listbox(GTK_LIST_BOX(get_home_list()), list, count);
		set_main_view(get_home_view());
	}else{
		set_view_empty();
	}
	dir_dialog_close();
}

static void tab_search_callback(){
	GtkBuilder* search_builder = load_ui("/ui/search_dialog");
	search_dialog = get_object(search_builder, "search_dialog");
	GObject* entry = get_object(search_builder, "search_entry");

	g_signal_connect(ADW_ENTRY_ROW(entry), "apply", G_CALLBACK(tab_search_apply_callback), NULL);
	adw_dialog_present(ADW_DIALOG(search_dialog), GTK_WIDGET(window));
}


static void tab_search_apply_callback(GtkWidget* self){
	const char* search = gtk_editable_get_chars(GTK_EDITABLE(self), 0, gtk_editable_get_max_width_chars(GTK_EDITABLE(self)));
	
	music_t list[MUSIC_LIST_SIZE];
	char buf[33];
	snprintf(buf, sizeof(buf), "%c%s%c", '%', search, '%');
	int count = db_music_search(list, MUSIC_LIST_SIZE, buf);
	if(count > 0){
		clear_home_list();
		clear_music_list();
		fill_listbox(GTK_LIST_BOX(get_home_list()), list, count);
		set_main_view(get_home_view());
	}else{
		set_view_empty();
	}
	gtk_widget_remove_css_class(GTK_WIDGET(cur_tab_active), "suggested-action");
	gtk_widget_add_css_class(GTK_WIDGET(tab_search), "suggested-action");
	cur_tab_active = (GObject*)tab_search;
		
	adw_dialog_close(ADW_DIALOG(search_dialog));
}

void set_view_empty(){
	set_main_view(empty);
}

void set_cur_playing(music_t music){
	cur_playing = music;
}

music_t get_cur_playing(){
	return cur_playing;
}
