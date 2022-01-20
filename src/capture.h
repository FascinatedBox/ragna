/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2018 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#ifndef CAPTURE_WIN_GL_H
#define CAPTURE_WIN_GL_H

#define GL_GLEXT_PROTOTYPES 1
#define QT_NO_OPENGL_ES_2

#include <QKeyEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QScrollArea>

#include "cv4l-helpers.h"

extern const __u32 formats[];
extern const __u32 colorspaces[];
extern const __u32 xfer_funcs[];
extern const __u32 ycbcr_encs[];
extern const __u32 hsv_encs[];
extern const __u32 quantizations[];

class QOpenGLPaintDevice;

// This must be equal to the max number of textures that any shader uses
#define MAX_TEXTURES_NEEDED 3

class CaptureWin : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	explicit CaptureWin(QScrollArea *sa, QWidget *parent = 0);
	~CaptureWin();

	void setModeV4L2(cv4l_fd *fd);
	void setQueue(cv4l_queue *q);
	bool setV4LFormat(cv4l_fmt &fmt);
	void setReportTimings(bool report) { m_reportTimings = report; }
	void setVerbose(bool verbose) { m_verbose = verbose; }

	cv4l_fmt getFmt() { return m_v4l_fmt; }

	__u32 getColorspace() { return m_overrideColorspace; }
	__u32 getYcbcrEnc() { return m_overrideYCbCrEnc; }
	__u32 getXferFunc() { return m_overrideXferFunc; }
	__u32 getQuantization() { return m_overrideQuantization; }

signals:
	void closing();
	void showConfigWindow();

public slots:
	void updateColorspace(int);
	void updateYcbcrEnc(int);
	void updateXferFunc(int);
	void updateQuantization(int);

private slots:
	void v4l2ReadEvent();
	void v4l2ExceptionEvent();

	void restoreAll(bool checked);
	void restoreSize(bool checked = false);
	void toggleFullScreen(bool b = false);

private:
	bool updateV4LFormat(const cv4l_fmt &fmt);
	void resizeEvent(QResizeEvent *event);
	void focusInEvent(QFocusEvent *event);
	void focusOutEvent(QFocusEvent *event);
	void paintGL();
	void initializeGL();
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent * e);
	void showCurrentOverrides();

	bool supportedFmt(__u32 fmt);
	void checkError(const char *msg);
	void configureTexture(size_t idx);
	void updateOrigValues();
	void updateShader();
	void changeShader();

	// Colorspace conversion shaders
	void shader_YUV();
	void shader_NV12();
	void shader_NV16();
	void shader_NV24();
	void shader_RGB();
	void shader_Bayer();
	void shader_YUV_packed();
	void shader_YUY2();

	// Colorspace conversion render
	void render_RGB(__u32 format);
	void render_Bayer(__u32 format);
	void render_YUY2(__u32 format);
	void render_YUV(__u32 format);
	void render_YUV_packed(__u32 format);
	void render_NV12(__u32 format);
	void render_NV16(__u32 format);
	void render_NV24(__u32 format);

	cv4l_fd *m_fd;
	cv4l_fmt m_v4l_fmt;
	cv4l_queue *m_v4l_queue;
	bool m_verbose;
	bool m_reportTimings;
	bool m_is_rgb;
	bool m_is_hsv;
	bool m_is_bayer;
	bool m_uses_gl_red;
	bool m_accepts_srgb;
	bool m_haveSwapBytes;
	bool m_updateShader;
	QSize m_viewSize;

	__u32 m_overrideColorspace;
	__u32 m_overrideYCbCrEnc;
	__u32 m_overrideHSVEnc;
	__u32 m_overrideXferFunc;
	__u32 m_overrideQuantization;
	__u32 m_origPixelFormat;
	__u32 m_origWidth;
	__u32 m_origHeight;
	__u32 m_origField;
	__u32 m_origColorspace;
	__u32 m_origYCbCrEnc;
	__u32 m_origHSVEnc;
	__u32 m_origXferFunc;
	__u32 m_origQuantization;

	int m_screenTextureCount;
	GLuint m_screenTexture[MAX_TEXTURES_NEEDED];
	QOpenGLShaderProgram *m_program;
	__u8 *m_curData[MAX_TEXTURES_NEEDED];
	unsigned m_curSize[MAX_TEXTURES_NEEDED];
	__u8 *m_nextData[MAX_TEXTURES_NEEDED];
	unsigned m_nextSize[MAX_TEXTURES_NEEDED];
	int m_curIndex;
	int m_nextIndex;

	QScrollArea *m_scrollArea;
	QAction *m_resolutionOverride;
	QAction *m_exitFullScreen;
	QAction *m_enterFullScreen;
};

#endif
