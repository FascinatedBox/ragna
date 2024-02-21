// SPDX-License-Identifier: GPL-2.0-only
/*
 * qvidcap: a control panel controlling v4l2 devices.
 *
 * Copyright 2018 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#include <QApplication>
#include "ragnacontroller.h"
#include "v4l2-info.h"

static void usage()
{
	puts("Usage: ragna <options>\n\n"
	       "Options:\n\n"
	       "  -d, --device=<dev>       use device <dev> as the video device\n"
	       "                           if <dev> is a number, then /dev/video<dev> is used\n"
	       "\n"
	       "  If -d is not specified, then use /dev/video0.\n"
	       "\n"
	       "  -b, --buffers=<bufs>     request <bufs> buffers (default 4) when streaming\n"
	       "                           from a video device\n"
	       "  -h, --help               display this help message\n"
	       "  -t, --timings            report frame render timings\n"
	       "  -v, --verbose            be more verbose\n"
	       "  -R, --raw                open device in raw mode\n"
	       "\n"
	       "  --opengl                 force openGL to display the video\n"
	       "                           (default: openGL ES)\n");
}

static void usageError(const char *msg)
{
	printf("Missing parameter for %s\n", msg);
	usage();
}

static void usageInvParm(const char *msg)
{
	printf("Invalid parameter for %s\n", msg);
	usage();
}

static QString getDeviceName(QString dev, QString &name)
{
	bool ok;
	name.toInt(&ok);
	return ok ? QString("%1%2").arg(dev).arg(name) : name;
}

static bool processOption(const QStringList &args, int &i, QString &s)
{
	int index = -1;

	if (args[i][1] == '-')
		index = args[i].indexOf('=');
	else if (args[i].length() > 2)
		index = 1;

	if (index >= 0) {
		s = args[i].mid(index + 1);
		if (s.length() == 0) {
			usageError(args[i].toUtf8());
			return false;
		}
		return true;
	}
	if (i + 1 >= args.size()) {
		usageError(args[i].toUtf8());
		return false;
	}
	s = args[++i];
	return true;
}

static bool processOption(const QStringList &args, int &i, unsigned &u)
{
	QString s;
	bool ok = processOption(args, i, s);

	if (!ok)
		return ok;
	u = s.toUInt(&ok, 0);
	if (!ok)
		usageInvParm(s.toUtf8());
	return ok;
}

static bool isOptArg(const QString &opt, const char *longOpt, const char *shortOpt = NULL)
{
	return opt.startsWith(longOpt) || (shortOpt && opt.startsWith(shortOpt));
}

static bool isOption(const QString &opt, const char *longOpt, const char *shortOpt = NULL)
{
	return opt == longOpt || opt == shortOpt;
}

QString loadFile(QString path)
{
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    QString s = f.readAll();
    f.close();

    return s;
}

int main(int argc, char **argv)
{
	QApplication disp(argc, argv);
	RagnaScrollArea *rsa = new RagnaScrollArea();
	QSurfaceFormat format;
	QString video_device = "0";
	QString filename;
	cv4l_fd fd;
	cv4l_fmt fmt;
	unsigned v4l2_bufs = 4;
	bool info_option = false;
	bool report_timings = false;
	bool verbose = false;
	bool force_opengl = false;

	disp.setApplicationDisplayName("Ragna Viewer");
	QStringList args = disp.arguments();
	for (int i = 1; i < args.size(); i++) {
		QString s;

		if (isOptArg(args[i], "--device", "-d")) {
			if (!processOption(args, i, video_device))
				return 0;
		} else if (isOption(args[i], "--help", "-h")) {
			usage();
			info_option = true;
		} else if (isOption(args[i], "--timings", "-t")) {
			report_timings = true;
		} else if (isOptArg(args[i], "--opengl")) {
			force_opengl = true;
		} else if (isOption(args[i], "--verbose", "-v")) {
			verbose = true;
		} else if (isOption(args[i], "--raw", "-R")) {
			fd.s_direct(true);
		} else if (isOptArg(args[i], "--buffers", "-b")) {
			if (!processOption(args, i, v4l2_bufs))
				return 0;
		} else {
			printf("Invalid argument %s\n", args[i].toUtf8().data());
			return 0;
		}
	}
	if (info_option)
		return 0;

	video_device = getDeviceName("/dev/video", video_device);
	if (fd.open(video_device.toUtf8().data(), true) < 0) {
		perror((QString("could not open ") + video_device).toUtf8().data());
		std::exit(EXIT_FAILURE);
	}
	if (!fd.has_vid_cap()) {
		fprintf(stderr, "%s is not a video capture device\n", video_device.toUtf8().data());
		std::exit(EXIT_FAILURE);
	}

	fd.g_fmt(fmt);

	RagnaController rc;

	rc.loadPrefs();
	rc.updateFormatForPrefs(&fmt);
	fd.s_fmt(fmt);

	{
		bool found = false;
		unsigned int pf = fmt.g_pixelformat();

		for (unsigned i = 0; formats[i]; i++) {
			if (pf == formats[i]) {
				found = true;
				break;
			}
		}

		if (!found) {
			/* Try fixing it to one that's known to work. */
			__u32 overridePixelFormat = V4L2_PIX_FMT_RGB24;

			fmt.s_pixelformat(V4L2_PIX_FMT_RGB24);
			fd.s_fmt(fmt);
			fd.g_fmt(fmt);

			if (fmt.g_pixelformat() != overridePixelFormat)
				fprintf(stderr, "Not able to override format to %s (%s)\n",
					fcc2s(overridePixelFormat).c_str(),
					pixfmt2s(overridePixelFormat).c_str());
			else
				found = true;
		}

		if (!found) {
			fprintf(stderr, "Unknown/invalid device format %s (%s)\n",
				fcc2s(pf).c_str(), pixfmt2s(pf).c_str());
			std::exit(EXIT_FAILURE);
		}
	}

	format.setDepthBufferSize(24);

	if (force_opengl)
		format.setRenderableType(QSurfaceFormat::OpenGL);
	else
		format.setRenderableType(QSurfaceFormat::OpenGLES);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);

	QSurfaceFormat::setDefaultFormat(format);
	CaptureWin win(rsa);
	win.setVerbose(verbose);
	win.setModeV4L2(&fd);
	win.setFormat(format);
	win.setReportTimings(report_timings);
	while (!win.setV4LFormat(fmt)) {
		fprintf(stderr, "Unsupported format: '%s' %s\n",
			fcc2s(fmt.g_pixelformat()).c_str(),
			pixfmt2s(fmt.g_pixelformat()).c_str());
		std::exit(EXIT_FAILURE);
	}

	rc.setCapture(&win, rsa);
	rsa->resize(QSize(fmt.g_width(), fmt.g_frame_height()));

	cv4l_queue q(fd.g_type(), V4L2_MEMORY_MMAP);
	q.reqbufs(&fd, v4l2_bufs);
	q.obtain_bufs(&fd);
	q.queue_all(&fd);
	win.setQueue(&q);
	if (fd.streamon()) {
		fputs("Error initializing the stream. Stopping.\n", stderr);
		std::exit(EXIT_FAILURE);
	}

	disp.setStyleSheet(loadFile(":/style.qss"));

	rc.start();

	return disp.exec();
}
