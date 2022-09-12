#include <QDir>
#include <QJsonDocument>

#include "ragnaconfigwindow.h"
#include "ragnacontroller.h"
#include "v4l2-info.h"

#define RAGNA_DIR ".config/ragna/"
#define RAGNA_JSON_PATH (QDir::homePath() + ("/" RAGNA_DIR "ragna.json"))

void RagnaController::onShowConfigWindow()
{
    m_configWindow->show();
    m_configWindow->raise();
    m_configWindow->activateWindow();
}

void RagnaController::readPrefs(QJsonObject o)
{
    m_prefs.read_colorspace(o["colorspace"].toString().toLower());
    m_prefs.read_quantization(o["quantization"].toString().toLower());
    m_prefs.read_xfer_func(o["xfer_func"].toString().toLower());
    m_prefs.read_ycbcr(o["ycbcr"].toString().toLower());
}

void RagnaController::savePrefs(QJsonObject &o)
{
    m_captureWin->saveToPrefs(&m_prefs);

    o["colorspace"] = RagnaPrefs::colorspace2s(m_prefs.colorspace);
    o["quantization"] = RagnaPrefs::quantization2s(m_prefs.quantization);
    o["xfer_func"] = RagnaPrefs::xfer_func2s(m_prefs.xfer_func);
    o["ycbcr"] = RagnaPrefs::ycbcr2s(m_prefs.ycbcr_enc);
}

void RagnaController::updateFormatForPrefs(cv4l_fmt *fmt)
{
    if (m_prefs.colorspace != V4L2_COLORSPACE_DEFAULT)
        fmt->s_colorspace(m_prefs.colorspace);
    if (m_prefs.quantization != V4L2_QUANTIZATION_DEFAULT)
        fmt->s_quantization(m_prefs.quantization);
    if (m_prefs.xfer_func != V4L2_XFER_FUNC_DEFAULT)
        fmt->s_xfer_func(m_prefs.colorspace);
    if (m_prefs.ycbcr_enc != V4L2_YCBCR_ENC_DEFAULT)
        fmt->s_ycbcr_enc(m_prefs.ycbcr_enc);

    m_v4l_fmt = fmt;
}

void RagnaController::loadPrefs()
{
    QFile f(RAGNA_JSON_PATH);

    if (f.open(QIODevice::ReadOnly) == false)
        return;

    QByteArray ba = f.readAll();
    QJsonDocument d(QJsonDocument::fromJson(ba));

    readPrefs(d.object());
    f.close();
}

void RagnaController::onCaptureClosing()
{
    m_configWindow->close();

    QFile f(RAGNA_JSON_PATH);
    QJsonObject o;

    if (f.open(QIODevice::WriteOnly) == false) {
        /* Maybe the config dir hasn't been made yet. */
        QDir configDir = QDir(QDir::homePath());

        if (configDir.exists(RAGNA_DIR) == true ||
            configDir.mkdir(RAGNA_DIR) == false ||
            f.open(QIODevice::WriteOnly) == false)
            /* Out of ideas. Give up on saving prefs. */
            return;
    }

    savePrefs(o);
    f.write(QJsonDocument(o).toJson());
    f.close();
}

void RagnaController::setCapture(CaptureWin *win, RagnaScrollArea *rsa)
{
    m_captureWin = win;
    m_captureArea = rsa;
    win->loadFromPrefs(&m_prefs);

    m_configWindow = new RagnaConfigWindow(win);

    win->setMinimumSize(16, 16);
    win->setSizeIncrement(2, 2);
    win->setFocusPolicy(Qt::StrongFocus);

    rsa->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    rsa->setWidget(win->window());
    rsa->setFrameShape(QFrame::NoFrame);
    rsa->setWidgetResizable(true);
}

void RagnaController::start()
{
    connect(m_captureWin, &CaptureWin::showConfigWindow,
            this, &RagnaController::onShowConfigWindow);

    connect(m_captureArea, &RagnaScrollArea::closing,
            this, &RagnaController::onCaptureClosing);

    m_captureArea->show();
}
