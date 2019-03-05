/*
 * The coloring function was made by Pradeep Padala.
 * You can find more info here https://www.linuxjournal.com/article/8603
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>

#define RESET			0
#define BRIGHT 		1
#define DIM				2
#define UNDERLINE 3
#define BLINK			4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED				1
#define GREEN			2
#define YELLOW		3
#define BLUE			4
#define MAGENTA		5
#define CYAN			6
#define	WHITE			7

//0 -> STDOUT
//1 -> SYSLOG
int outputLogger = 0;

void
textcolor (int attr, int fg, int bg)
{
  char command[13];
  sprintf (command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf ("%s", command);
}

int
initLogger (char *logType)
{
  if (strcmp ("syslog", logType) == 0)
    {
			outputLogger = 1;
      return 1;
    }
  else if (strcmp ("stdout", logType) == 0 || strcmp ("", logType) == 0)
    {
			outputLogger = 0;
      return 1;
    }
  return 0;
}

int
infof (const char *format, ...)
{
  va_list arg;
  int done;

	if (outputLogger == 0){
	  textcolor (BRIGHT, BLUE, HIDDEN);
	  printf ("INFO: ");
	  textcolor (RESET, WHITE, HIDDEN);

	  va_start (arg, format);
	  done = vprintf (format, arg);
	  va_end (arg);
	  return done;
	} else {
	  va_start (arg, format);
	  vsyslog(LOG_USER | LOG_INFO, format, arg);
	  va_end (arg);
	  return 1;
	}
}

int
warnf (const char *format, ...)
{
  va_list arg;
  int done;

	if (outputLogger == 0){
	  textcolor (BRIGHT, YELLOW, HIDDEN);
	  printf ("WARN: ");
	  textcolor (RESET, WHITE, HIDDEN);

	  va_start (arg, format);
	  done = vprintf (format, arg);
	  va_end (arg);
	  return done;
	} else {
	  va_start (arg, format);
	  vsyslog(LOG_USER | LOG_WARNING, format, arg);
	  va_end (arg);
	  return 1;
	}
}

int
errorf (const char *format, ...)
{
  va_list arg;
  int done;

	if (outputLogger == 0){
		textcolor (BRIGHT, RED, HIDDEN);
	  printf ("ERROR: ");
	  textcolor (RESET, WHITE, HIDDEN);

	  va_start (arg, format);
	  done = vprintf (format, arg);
	  va_end (arg);
	  return done;
	} else {
	  va_start (arg, format);
	  vsyslog(LOG_USER | LOG_ERR, format, arg);
	  va_end (arg);
	  return 1;
	}
}

int
panicf (const char *format, ...)
{
  va_list arg;
  int done;

	if (outputLogger == 0){
	  textcolor (BRIGHT, WHITE, RED);
	  printf ("PANIC:");
	  textcolor (RESET, WHITE, HIDDEN);
	  printf (" ");

	  va_start (arg, format);
	  done = vprintf (format, arg);
	  va_end (arg);
	  return done;
	} else {
	  va_start (arg, format);
	  vsyslog(LOG_USER | LOG_EMERG, format, arg);
	  va_end (arg);
	  return 1;
	}
}
