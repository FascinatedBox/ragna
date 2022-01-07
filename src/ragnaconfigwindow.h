#ifndef RAGNACONFIGWINDOW_H
# define RAGNACONFIGWINDOW_H
# include <QDialog>

# include "capture.h"

class RagnaConfigComboBox;

class RagnaConfigWindow : public QDialog
{
    Q_OBJECT

public:
    RagnaConfigWindow(CaptureWin *);

private:
    RagnaConfigComboBox *newConfigComboBox(
        const __u32 *,
        int,
        std::string(*)(int)
    );
    QWidget *createColorSection();

    CaptureWin *m_captureWin;
};

#endif
