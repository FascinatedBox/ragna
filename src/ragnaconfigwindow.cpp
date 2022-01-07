#include <QFormLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include "ragnaconfigcombobox.h"
#include "ragnaconfigwindow.h"
#include "v4l2-info.h"

RagnaConfigWindow::RagnaConfigWindow(CaptureWin *win)
    : m_captureWin(win)
{
    QGridLayout *layout = new QGridLayout;
    QListWidget *configListWidget = new QListWidget;
    QListWidgetItem *item = new QListWidgetItem;
    QStackedWidget *stack = new QStackedWidget;

    item->setText("Color");
    stack->addWidget(createColorSection());
    configListWidget->addItem(item);

    configListWidget->setCurrentItem(item);

    layout->addWidget(configListWidget, 0, 0);
    layout->addWidget(stack, 0, 1);

    setObjectName("configWindow");
    setLayout(layout);
    setWindowTitle("Settings");
}

RagnaConfigComboBox *RagnaConfigWindow::newConfigComboBox(
        const __u32 *values,
        int currentValue,
        std::string(*to_s)(int))
{
    RagnaConfigComboBox *c = new RagnaConfigComboBox;
    int indexForCurrent = 0;

    for (int i = 0;values[i];i++) {
        int v = values[i];
        QString s = QString::fromStdString(to_s(v));

        if (v == currentValue)
            indexForCurrent = i;

        c->addItem(s, v);
    }

    c->setCurrentIndex(indexForCurrent);

    return c;
}

#define addConfigComboBoxRow(desc, fn, base_name) \
RagnaConfigComboBox *base_name##ComboBox = \
    newConfigComboBox(base_name##s, \
                      (int)m_captureWin->get##fn(), \
                      base_name##2s); \
 \
layout->addRow(desc, base_name##ComboBox); \
connect(base_name##ComboBox, &RagnaConfigComboBox::valueChanged, \
        m_captureWin, &CaptureWin::update##fn);

QWidget *RagnaConfigWindow::createColorSection()
{
    QWidget *w = new QWidget;
    w->setObjectName("configColor");

    QFormLayout *layout = new QFormLayout;

    addConfigComboBoxRow("Colorspace", Colorspace, colorspace)
    addConfigComboBoxRow("Quantization", Quantization, quantization)
    addConfigComboBoxRow("Transfer Func", XferFunc, xfer_func)
    addConfigComboBoxRow("Y'CbCr Encoding", YcbcrEnc, ycbcr_enc)

    w->setLayout(layout);

    return w;
}
