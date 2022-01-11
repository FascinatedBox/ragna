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
	       "                           If not specified, /dev/video0 is used.\n"
	       "\n"
	       "  If -d is not specified, then use /dev/video0.\n"
	       "\n"
	       "  -b, --buffers=<bufs>     request <bufs> buffers (default 4) when streaming\n"
	       "                           from a video device\n"
	       "  -C, --colorspace=<c>     override colorspace\n"
	       "                           <c> can be one of the following colorspaces:\n"
	       "                               smpte170m, smpte240m, rec709, 470m, 470bg, jpeg, srgb,\n"
	       "                               oprgb, bt2020, dcip3\n"
	       "  -X, --xfer-func=<x>      override transfer function\n"
	       "                           <x> can be one of the following transfer functions:\n"
	       "                               default, 709, srgb, oprgb, smpte240m, smpte2084, dcip3, none\n"
	       "  -Y, --ycbcr-enc=<y>      override Y'CbCr encoding\n"
	       "                           <y> can be one of the following Y'CbCr encodings:\n"
	       "                               default, 601, 709, xv601, xv709, bt2020, bt2020c, smpte240m\n"
	       "  -H, --hsv-enc=<hsv>      override HSV encoding\n"
	       "                           <hsv> can be one of the following HSV encodings:\n"
	       "                               default, 180, 256\n"
	       "  -Q, --quant=<q>          override quantization\n"
	       "                           <q> can be one of the following quantization methods:\n"
	       "                               default, full-range, lim-range\n"
	       "\n"
	       "  -l, --list-formats       display all supported formats\n"
	       "  -h, --help               display this help message\n"
	       "  -t, --timings            report frame render timings\n"
	       "  -v, --verbose            be more verbose\n"
	       "  -R, --raw                open device in raw mode\n"
	       "\n"
	       "  --opengl                 force openGL to display the video\n"
	       "  --opengles               force openGL ES to display the video\n");
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

static __u32 parse_colorspace(const QString &s)
{
	if (s == "smpte170m") return V4L2_COLORSPACE_SMPTE170M;
	if (s == "smpte240m") return V4L2_COLORSPACE_SMPTE240M;
	if (s == "rec709") return V4L2_COLORSPACE_REC709;
	if (s == "470m") return V4L2_COLORSPACE_470_SYSTEM_M;
	if (s == "470bg") return V4L2_COLORSPACE_470_SYSTEM_BG;
	if (s == "jpeg") return V4L2_COLORSPACE_JPEG;
	if (s == "srgb") return V4L2_COLORSPACE_SRGB;
	if (s == "oprgb") return V4L2_COLORSPACE_OPRGB;
	if (s == "bt2020") return V4L2_COLORSPACE_BT2020;
	if (s == "dcip3") return V4L2_COLORSPACE_DCI_P3;
	return 0;
}

static __u32 parse_xfer_func(const QString &s)
{
	if (s == "default") return V4L2_XFER_FUNC_DEFAULT;
	if (s == "smpte240m") return V4L2_XFER_FUNC_SMPTE240M;
	if (s == "rec709") return V4L2_XFER_FUNC_709;
	if (s == "srgb") return V4L2_XFER_FUNC_SRGB;
	if (s == "oprgb") return V4L2_XFER_FUNC_OPRGB;
	if (s == "dcip3") return V4L2_XFER_FUNC_DCI_P3;
	if (s == "smpte2084") return V4L2_XFER_FUNC_SMPTE2084;
	if (s == "none") return V4L2_XFER_FUNC_NONE;
	return 0;
}

static __u32 parse_ycbcr(const QString &s)
{
	if (s == "default") return V4L2_YCBCR_ENC_DEFAULT;
	if (s == "601") return V4L2_YCBCR_ENC_601;
	if (s == "709") return V4L2_YCBCR_ENC_709;
	if (s == "xv601") return V4L2_YCBCR_ENC_XV601;
	if (s == "xv709") return V4L2_YCBCR_ENC_XV709;
	if (s == "bt2020") return V4L2_YCBCR_ENC_BT2020;
	if (s == "bt2020c") return V4L2_YCBCR_ENC_BT2020_CONST_LUM;
	if (s == "smpte240m") return V4L2_YCBCR_ENC_SMPTE240M;
	return V4L2_YCBCR_ENC_DEFAULT;
}

static __u32 parse_hsv(const QString &s)
{
	if (s == "default") return V4L2_YCBCR_ENC_DEFAULT;
	if (s == "180") return V4L2_HSV_ENC_180;
	if (s == "256") return V4L2_HSV_ENC_256;
	return V4L2_YCBCR_ENC_DEFAULT;
}

static __u32 parse_quantization(const QString &s)
{
	if (s == "default") return V4L2_QUANTIZATION_DEFAULT;
	if (s == "full-range") return V4L2_QUANTIZATION_FULL_RANGE;
	if (s == "lim-range") return V4L2_QUANTIZATION_LIM_RANGE;
	return V4L2_QUANTIZATION_DEFAULT;
}

