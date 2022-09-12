#include "ragnascrollarea.h"

RagnaScrollArea::RagnaScrollArea()
{

}

void RagnaScrollArea::closeEvent(QCloseEvent *event)
{
    emit closing();
    event->accept();
}
