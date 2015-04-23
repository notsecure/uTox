#include "../main.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
    
#ifdef MAKEFILE
#include "interaction.m"
#include "drawing.m"
#include "video.m"
#include "grabdesktop.m"
#endif

#define DEFAULT_WIDTH (382 * DEFAULT_SCALE)
#define DEFAULT_HEIGHT (320 * DEFAULT_SCALE)

static _Bool utox_portable = 0;

void debug(const char *fmt, ...) {
    va_list l;
    va_start(l, fmt);
    NSLogv(@(fmt), l);
    va_end(l);
}

int UTOX_NATIVE_IMAGE_IS_VALID(UTOX_NATIVE_IMAGE *img) {
    return img->image != nil;
}

UTOX_NATIVE_IMAGE *png_to_image(const UTOX_PNG_IMAGE data, size_t size, uint16_t *w, uint16_t *h, _Bool keep_alpha) {
    CFDataRef idata_copy = CFDataCreate(kCFAllocatorDefault, data->png_data, size);
    CGDataProviderRef src = CGDataProviderCreateWithCFData(idata_copy);
    CGImageRef underlying_img = CGImageCreateWithPNGDataProvider(src, NULL, YES, kCGRenderingIntentDefault);
    CGDataProviderRelease(src);
    CFRelease(idata_copy);

    if (underlying_img) {
        *w = CGImageGetWidth(underlying_img);
        *h = CGImageGetHeight(underlying_img);
        UTOX_NATIVE_IMAGE *ret = malloc(sizeof(UTOX_NATIVE_IMAGE));
        ret->scale = 1.0;
        ret->image = underlying_img;
        return ret;
    } else {
        return NULL;
    }
}

void image_set_filter(UTOX_NATIVE_IMAGE *image, uint8_t filter) {

}

void image_set_scale(UTOX_NATIVE_IMAGE *image, double scale) {
    image->scale = scale;
}

void image_free(UTOX_NATIVE_IMAGE *img) {
    CGImageRelease(img->image);
    free(img);
}

static BOOL theme_set_on_argv = NO;

int parse_argv(int argc, char const *argv[]) {
    int parse_args_wait_for_theme = 0;

    if (argc > 1)
        for (int i = 1; i < argc; i++) {
            if (parse_args_wait_for_theme) {
                if(!strcmp(argv[i], "default")) {
                    theme = THEME_DEFAULT;
                    parse_args_wait_for_theme = 0;
                    theme_set_on_argv = YES;
                    continue;
                }
                if(!strcmp(argv[i], "dark")) {
                    theme = THEME_DARK;
                    parse_args_wait_for_theme = 0;
                    theme_set_on_argv = YES;
                    continue;
                }
                if(!strcmp(argv[i], "light")) {
                    theme = THEME_LIGHT;
                    parse_args_wait_for_theme = 0;
                    theme_set_on_argv = YES;
                    continue;
                }
                if(!strcmp(argv[i], "highcontrast")) {
                    theme = THEME_HIGHCONTRAST;
                    parse_args_wait_for_theme = 0;
                    theme_set_on_argv = YES;
                    continue;
                }
                debug("Please specify correct theme (please check user manual for list of correct values).");
                return 1;
            }

            if(!strcmp(argv[i], "--version")) {
                debug("%s\n", VERSION);
                return 1;
            }
            if(!strcmp(argv[i], "--portable")) {
                debug("Launching uTox in portable mode: All data will be saved to the tox folder in the current working directory\n");
                utox_portable = 1;
            }
            if(!strcmp(argv[i], "--theme")) {
                parse_args_wait_for_theme = 1;
            }
            printf("arg %d: %s\n", i, argv[i]);
        }

    if (parse_args_wait_for_theme) {
        debug("Expected theme name, but got nothing. -_-\n");
        return 0;
    }
    return 0;
}

void thread(void func(void*), void *args) {
    pthread_t thread_temp;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1 << 18);
    pthread_create(&thread_temp, &attr, (void*(*)(void*))func, args);
    pthread_attr_destroy(&attr);
}

void yieldcpu(uint32_t ms) {
    usleep(1000 * ms);
}

/* *** audio/video *** */
void audio_detect(void) {}

_Bool audio_init(void *handle) {
    return 0;
}

_Bool audio_close(void *handle) {
    return 0;
}

_Bool audio_frame(int16_t *buffer) {
    return 0;
}

/* *** os *** */

int systemlang(void) {
    // FIXME maybe replace with NSLocale?
    char *str = getenv("LC_MESSAGES");
    if(!str) {
        str = getenv("LANG");
    }
    if(!str) {
        return DEFAULT_LANG;
    }
    return ui_guess_lang_by_posix_locale(str, DEFAULT_LANG);
}

uint64_t get_time(void) {
    struct timespec ts;
    clock_serv_t muhclock;
    mach_timespec_t machtime;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &muhclock);
    clock_get_time(muhclock, &machtime);
    mach_port_deallocate(mach_task_self(), muhclock);
    ts.tv_sec = machtime.tv_sec;
    ts.tv_nsec = machtime.tv_nsec;

    return ((uint64_t)ts.tv_sec * (1000 * 1000 * 1000)) + (uint64_t)ts.tv_nsec;
}

