#include <QFormLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include "ragnaconfigcombobox.h"
#include "ragnaconfigwindow.h"
#include "ragnaprefs.h"
#include "v4l2-info.h"

#define ADD_SECTION_ITEM(name) \
{ \
    QListWidgetItem *item = new QListWidgetItem(); \
 \
    item->setText(#name); \
    m_stack->addWidget(create##name##Section()); \
    configListWidget->addItem(item); \
}

RagnaConfigWindow::RagnaConfigWindow(CaptureWin *win, RagnaPrefs *prefs)
    : m_captureWin(win),
      m_prefs(prefs)
{
    QGridLayout *layout = new QGridLayout;
    QListWidget *configListWidget = new QListWidget;
    QListWidgetItem *item = new QListWidgetItem;
    m_stack = new QStackedWidget;

    ADD_SECTION_ITEM(Color)

    configListWidget->setCurrentRow(0);

    layout->addWidget(configListWidget, 0, 0);
    layout->addWidget(m_stack, 0, 1);

    connect(configListWidget, &QListWidget::currentRowChanged,
            this, &RagnaConfigWindow::onListRowChanged);

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
                      m_prefs->base_name, \
                      base_name##2s); \
 \
layout->addRow(desc, base_name##ComboBox); \
connect(base_name##ComboBox, &RagnaConfigComboBox::valueChanged, \
        this, &RagnaConfigWindow::onUpdate##fn);

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

void RagnaConfigWindow::onListRowChanged(int row)
{
    m_stack->setCurrentIndex(row);
}

void RagnaConfigWindow::onUpdateColorspace(int data)
{
    m_prefs->colorspace = data;

    m_captureWin->syncPrefsColor();
}

void RagnaConfigWindow::onUpdateQuantization(int data)
{
    m_prefs->quantization = data;
    m_captureWin->syncPrefsColor();
}

void RagnaConfigWindow::onUpdateXferFunc(int data)
{
    m_prefs->xfer_func = data;
    m_captureWin->syncPrefsColor();
}

void RagnaConfigWindow::onUpdateYcbcrEnc(int data)
{
    m_prefs->ycbcr_enc = data;
    m_captureWin->syncPrefsColor();
}