static __u32 parse_pixel_format(const QString &s)
{
	for (unsigned i = 0; formats[i]; i++)
		if (s == fcc2s(formats[i]).c_str())
			return formats[i];
	return 0;
}

static void list_formats()
{
	for (unsigned i = 0; formats[i]; i++)
		printf("'%s':\r\t\t%s\n", fcc2s(formats[i]).c_str(),
		       pixfmt2s(formats[i]).c_str());
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
	QScrollArea *sa = new QScrollArea; // Automatically freed on window close
	QSurfaceFormat format;
	QString video_device = "0";
	QString filename;
	cv4l_fd fd;
	cv4l_fmt fmt;
	unsigned v4l2_bufs = 4;
	bool info_option = false;
	bool report_timings = false;
	bool verbose = false;
	__u32 overrideColorspace = 0xffffffff;
	__u32 overrideYCbCrEnc = 0xffffffff;
	__u32 overrideHSVEnc = 0xffffffff;
	__u32 overrideXferFunc = 0xffffffff;
	__u32 overrideQuantization = 0xffffffff;
	bool force_opengl = false;
	bool force_opengles = false;

	disp.setApplicationDisplayName("Ragna Viewer");
	QStringList args = disp.arguments();
	for (int i = 1; i < args.size(); i++) {
		QString s;

		if (isOptArg(args[i], "--device", "-d")) {
			if (!processOption(args, i, video_device))
				return 0;
		} else if (isOptArg(args[i], "--colorspace", "-C")) {
			if (!processOption(args, i, s))
				return 0;
			overrideColorspace = parse_colorspace(s);
		} else if (isOptArg(args[i], "--ycbcr-enc", "-Y")) {
			if (!processOption(args, i, s))
				return 0;
			overrideYCbCrEnc = parse_ycbcr(s);
		} else if (isOptArg(args[i], "--hsv-enc", "-H")) {
			if (!processOption(args, i, s))
				return 0;
			overrideHSVEnc = parse_hsv(s);
		} else if (isOptArg(args[i], "--xfer-func", "-X")) {
			if (!processOption(args, i, s))
				return 0;
			overrideXferFunc = parse_xfer_func(s);
		} else if (isOptArg(args[i], "--quant", "-Q")) {
			if (!processOption(args, i, s))
				return 0;
			overrideQuantization = parse_quantization(s);
		} else if (isOption(args[i], "--help", "-h")) {
			usage();
			info_option = true;
		} else if (isOption(args[i], "--list-formats", "-l")) {
			list_formats();
			info_option = true;
		} else if (isOption(args[i], "--timings", "-t")) {
			report_timings = true;
		} else if (isOptArg(args[i], "--opengles")) {
			force_opengles = true;
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
			fprintf(stderr, "Device has invalid/unknown format: '%s' %s\n",
				fcc2s(pf).c_str(), pixfmt2s(pf).c_str());
			std::exit(EXIT_FAILURE);
		}
	}

	format.setDepthBufferSize(24);

	if (force_opengles)
		format.setRenderableType(QSurfaceFormat::OpenGLES);
	else if (force_opengl)
		format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);

	QSurfaceFormat::setDefaultFormat(format);
	CaptureWin win(sa);
	win.setVerbose(verbose);
	win.setModeV4L2(&fd);
	win.setFormat(format);
	win.setReportTimings(report_timings);
	win.setOverrideColorspace(overrideColorspace);
	win.setOverrideYCbCrEnc(overrideYCbCrEnc);
	win.setOverrideHSVEnc(overrideHSVEnc);
	win.setOverrideXferFunc(overrideXferFunc);
	win.setOverrideQuantization(overrideQuantization);
	while (!win.setV4LFormat(fmt)) {
		fprintf(stderr, "Unsupported format: '%s' %s\n",
			fcc2s(fmt.g_pixelformat()).c_str(),
			pixfmt2s(fmt.g_pixelformat()).c_str());
		std::exit(EXIT_FAILURE);
	}
	win.setMinimumSize(16, 16);
	win.setSizeIncrement(2, 2);
	win.setFocusPolicy(Qt::StrongFocus);

	sa->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	sa->setWidget(win.window());
	sa->setFrameShape(QFrame::NoFrame);
	sa->resize(QSize(fmt.g_width(), fmt.g_frame_height()));
	sa->setWidgetResizable(true);

	cv4l_queue q(fd.g_type(), V4L2_MEMORY_MMAP);
	q.reqbufs(&fd, v4l2_bufs);
	q.obtain_bufs(&fd);
	q.queue_all(&fd);
	win.setQueue(&q);
	if (fd.streamon()) {
		fputs("Error initializing the stream. Stopping.\n", stderr);
		std::exit(EXIT_FAILURE);
	}

	RagnaController rc(&win, sa);

	disp.setStyleSheet(loadFile(":/style.css"));

	rc.start();

	return disp.exec();
}
