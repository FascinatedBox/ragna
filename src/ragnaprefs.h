#ifndef RAGNAPREFS_H
# define RAGNAPREFS_H
# include <QObject>
# include <QString>

class RagnaPrefs : public QObject
{
    Q_OBJECT

public:
    RagnaPrefs() {};

    static const char *colorspace2s(__u32);
    static const char *quantization2s(__u32);
    static const char *xfer_func2s(__u32);
    static const char *ycbcr2s(__u32);

    void read_colorspace(QString);
    void read_quantization(QString);
    void read_xfer_func(QString);
    void read_ycbcr(QString);

    __u32 colorspace;
    __u32 xfer_func;
    __u32 ycbcr_enc;
    __u32 quantization;
};

#endif
