// SPDX-License-Identifier: GPL-2.0-only
/*
 * The YUY2 shader code is based on face-responder. The code is under public domain:
 * https://bitbucket.org/nateharward/face-responder/src/0c3b4b957039d9f4bf1da09b9471371942de2601/yuv42201_laplace.frag?at=master
 *
 * All other OpenGL code:
 *
 * Copyright 2018 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#include <QApplication>
#include <QMenu>
#include <QSocketNotifier>
#include <QtMath>

#include "capture.h"
#include "v4l2-info.h"

const __u32 formats[] = {
	V4L2_PIX_FMT_YUYV,
	V4L2_PIX_FMT_YVYU,
	V4L2_PIX_FMT_UYVY,
	V4L2_PIX_FMT_VYUY,
	V4L2_PIX_FMT_YUV422P,
	V4L2_PIX_FMT_YVU420,
	V4L2_PIX_FMT_YUV420,
	V4L2_PIX_FMT_NV12,
	V4L2_PIX_FMT_NV21,
	V4L2_PIX_FMT_NV16,
	V4L2_PIX_FMT_NV61,
	V4L2_PIX_FMT_NV24,
	V4L2_PIX_FMT_NV42,
	V4L2_PIX_FMT_NV16M,
	V4L2_PIX_FMT_NV61M,
	V4L2_PIX_FMT_YVU420M,
	V4L2_PIX_FMT_YUV420M,
	V4L2_PIX_FMT_YVU422M,
	V4L2_PIX_FMT_YUV422M,
	V4L2_PIX_FMT_YVU444M,
	V4L2_PIX_FMT_YUV444M,
	V4L2_PIX_FMT_NV12M,
	V4L2_PIX_FMT_NV21M,
	V4L2_PIX_FMT_YUV444,
	V4L2_PIX_FMT_YUV555,
	V4L2_PIX_FMT_YUV565,
	V4L2_PIX_FMT_YUV32,
	V4L2_PIX_FMT_AYUV32,
	V4L2_PIX_FMT_XYUV32,
	V4L2_PIX_FMT_VUYA32,
	V4L2_PIX_FMT_VUYX32,
	V4L2_PIX_FMT_RGB32,
	V4L2_PIX_FMT_XRGB32,
	V4L2_PIX_FMT_ARGB32,
	V4L2_PIX_FMT_RGBX32,
	V4L2_PIX_FMT_RGBA32,
	V4L2_PIX_FMT_BGR32,
	V4L2_PIX_FMT_XBGR32,
	V4L2_PIX_FMT_ABGR32,
	V4L2_PIX_FMT_BGRX32,
	V4L2_PIX_FMT_BGRA32,
	V4L2_PIX_FMT_RGB24,
	V4L2_PIX_FMT_BGR24,
	V4L2_PIX_FMT_RGB565,
	V4L2_PIX_FMT_RGB565X,
	V4L2_PIX_FMT_RGB444,
	V4L2_PIX_FMT_XRGB444,
	V4L2_PIX_FMT_ARGB444,
	V4L2_PIX_FMT_XBGR444,
	V4L2_PIX_FMT_ABGR444,
	V4L2_PIX_FMT_RGBX444,
	V4L2_PIX_FMT_RGBA444,
	V4L2_PIX_FMT_BGRX444,
	V4L2_PIX_FMT_BGRA444,
	V4L2_PIX_FMT_RGB555,
	V4L2_PIX_FMT_XRGB555,
	V4L2_PIX_FMT_ARGB555,
	V4L2_PIX_FMT_RGB555X,
	V4L2_PIX_FMT_XRGB555X,
	V4L2_PIX_FMT_ARGB555X,
	V4L2_PIX_FMT_RGBX555,
	V4L2_PIX_FMT_RGBA555,
	V4L2_PIX_FMT_XBGR555,
	V4L2_PIX_FMT_ABGR555,
	V4L2_PIX_FMT_BGRX555,
	V4L2_PIX_FMT_BGRA555,
	V4L2_PIX_FMT_RGB332,
	V4L2_PIX_FMT_BGR666,
	V4L2_PIX_FMT_SBGGR8,
	V4L2_PIX_FMT_SGBRG8,
	V4L2_PIX_FMT_SGRBG8,
	V4L2_PIX_FMT_SRGGB8,
	V4L2_PIX_FMT_SBGGR10,
	V4L2_PIX_FMT_SGBRG10,
	V4L2_PIX_FMT_SGRBG10,
	V4L2_PIX_FMT_SRGGB10,
	V4L2_PIX_FMT_SBGGR12,
	V4L2_PIX_FMT_SGBRG12,
	V4L2_PIX_FMT_SGRBG12,
	V4L2_PIX_FMT_SRGGB12,
	V4L2_PIX_FMT_SBGGR16,
	V4L2_PIX_FMT_SGBRG16,
	V4L2_PIX_FMT_SGRBG16,
	V4L2_PIX_FMT_SRGGB16,
	V4L2_PIX_FMT_HSV24,
	V4L2_PIX_FMT_HSV32,
	V4L2_PIX_FMT_GREY,
	V4L2_PIX_FMT_Y10,
	V4L2_PIX_FMT_Y12,
	V4L2_PIX_FMT_Y16,
	V4L2_PIX_FMT_Y16_BE,
	V4L2_PIX_FMT_Z16,
	0
};

const __u32 fields[] = {
	V4L2_FIELD_NONE,
	V4L2_FIELD_TOP,
	V4L2_FIELD_BOTTOM,
	V4L2_FIELD_INTERLACED,
	V4L2_FIELD_SEQ_TB,
	V4L2_FIELD_SEQ_BT,
	V4L2_FIELD_ALTERNATE,
	V4L2_FIELD_INTERLACED_TB,
	V4L2_FIELD_INTERLACED_BT,
	0
};

const __u32 colorspaces[] = {
	V4L2_COLORSPACE_SMPTE170M,
	V4L2_COLORSPACE_SMPTE240M,
	V4L2_COLORSPACE_REC709,
	V4L2_COLORSPACE_470_SYSTEM_M,
	V4L2_COLORSPACE_470_SYSTEM_BG,
	V4L2_COLORSPACE_SRGB,
	V4L2_COLORSPACE_OPRGB,
	V4L2_COLORSPACE_DCI_P3,
	V4L2_COLORSPACE_BT2020,
	0
};

const __u32 xfer_funcs[] = {
	V4L2_XFER_FUNC_709,
	V4L2_XFER_FUNC_SRGB,
	V4L2_XFER_FUNC_OPRGB,
	V4L2_XFER_FUNC_DCI_P3,
	V4L2_XFER_FUNC_SMPTE2084,
	V4L2_XFER_FUNC_SMPTE240M,
	V4L2_XFER_FUNC_NONE,
	0
};

const __u32 ycbcr_encs[] = {
	V4L2_YCBCR_ENC_601,
	V4L2_YCBCR_ENC_709,
	V4L2_YCBCR_ENC_XV601,
	V4L2_YCBCR_ENC_XV709,
	V4L2_YCBCR_ENC_BT2020,
	V4L2_YCBCR_ENC_BT2020_CONST_LUM,
	V4L2_YCBCR_ENC_SMPTE240M,
	0
};

const __u32 hsv_encs[] = {
	V4L2_HSV_ENC_180,
	V4L2_HSV_ENC_256,
	0
};

const __u32 quantizations[] = {
	V4L2_QUANTIZATION_FULL_RANGE,
	V4L2_QUANTIZATION_LIM_RANGE,
	0
};

CaptureWin::CaptureWin(QScrollArea *sa, QWidget *parent) :
	QOpenGLWidget(parent),
	m_fd(0),
	m_v4l_queue(0),
	m_origPixelFormat(0),
	m_screenTextureCount(0),
	m_program(0),
	m_curIndex(-1),
	m_nextIndex(-1),
	m_scrollArea(sa)
{
	m_curSize[0] = 0;
	m_curData[0] = 0;

	m_enterFullScreen = new QAction("Enter fullscreen (F)", this);
	connect(m_enterFullScreen, SIGNAL(triggered(bool)),
		this, SLOT(toggleFullScreen(bool)));

	m_exitFullScreen = new QAction("Exit fullscreen (F or Esc)", this);
	connect(m_exitFullScreen, SIGNAL(triggered(bool)),
		this, SLOT(toggleFullScreen(bool)));
}

CaptureWin::~CaptureWin()
{
	emit closing();
	makeCurrent();
	delete m_program;
}

void CaptureWin::resizeEvent(QResizeEvent *event)
{
	QSize origSize = QSize(m_origWidth, m_origHeight);
	QSize window = size();
	qreal scale;

	if ((qreal)window.width() / (qreal)origSize.width() >
	    (qreal)window.height() / (qreal)origSize.height()) {
		// Horizontal scale factor > vert. scale factor
		scale = (qreal)window.height() / (qreal)origSize.height();
	} else {
		scale = (qreal)window.width() / (qreal)origSize.width();
	}
	m_viewSize = QSize((qreal)m_origWidth * scale, (qreal)m_origHeight * scale);
	QOpenGLWidget::resizeEvent(event);
}

void CaptureWin::focusInEvent(QFocusEvent *event)
{
	event->ignore();
}

void CaptureWin::focusOutEvent(QFocusEvent *event)
{
	event->ignore();
}

void CaptureWin::updateShader()
{
	setV4LFormat(m_v4l_fmt);
	m_updateShader = true;
}

void CaptureWin::showCurrentOverrides()
{
	static bool firstTime = true;
	const char *prefix = firstTime ? "" : "New ";

	if (firstTime) {
		printf("%sPixel Format: '%s' %s\n", prefix,
		       fcc2s(m_origPixelFormat).c_str(),
		       pixfmt2s(m_origPixelFormat).c_str());
		printf("%sField: %s\n", prefix, field2s(m_origField).c_str());
	}
	printf("%sColorspace: %s\n", prefix, colorspace2s(m_origColorspace).c_str());
	printf("%sTransfer Function: %s\n", prefix, xfer_func2s(m_origXferFunc).c_str());
	if (m_is_hsv)
		printf("%sHSV Encoding: %s\n", prefix, ycbcr_enc2s(m_origHSVEnc).c_str());
	else if (!m_is_rgb)
		printf("%sY'CbCr Encoding: %s\n", prefix, ycbcr_enc2s(m_origYCbCrEnc).c_str());
	printf("%sQuantization: %s\n", prefix, quantization2s(m_origQuantization).c_str());
	firstTime = false;
}

void CaptureWin::restoreAll(bool checked)
{
	m_overrideColorspace = m_origColorspace;
	m_overrideXferFunc = m_origXferFunc;
	m_overrideYCbCrEnc = m_origYCbCrEnc;
	m_overrideHSVEnc = m_origHSVEnc;
	m_overrideQuantization = m_origQuantization;
	showCurrentOverrides();
	restoreSize();
}

void CaptureWin::updateColorspace(int data)
{
	m_overrideColorspace = data;
	updateShader();
}

void CaptureWin::updateYcbcrEnc(int data)
{
	m_overrideYCbCrEnc = data;
	updateShader();
}

void CaptureWin::updateXferFunc(int data)
{
	m_overrideXferFunc = data;
	updateShader();
}

void CaptureWin::updateQuantization(int data)
{
	m_overrideQuantization = data;
	updateShader();
}

void CaptureWin::restoreSize(bool)
{
	QSize s = QSize(m_origWidth, m_origHeight);

	m_scrollArea->resize(s);
	resize(s);
	updateShader();
}

void CaptureWin::toggleFullScreen(bool)
{
	if (m_scrollArea->isFullScreen())
		m_scrollArea->showNormal();
	else
		m_scrollArea->showFullScreen();
}

void CaptureWin::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);

	QAction *act = menu.addAction("Restore All");
	connect(act, SIGNAL(triggered(bool)), this, SLOT(restoreAll(bool)));

	act = menu.addAction("Reset window");
	connect(act, SIGNAL(triggered(bool)), this, SLOT(restoreSize(bool)));

	if (m_scrollArea->isFullScreen())
		menu.addAction(m_exitFullScreen);
	else
		menu.addAction(m_enterFullScreen);

	menu.exec(event->globalPos());
}

void CaptureWin::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Escape:
		if (!m_scrollArea->isFullScreen())
			return;
	case Qt::Key_M:
		emit showConfigWindow();
		return;
	case Qt::Key_F:
		toggleFullScreen();
		return;
	default:
		QOpenGLWidget::keyPressEvent(event);
		return;
	}
}

bool CaptureWin::supportedFmt(__u32 fmt)
{
	switch (fmt) {
	case V4L2_PIX_FMT_RGB565X:
	case V4L2_PIX_FMT_Y16_BE:
		return m_haveSwapBytes;

	/*
	 * Note for RGB555(X) formats:
	 * openGL ES doesn't support GL_UNSIGNED_SHORT_1_5_5_5_REV
	 */
	case V4L2_PIX_FMT_RGB555:
	case V4L2_PIX_FMT_XRGB555:
	case V4L2_PIX_FMT_ARGB555:
	case V4L2_PIX_FMT_RGB555X:
	case V4L2_PIX_FMT_XRGB555X:
	case V4L2_PIX_FMT_ARGB555X:
	case V4L2_PIX_FMT_RGBX555:
	case V4L2_PIX_FMT_RGBA555:
	case V4L2_PIX_FMT_XBGR555:
	case V4L2_PIX_FMT_ABGR555:
	case V4L2_PIX_FMT_BGRX555:
	case V4L2_PIX_FMT_BGRA555:
	case V4L2_PIX_FMT_YUV555:
	case V4L2_PIX_FMT_RGB332:
	case V4L2_PIX_FMT_BGR666:
		return !context()->isOpenGLES();
	}

	return true;
}

