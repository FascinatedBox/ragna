#ifndef RAGNACONFIGWINDOW_H
# define RAGNACONFIGWINDOW_H
# include <QDialog>
# include <QStackedWidget>

# include "capture.h"

class RagnaConfigComboBox;
class RagnaPrefs;

class RagnaConfigWindow : public QDialog
{
    Q_OBJECT

public:
    RagnaConfigWindow(CaptureWin *, RagnaPrefs *);

private slots:
    void onListRowChanged(int);
    void onUpdateColorspace(int);
    void onUpdateYcbcrEnc(int);
    void onUpdateXferFunc(int);
    void onUpdateQuantization(int);

private:
    RagnaConfigComboBox *newConfigComboBox(
        const __u32 *,
        int,
        std::string(*)(int)
    );
    QWidget *createColorSection();

    CaptureWin *m_captureWin;
    RagnaPrefs *m_prefs;
    QStackedWidget *m_stack;
};

#endif