void openurl(char_t *str) {
    NSString *urls = [[NSString alloc] initWithCString:(char *)str encoding:NSUTF8StringEncoding];
    NSURL *url = [NSURL fileURLWithPath:urls];
    [[NSWorkspace sharedWorkspace] openURL:url];
    [urls release];
}

void config_osdefaults(UTOX_SAVE *r) {
    r->window_x = 0;
    r->window_y = 0;
    r->window_width = DEFAULT_WIDTH;
    r->window_height = DEFAULT_HEIGHT;
}

int datapath_old(uint8_t *dest) {
    return 0;
}

/* it occured to me that we should probably make datapath allocate memory for its caller */
int datapath(uint8_t *dest) {
    if (utox_portable) {
        const char *home = NSBundle.mainBundle.bundlePath.UTF8String;
        int l = sprintf((char*)dest, "%.238s/tox", home);
        mkdir((char*)dest, 0700);
        dest[l++] = '/';

        return l;
    } else {
        const char *home = NSHomeDirectory().UTF8String;
        int l = sprintf((char*)dest, "%.230s/.config/tox", home);
        mkdir((char*)dest, 0700);
        dest[l++] = '/';

        return l;
    }
}

int datapath_subdir(uint8_t *dest, const char *subdir) {
    int l = datapath(dest);
    l += sprintf((char*)(dest+l), "%s", subdir);
    mkdir((char*)dest, 0700);
    dest[l++] = '/';
    
    return l;
}

int ch_mod(uint8_t *file){
    return chmod((char*)file, S_IRUSR | S_IWUSR);
}

int file_lock(FILE *file, uint64_t start, size_t length){
    int result = -1;
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = length;

    result = fcntl(fileno(file), F_SETLK, &fl);
    if(result != -1){
        return 1;
    } else {
        return 0;
    }
}

int file_unlock(FILE *file, uint64_t start, size_t length){
    int result = -1;
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = length;

    result = fcntl(fileno(file), F_SETLK, &fl);
    if(result != -1){
        return 1;
    } else {
        return 0;
    }
}

void flush_file(FILE *file) {
    fflush(file);
    int fd = fileno(file);
    fsync(fd);
}

// do not breakpoint in this function or you're gonna have a fun time
void desktopgrab(_Bool video) {
    uToxAppDelegate *ad = (uToxAppDelegate *)[NSApp delegate];
    NSScreen *target = [ad.utox_window screen];

    NSWindow *window = [uToxStardustView createWindowOnScreen:target];
    uToxStardustView *v = [[uToxStardustView alloc] initWithFrame:(CGRect){0, 0, window.frame.size.width, window.frame.size.height}];
    v.video = video;
    window.contentView = v;
    [window makeKeyAndOrderFront:ad];
    [v release];
}