void CaptureWin::checkError(const char *msg)
{
	int err;
	unsigned errNo = 0;

	while ((err = glGetError()) != GL_NO_ERROR)
		fprintf(stderr, "OpenGL Error (no: %u) code 0x%x: %s.\n", errNo++, err, msg);

	if (errNo)
		std::exit(errNo);
}

void CaptureWin::configureTexture(size_t idx)
{
	glBindTexture(GL_TEXTURE_2D, m_screenTexture[idx]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void CaptureWin::setModeV4L2(cv4l_fd *fd)
{
	m_fd = fd;
	QSocketNotifier *readSock = new QSocketNotifier(fd->g_fd(),
		QSocketNotifier::Read, this);
	QSocketNotifier *excepSock = new QSocketNotifier(fd->g_fd(),
		QSocketNotifier::Exception, this);

	v4l2_event_subscription sub = { };

	sub.type = V4L2_EVENT_SOURCE_CHANGE;
	m_fd->subscribe_event(sub);
	connect(readSock, SIGNAL(activated(int)), this, SLOT(v4l2ReadEvent()));
	connect(excepSock, SIGNAL(activated(int)), this, SLOT(v4l2ExceptionEvent()));

	if (m_verbose && m_fd->g_direct())
		printf("using libv4l2\n");
}

void CaptureWin::setQueue(cv4l_queue *q)
{
	m_v4l_queue = q;
	if (m_origPixelFormat == 0)
		updateOrigValues();
}

bool CaptureWin::updateV4LFormat(const cv4l_fmt &fmt)
{
	m_is_rgb = true;
	m_is_hsv = false;
	m_uses_gl_red = false;
	m_accepts_srgb = true;
	m_is_bayer = false;

	switch (fmt.g_pixelformat()) {
	case V4L2_PIX_FMT_YUV422P:
	case V4L2_PIX_FMT_YUV420:
	case V4L2_PIX_FMT_YVU420:
	case V4L2_PIX_FMT_YUV420M:
	case V4L2_PIX_FMT_YVU420M:
	case V4L2_PIX_FMT_YUV422M:
	case V4L2_PIX_FMT_YVU422M:
	case V4L2_PIX_FMT_YUV444M:
	case V4L2_PIX_FMT_YVU444M:
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV16:
	case V4L2_PIX_FMT_NV61:
	case V4L2_PIX_FMT_NV24:
	case V4L2_PIX_FMT_NV42:
	case V4L2_PIX_FMT_NV16M:
	case V4L2_PIX_FMT_NV61M:
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
		m_uses_gl_red = true;
		/* fall through */
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_YVYU:
	case V4L2_PIX_FMT_UYVY:
	case V4L2_PIX_FMT_VYUY:
	case V4L2_PIX_FMT_YUV444:
	case V4L2_PIX_FMT_YUV555:
	case V4L2_PIX_FMT_YUV565:
	case V4L2_PIX_FMT_YUV32:
	case V4L2_PIX_FMT_AYUV32:
	case V4L2_PIX_FMT_XYUV32:
	case V4L2_PIX_FMT_VUYA32:
	case V4L2_PIX_FMT_VUYX32:
		m_is_rgb = false;
		m_accepts_srgb = false;
		break;
	case V4L2_PIX_FMT_HSV24:
	case V4L2_PIX_FMT_HSV32:
		m_is_rgb = false;
		m_is_hsv = true;
		m_accepts_srgb = false;
		break;
	case V4L2_PIX_FMT_SBGGR8:
	case V4L2_PIX_FMT_SGBRG8:
	case V4L2_PIX_FMT_SGRBG8:
	case V4L2_PIX_FMT_SRGGB8:
	case V4L2_PIX_FMT_SBGGR10:
	case V4L2_PIX_FMT_SGBRG10:
	case V4L2_PIX_FMT_SGRBG10:
	case V4L2_PIX_FMT_SRGGB10:
	case V4L2_PIX_FMT_SBGGR12:
	case V4L2_PIX_FMT_SGBRG12:
	case V4L2_PIX_FMT_SGRBG12:
	case V4L2_PIX_FMT_SRGGB12:
	case V4L2_PIX_FMT_SBGGR16:
	case V4L2_PIX_FMT_SGBRG16:
	case V4L2_PIX_FMT_SGRBG16:
	case V4L2_PIX_FMT_SRGGB16:
		m_is_bayer = true;
		/* fall through */
	case V4L2_PIX_FMT_GREY:
	case V4L2_PIX_FMT_Y10:
	case V4L2_PIX_FMT_Y12:
	case V4L2_PIX_FMT_Y16:
	case V4L2_PIX_FMT_Y16_BE:
	case V4L2_PIX_FMT_Z16:
		m_uses_gl_red = true;
		/* fall through */
	case V4L2_PIX_FMT_BGR666:
	case V4L2_PIX_FMT_RGB565:
	case V4L2_PIX_FMT_RGB565X:
	case V4L2_PIX_FMT_RGB444:
	case V4L2_PIX_FMT_XRGB444:
	case V4L2_PIX_FMT_ARGB444:
	case V4L2_PIX_FMT_XBGR444:
	case V4L2_PIX_FMT_ABGR444:
	case V4L2_PIX_FMT_RGBX444:
	case V4L2_PIX_FMT_RGBA444:
	case V4L2_PIX_FMT_BGRX444:
	case V4L2_PIX_FMT_BGRA444:
	case V4L2_PIX_FMT_RGB32:
	case V4L2_PIX_FMT_XRGB32:
	case V4L2_PIX_FMT_ARGB32:
	case V4L2_PIX_FMT_RGBX32:
	case V4L2_PIX_FMT_RGBA32:
	case V4L2_PIX_FMT_BGR32:
	case V4L2_PIX_FMT_XBGR32:
	case V4L2_PIX_FMT_ABGR32:
	case V4L2_PIX_FMT_BGRX32:
	case V4L2_PIX_FMT_BGRA32:
		m_accepts_srgb = false;
		/* fall through */
	case V4L2_PIX_FMT_RGB24:
	case V4L2_PIX_FMT_BGR24:
	case V4L2_PIX_FMT_RGB555:
	case V4L2_PIX_FMT_XRGB555:
	case V4L2_PIX_FMT_ARGB555:
	case V4L2_PIX_FMT_RGB555X:
	case V4L2_PIX_FMT_XRGB555X:
	case V4L2_PIX_FMT_ARGB555X:
	case V4L2_PIX_FMT_RGBX555:
	case V4L2_PIX_FMT_RGBA555:
	case V4L2_PIX_FMT_XBGR555:
	case V4L2_PIX_FMT_ABGR555:
	case V4L2_PIX_FMT_BGRX555:
	case V4L2_PIX_FMT_BGRA555:
	case V4L2_PIX_FMT_RGB332:
		break;
	default:
		return false;
	}
	return true;
}

bool CaptureWin::setV4LFormat(cv4l_fmt &fmt)
{
	if (!updateV4LFormat(fmt))
		return false;

	if (m_overrideColorspace != 0xffffffff)
		fmt.s_colorspace(m_overrideColorspace);
	if (m_is_hsv && m_overrideHSVEnc != 0xffffffff)
		fmt.s_ycbcr_enc(m_overrideHSVEnc);
	else if (!m_is_rgb && m_overrideYCbCrEnc != 0xffffffff)
		fmt.s_ycbcr_enc(m_overrideYCbCrEnc);
	if (m_overrideXferFunc != 0xffffffff)
		fmt.s_xfer_func(m_overrideXferFunc);
	if (m_overrideQuantization != 0xffffffff)
		fmt.s_quantization(m_overrideQuantization);

	m_v4l_fmt = fmt;

	switch (fmt.g_colorspace()) {
	case V4L2_COLORSPACE_SMPTE170M:
	case V4L2_COLORSPACE_SMPTE240M:
	case V4L2_COLORSPACE_REC709:
	case V4L2_COLORSPACE_470_SYSTEM_M:
	case V4L2_COLORSPACE_470_SYSTEM_BG:
	case V4L2_COLORSPACE_SRGB:
	case V4L2_COLORSPACE_OPRGB:
	case V4L2_COLORSPACE_BT2020:
	case V4L2_COLORSPACE_DCI_P3:
		break;
	default:
		// If the colorspace was not specified, then guess
		// based on the pixel format.
		if (m_is_rgb)
			m_v4l_fmt.s_colorspace(V4L2_COLORSPACE_SRGB);
		else
			m_v4l_fmt.s_colorspace(V4L2_COLORSPACE_REC709);
		break;
	}
	if (fmt.g_xfer_func() == V4L2_XFER_FUNC_DEFAULT)
		m_v4l_fmt.s_xfer_func(V4L2_MAP_XFER_FUNC_DEFAULT(m_v4l_fmt.g_colorspace()));
	if (m_is_hsv)
		m_v4l_fmt.s_ycbcr_enc(fmt.g_hsv_enc());
	else if (fmt.g_ycbcr_enc() == V4L2_YCBCR_ENC_DEFAULT)
		m_v4l_fmt.s_ycbcr_enc(V4L2_MAP_YCBCR_ENC_DEFAULT(m_v4l_fmt.g_colorspace()));
	if (fmt.g_quantization() == V4L2_QUANTIZATION_DEFAULT)
		m_v4l_fmt.s_quantization(V4L2_MAP_QUANTIZATION_DEFAULT(m_is_rgb,
				m_v4l_fmt.g_colorspace(), m_v4l_fmt.g_ycbcr_enc()));

	if (m_accepts_srgb &&
	    (m_v4l_fmt.g_quantization() == V4L2_QUANTIZATION_LIM_RANGE ||
	    m_v4l_fmt.g_xfer_func() != V4L2_XFER_FUNC_SRGB)) {
		/* Can't let openGL convert from non-linear to linear */
		m_accepts_srgb = false;
	}

	if (m_verbose) {
		v4l2_fmtdesc fmt;

		strcpy((char *)fmt.description, fcc2s(m_v4l_fmt.g_pixelformat()).c_str());
		if (m_fd) {
			m_fd->enum_fmt(fmt, true);
			while (fmt.pixelformat != m_v4l_fmt.g_pixelformat()) {
				if (m_fd->enum_fmt(fmt))
					break;
			}
		}

		printf("\n");
		printf("Width x Height:    %ux%u\n", m_v4l_fmt.g_width(), m_v4l_fmt.g_height());
		printf("Field:             %s\n", field2s(m_v4l_fmt.g_field()).c_str());
		printf("Pixel Format:      %s ('%s')\n", fmt.description, fcc2s(m_v4l_fmt.g_pixelformat()).c_str());
		printf("Colorspace:        %s\n", colorspace2s(m_v4l_fmt.g_colorspace()).c_str());
		if (m_is_hsv)
			printf("HSV Encoding:      %s\n", ycbcr_enc2s(m_v4l_fmt.g_hsv_enc()).c_str());
		else if (!m_is_rgb)
			printf("Y'CbCr Encoding:   %s\n", ycbcr_enc2s(m_v4l_fmt.g_ycbcr_enc()).c_str());
		printf("Transfer Function: %s\n", xfer_func2s(m_v4l_fmt.g_xfer_func()).c_str());
		printf("Quantization:      %s\n", quantization2s(m_v4l_fmt.g_quantization()).c_str());
		for (unsigned i = 0; i < m_v4l_fmt.g_num_planes(); i++) {
			printf("Plane %d Image Size:     %u\n", i, m_v4l_fmt.g_sizeimage(i));
			printf("Plane %d Bytes per Line: %u\n", i, m_v4l_fmt.g_bytesperline(i));
		}
	}
	return true;
}

void CaptureWin::v4l2ReadEvent()
{
	cv4l_buffer buf(m_fd->g_type());

	if (m_fd->dqbuf(buf))
		return;

	for (unsigned i = 0; i < m_v4l_queue->g_num_planes(); i++) {
		m_nextData[i] = (__u8 *)m_v4l_queue->g_dataptr(buf.g_index(), i);
		m_nextSize[i] = buf.g_bytesused(i);
	}
	int next = m_nextIndex;
	m_nextIndex = buf.g_index();
	if (next != -1) {
		buf.s_index(next);
		m_fd->qbuf(buf);
	}
	update();
}

void CaptureWin::v4l2ExceptionEvent()
{
	v4l2_event ev;
	cv4l_fmt fmt;

	while (m_fd->dqevent(ev) == 0) {
		if (ev.type == V4L2_EVENT_SOURCE_CHANGE) {
			m_fd->g_fmt(fmt);
			if (!setV4LFormat(fmt)) {
				fprintf(stderr, "Unsupported format: '%s' %s\n",
					fcc2s(fmt.g_pixelformat()).c_str(),
					pixfmt2s(fmt.g_pixelformat()).c_str());
				std::exit(EXIT_FAILURE);
			}
			updateOrigValues();
			showCurrentOverrides();

			m_updateShader = true;
		}
	}
}

void CaptureWin::updateOrigValues()
{
	m_origWidth = m_v4l_fmt.g_width();
	m_origHeight = m_v4l_fmt.g_frame_height();
	m_origPixelFormat = m_v4l_fmt.g_pixelformat();
	m_origField = m_v4l_fmt.g_field();
	m_origColorspace = m_v4l_fmt.g_colorspace();
	m_origXferFunc = m_v4l_fmt.g_xfer_func();
	if (m_is_rgb)
		m_origXferFunc = V4L2_YCBCR_ENC_601;
	else if (m_is_hsv)
		m_origHSVEnc = m_v4l_fmt.g_hsv_enc();
	else
		m_origYCbCrEnc = m_v4l_fmt.g_ycbcr_enc();
	m_origQuantization = m_v4l_fmt.g_quantization();
	m_viewSize = QSize(m_origWidth, m_origHeight);
	m_overrideColorspace = m_origColorspace;
	m_overrideXferFunc = m_origXferFunc;
	m_overrideYCbCrEnc = m_origYCbCrEnc;
	m_overrideQuantization = m_origQuantization;
}
