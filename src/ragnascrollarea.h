#ifndef RAGNASCROLLAREA_H
# define RAGNASCROLLAREA_H
# include <QCloseEvent>
# include <QScrollArea>

class RagnaScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    RagnaScrollArea();

signals:
    void closing();

private:
    void closeEvent(QCloseEvent *);
};

#endif
