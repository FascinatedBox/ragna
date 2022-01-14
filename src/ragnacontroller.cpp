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

    win->setMinimumSize(16, 16);
    win->setSizeIncrement(2, 2);
    win->setFocusPolicy(Qt::StrongFocus);

    sa->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    sa->setWidget(win->window());
    sa->setFrameShape(QFrame::NoFrame);
    sa->setWidgetResizable(true);
}

void RagnaController::start()
{
    connect(m_captureWin, &CaptureWin::showConfigWindow,
            this, &RagnaController::onShowConfigWindow);

    m_captureArea->show();
}