void utoxshield_display_capping_done(_Bool video, uint32_t ret, NSWindow *window) {
    uToxStardustView *v = window.contentView;
    NSScreen *target = window.screen;

    if ((ret & 0xff) == 0) {
        if (!video) {
            CGRect rect = [v getRect];
            rect.origin.y = target.frame.size.height - rect.origin.y - rect.size.height;
            CGImageRef inliness = CGWindowListCreateImage(rect, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
            UTOX_NATIVE_IMAGE *img = malloc(sizeof(UTOX_NATIVE_IMAGE));
            img->scale = 1.0;
            img->image = inliness;

            CFMutableDataRef dat = CFDataCreateMutable(kCFAllocatorDefault, 0);
            CGImageDestinationRef dest = CGImageDestinationCreateWithData(dat, kUTTypePNG, 1, NULL);
            CGImageDestinationAddImage(dest, inliness, NULL);
            CGImageDestinationFinalize(dest);
            CFRelease(dest);

            size_t size = CFDataGetLength(dat);
            uint8_t *owned_ptr = malloc(size);
            memcpy(owned_ptr, CFDataGetBytePtr(dat), size);
            CFRelease(dat);

            friend_sendimage(sitem->data, img, CGImageGetWidth(inliness), CGImageGetHeight(inliness), (UTOX_PNG_IMAGE)owned_ptr, size);
        } else {
            desktop_capture_from = (ret >> 8) & 0xffffff;
            CGRect rect = [v getRect];
            //rect.origin.y = target.frame.size.height - rect.origin.y - rect.size.height;

            desktop_capture_rect = rect;
        }
    }

    //dispatch_async(dispatch_get_main_queue(), ^{
        [window release];
    //});
}

void postmessage(uint32_t msg, uint16_t param1, uint16_t param2, void *data) {
    dispatch_async(dispatch_get_main_queue(), ^{
        tox_message(msg, param1, param2, data);
    });
}

void redraw(void) {
    uToxAppDelegate *ad = (uToxAppDelegate *)[NSApp delegate];
    [ad soilWindowContents];
}

@implementation uToxAppDelegate

#undef TITLE
#define TITLE "µTox"
#undef SUB_TITLE
#define SUB_TITLE "(-cocoa alpha)"
- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    ironclad = [[NSMutableDictionary alloc] init];

    // hold COMMAND to start utox in portable mode
    // unfortunately, OS X doesn't have the luxury of passing argv in the GUI
    if ([NSEvent modifierFlags] & NSCommandKeyMask)
        utox_portable = 1;

    /* load save data */
    UTOX_SAVE *save = config_load();
    if (!theme_set_on_argv)
        theme = save->theme;
    theme_load(theme);

    /* start the tox thread */
    thread(tox_thread, NULL);

    /* wait for the tox thread to finish initializing */
    while(!tox_thread_init) {
        yieldcpu(1);
    }

    /* set up the contact list */
    list_start();

    self.nameMenuItem = [[[NSMenuItem alloc] initWithTitle:@"j" action:NULL keyEquivalent:@""] autorelease];
    self.statusMenuItem = [[[NSMenuItem alloc] initWithTitle:@"j" action:NULL keyEquivalent:@""] autorelease];
    update_tray();
    //[self.nameMenuItem release];
    //[self.statusMenuItem release];

    char title_name[128];
    snprintf(title_name, 128, "%s %s (version: %s)", TITLE, SUB_TITLE, VERSION);

#define WINDOW_MASK (NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask)
    self.utox_window = [[NSWindow alloc] initWithContentRect:(NSRect){save->window_x, save->window_y, save->window_width, save->window_height} styleMask:WINDOW_MASK backing:NSBackingStoreBuffered defer:NO screen:[NSScreen mainScreen]];
#undef WINDOW_MASK

    max_video_width = [NSScreen mainScreen].frame.size.width;
    max_video_height = [NSScreen mainScreen].frame.size.height;
    self.utox_window.delegate = self;

    self.utox_window.title = @(title_name);
    self.utox_window.contentView = [[[uToxView alloc] initWithFrame:(CGRect){0, 0, self.utox_window.frame.size}] autorelease];

    ui_scale(save->scale + 1);

    [self.utox_window makeFirstResponder:self.utox_window.contentView];
    [self.utox_window makeKeyAndOrderFront:self];

    /* done with save */
    free(save);
}

- (NSMenu *)applicationDockMenu:(NSApplication *)sender {
    if (!self.dockMenu) {
        self.dockMenu = [[[NSMenu alloc] init] autorelease];
        [self.dockMenu addItem:self.nameMenuItem];
        [self.dockMenu addItem:self.statusMenuItem];
    }
    return self.dockMenu;
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    toxaudio_postmessage(AUDIO_KILL, 0, 0, NULL);
    toxvideo_postmessage(VIDEO_KILL, 0, 0, NULL);
    toxav_postmessage(TOXAV_KILL, 0, 0, NULL);
    tox_postmessage(TOX_KILL, 0, 0, NULL);

    UTOX_SAVE d = {
        // from bottom of screen
        // TODO: translate to xy from top
        .window_x = self.utox_window.frame.origin.x,
        .window_y = self.utox_window.frame.origin.y,
        .window_width = self.utox_window.frame.size.width,
        .window_height = self.utox_window.frame.size.height,
    };

    config_save(&d);

    /* wait for threads to exit */
    while(tox_thread_init) {
        yieldcpu(1);
    }

    debug("clean exit\n");
}

- (void)soilWindowContents {
    uToxView *cv = self.utox_window.contentView;
    [cv setNeedsDisplay:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (void)windowDidChangeScreen:(NSNotification *)notification {
    if (notification.object == self.utox_window) {
        NSScreen *screen = self.utox_window.screen;
        max_video_width = screen.frame.size.width;
        max_video_height = screen.frame.size.height;
    }
}

- (void)dealloc {
    [ironclad release];
    [devices release];
    [super dealloc];
}

@end

int main(int argc, char const *argv[]) {
    theme = THEME_DEFAULT;

    int fatal = parse_argv(argc, argv);
    if (fatal)
        return fatal;

    setlocale(LC_ALL, "");

    LANG = systemlang();
    dropdown_language.selected = dropdown_language.over = LANG;

    /* set the width/height of the drawing region */
    utox_window_width = DEFAULT_WIDTH;
    utox_window_height = DEFAULT_HEIGHT;
    ui_size(utox_window_width, utox_window_height);

    /* event loop */
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        NSArray *maybeMenus = nil;
        BOOL ok = [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:nil topLevelObjects:&maybeMenus];
        if (ok) {
            for (id obj in maybeMenus) {
                if ([obj isKindOfClass:[NSMenu class]]) {
                    app.mainMenu = obj;
                    break;
                }
            }
        }

        uToxAppDelegate *appdelegate;
        app.delegate = appdelegate = [[uToxAppDelegate alloc] init];
        [app run];
        [appdelegate release]; // never executed
    }

    return 1;
}