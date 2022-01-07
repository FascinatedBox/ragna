#include "ragnaconfigcombobox.h"

RagnaConfigComboBox::RagnaConfigComboBox()
{
    connect(this, QOverload<int>::of(&QComboBox::activated),
            this, &RagnaConfigComboBox::onActivated);
}

void RagnaConfigComboBox::onActivated(int)
{
    emit valueChanged(currentData().toInt());
}
