#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include "logger.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int inotifyFd;

static int
get_files(const char *fpath, const struct stat *sb,
	  int tflag, struct FTW *ftwbuf)
{
	/* Check for creation, deletion or rename of files */
	int wd = inotify_add_watch(inotifyFd, fpath,
				   IN_CREATE | IN_DELETE | IN_MOVED_FROM |
				   IN_MOVED_TO);
	if (wd == -1) {
		errorf("couldn't add to inotify_add_watch");
		exit(EXIT_FAILURE);
	}
	return 0;		/* To tell nftw() to continue */
}

static void /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
	if (i->mask & IN_CREATE)
		infof("IN_CREATE ");
	if (i->mask & IN_DELETE)
		infof("IN_DELETE ");
	if (i->mask & IN_MOVED_FROM)
		infof("IN_MOVED_FROM ");
	if (i->mask & IN_MOVED_TO)
		infof("IN_MOVED_TO ");
	printf("\n");

	if (i->len > 0)
		printf("        name = %s\n", i->name);
}

int main(int argc, char *argv[])
{
	inotifyFd = inotify_init();
	if (inotifyFd == -1) {
		errorf("couldn't create inotifyFd");
		exit(EXIT_FAILURE);
	}

	int flags = FTW_PHYS;	/* Don't follow symbolic links */
	if (nftw((argc < 2) ? "." : argv[1], get_files, 20, flags) == -1) {
		panicf("couldn't transverse nftw");
		exit(EXIT_FAILURE);
	}

	char buf[BUF_LEN] __attribute__ ((aligned(8)));
	ssize_t numRead;
	char *p;
	struct inotify_event *event;

	for (;;) {		/* Read events forever */
		numRead = read(inotifyFd, buf, BUF_LEN);
		if (numRead == 0) {
			panicf("read() from inotify fd returned 0!");
			exit(EXIT_FAILURE);
		}
		if (numRead == -1) {
			errorf("read");
			exit(EXIT_FAILURE);
		}
		// infof("Read %ld bytes from inotify fd\n", (long) numRead);
		for (p = buf; p < buf + numRead;) {
			event = (struct inotify_event *)p;
			displayInotifyEvent(event);
			p += sizeof(struct inotify_event) + event->len;
		}
		inotifyFd = inotify_init();
		if (nftw((argc < 2) ? "." : argv[1], get_files, 20, flags) ==
		    -1) {
			panicf("couldn't transverse nftw");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
