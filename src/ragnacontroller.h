#ifndef RAGNACONTROLLER_H
# define RAGNACONTROLLER_H
# include <QScrollArea>

# include "capture.h"

class RagnaConfigWindow;

class RagnaController : public QObject
{
    Q_OBJECT

public:
    RagnaController(CaptureWin *, QScrollArea *);

    void start();

private slots:
    void onShowConfigWindow();

private:
    RagnaConfigWindow *m_configWindow;
    CaptureWin *m_captureWin;
    QScrollArea *m_captureArea;
};

#endif
