int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);
int initLogger (char *logType);

int main()
{
	infof("This is information test\n");
	warnf("This is a %s test\n", "warn");
	errorf("This is an %s %s\n", "error", "test");
	panicf("This is a panic test\n\n");

	infof("Check your syslog file at /var/log/syslog to see the rest of calls\n");
	initLogger("syslog");
	
	infof("<info> This is information test from syslog\n");
	warnf("<warn> This is a %s test from syslog\n", "warn");
	errorf("<error> This is an %s %s from syslog\n", "error", "test");
	panicf("<panic> This is a panic test from syslog\n");

	return 0;
}
