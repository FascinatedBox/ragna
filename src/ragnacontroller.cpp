#include "ragnaconfigwindow.h"
#include "ragnacontroller.h"
#include "v4l2-info.h"

void RagnaController::onShowConfigWindow()
{
    m_configWindow->show();
}

void RagnaController::setCapture(CaptureWin *win, QScrollArea *sa)
{
    m_captureWin = win;
    m_captureArea = sa;
    m_configWindow = new RagnaConfigWindow(win);
}

void RagnaController::start()
{
    connect(m_captureWin, &CaptureWin::showConfigWindow,
            this, &RagnaController::onShowConfigWindow);

    m_captureArea->show();
}
