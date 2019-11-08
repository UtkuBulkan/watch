#include "watch_qt.h"

int main(int argc, char *argv[])
{
	setlogmask (LOG_UPTO (LOG_NOTICE));
	openlog ("watch", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	closelog ();
	return a.exec();
}
