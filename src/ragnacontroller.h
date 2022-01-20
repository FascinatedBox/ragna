#ifndef RAGNACONTROLLER_H
# define RAGNACONTROLLER_H
# include <QJsonObject>
# include <QScrollArea>

# include "capture.h"
# include "ragnaprefs.h"
# include "cv4l-helpers.h"

class RagnaConfigWindow;

class RagnaController : public QObject
{
    Q_OBJECT

public:
    RagnaController() {};

    void loadPrefs();
    void setCapture(CaptureWin *, QScrollArea *);
    void start();
    void updateFormatForPrefs(cv4l_fmt *);

private slots:
    void onCaptureClosing();
    void onShowConfigWindow();

private:
    void readPrefs(QJsonObject);
    void savePrefs(QJsonObject &);

    RagnaConfigWindow *m_configWindow;
    CaptureWin *m_captureWin;
    QScrollArea *m_captureArea;
    cv4l_fmt *m_v4l_fmt;
    RagnaPrefs m_prefs;
};

#endif
