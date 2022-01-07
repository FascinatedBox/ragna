#ifndef RAGNACONFIGCOMBOBOX_H
# define RAGNACONFIGCOMBOBOX_H
# include <QComboBox>

class RagnaConfigComboBox : public QComboBox
{
    Q_OBJECT

public:
    RagnaConfigComboBox();

signals:
    void valueChanged(int);

private slots:
    void onActivated(int);
};

#endif
