#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <ftw.h>
#include <sys/inotify.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#define WDNAMENUM   100
#define TYPE_FILE   1
#define TYPE_DIR    0

struct wd_name {
    int wd;
    char *name;
    int name_len;
    char type;
};

struct wd_name wdName[WDNAMENUM];
int wdName_count;
int inotidy;

static void errExit(int line, const char *format,...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);

    printf("%s line: %d\n", buf, line);
    exit(EXIT_FAILURE);
}

static void add_inodity_for_entry(const char *filename, char type) 
{
    int wd;
    int i;

    wd = inotify_add_watch(inotidy, filename, IN_ALL_EVENTS);
    if (wd == -1) {
        errExit(__LINE__, "inotify_add_watch failed");
    }

    for (i = 0; i < WDNAMENUM; i++) {
        if (wdName[i].name == NULL) {
            break;
        }
    }

    wdName[i].type = type;
    wdName[i].wd = wd;
    wdName[i].name_len = strlen(filename) + 1;
    wdName[i].name = malloc(wdName[i].name_len);
    if (wdName[i].name == NULL) {
        errExit(__LINE__, "malloc failed! count: %d", i);
    }
    strcpy(wdName[i].name, filename);

    wdName_count++;
}

static void remove_inodity_for_entry(int wdNmae_num) 
{
    inotify_rm_watch(inotidy, wdName[wdName_count].wd);
    wdName_count--;
    free(wdName[wdNmae_num].name);
    memset(&wdName, 0, sizeof(struct wd_name));
}

static int nftw_func(const char *filename, const struct stat *status, 
                    int flag, struct FTW *info)
{

    if (flag & FTW_D) {
        add_inodity_for_entry(filename, TYPE_DIR);
    } else {
        add_inodity_for_entry(filename, TYPE_FILE);
    }

    return 0;
}

static void displayInotifyEvent(struct inotify_event *event)
{
    int i;
    struct wd_name *temp;
    char path[PATH_MAX];
    struct stat st;

    for (i = 0; i < WDNAMENUM; i++) {
        if (wdName[i].wd == event->wd) {
            temp = &wdName[i];
            break;
        }
    }

    sprintf(path, "%s/%s", temp->name, event->name);

    switch (event->mask) {
        case IN_CREATE :
            if (stat(path, &st) == -1) {
                errExit(__LINE__, "stat failed! Path: %s", path);
            }
            switch (st.st_mode & S_IFMT) {
                case S_IFREG:
                    printf("%s create a file, file name is %s\n", temp->name, event->name);
                    add_inodity_for_entry(path, TYPE_FILE);
                    break;
                case S_IFDIR:
                    printf("%s create a dir, file name is %s\n", temp->name, event->name);
                    add_inodity_for_entry(path, TYPE_DIR);
                    break;
                }

            break;
        
        case IN_DELETE:
            if (temp->type == TYPE_FILE) {
                printf("%s delete a file, file name is %s\n", temp->name, event->name);
            } else {
                printf("%s delete a dir, file name is %s\n", temp->name, event->name);
            }
            remove_inodity_for_entry(i);
            break;

        case IN_MOVED_FROM:
            printf("%s move a file, before move file name is %s\n", temp->name, event->name);
            break;

        case IN_MOVED_TO:
            printf("%s move a file, after move file name is %s\n", temp->name, event->name);
            break;

        default :
            break;
    }

}

#define BUF_LEN  (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(char argc, char *argv[])
{
    char buf[BUF_LEN];
    int numRd;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || (strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr,"%s path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    wdName_count = 0;
    memset(wdName, 0, sizeof(wdName));

    inotidy = inotify_init();
    if (inotidy == -1) {
        errExit(__LINE__, "inotify_init failed!");
    }

    if (nftw(argv[1], nftw_func, 20, 0) == -1) {
        errExit(__LINE__, "nftw failed!");
    }

    while (1) {
        numRd = read(inotidy, buf, BUF_LEN);
        if (numRd == -1) {
            errExit(__LINE__, "read failed!");
        }


        for (p = buf; p < buf + numRd; ) {
            event = (struct inotify_event *)p;
            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}