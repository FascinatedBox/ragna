#include "v4l2-info.h"
#include "ragnaprefs.h"

typedef struct {
    __u32 code;
    const char *str;
} table_entry;

static table_entry colorspace_table[] = {
    {V4L2_COLORSPACE_SMPTE170M,     "smpte170m"},
    {V4L2_COLORSPACE_SMPTE240M,     "smpte240m"},
    {V4L2_COLORSPACE_REC709,        "rec709"},
    {V4L2_COLORSPACE_470_SYSTEM_M,  "470m"},
    {V4L2_COLORSPACE_470_SYSTEM_BG, "470bg"},
    {V4L2_COLORSPACE_JPEG,          "jpeg"},
    {V4L2_COLORSPACE_SRGB,          "srgb"},
    {V4L2_COLORSPACE_OPRGB,         "oprgb"},
    {V4L2_COLORSPACE_BT2020,        "bt2020"},
    {V4L2_COLORSPACE_DCI_P3,        "dcip3"},
    {0,                             NULL},
};

static table_entry xfer_func_table[] = {
    {V4L2_XFER_FUNC_SMPTE240M, "smpte240m"},
    {V4L2_XFER_FUNC_709,       "rec709"},
    {V4L2_XFER_FUNC_SRGB,      "srgb"},
    {V4L2_XFER_FUNC_OPRGB,     "oprgb"},
    {V4L2_XFER_FUNC_DCI_P3,    "dcip3"},
    {V4L2_XFER_FUNC_SMPTE2084, "smpte2084"},
    {V4L2_XFER_FUNC_NONE,      "none"},
    {0,                        NULL},
};

static table_entry ycbcr_enc_table[] = {
    {V4L2_YCBCR_ENC_601,              "601"},
    {V4L2_YCBCR_ENC_709,              "709"},
    {V4L2_YCBCR_ENC_XV601,            "xv601"},
    {V4L2_YCBCR_ENC_XV709,            "xv709"},
    {V4L2_YCBCR_ENC_BT2020,           "bt2020"},
    {V4L2_YCBCR_ENC_BT2020_CONST_LUM, "bt2020c"},
    {V4L2_YCBCR_ENC_SMPTE240M,        "smpte240m"},
    {0,                               NULL},
};

static table_entry quantization_table[] = {
    {V4L2_QUANTIZATION_FULL_RANGE, "full"},
    {V4L2_QUANTIZATION_LIM_RANGE,  "limited"},
    {0,                            NULL},
};

static __u32 s2code(table_entry *table, QString s, __u32 v)
{
    const char *str = s.toLower().toStdString().c_str();
    int i;

    for (i = 0;table[i].str != NULL;i++) {
        const char *cmp_str = table[i].str;

        if (strcmp(str, cmp_str) == 0) {
            v = table[i].code;
            break;
        }
    }

    return v;
}

static const char *code2s(table_entry *table, __u32 v)
{
    const char *s = "default";
    int i;

    for (i = 0;table[i].str != NULL;i++) {
        __u32 cmp_code = table[i].code;

        if (v == cmp_code) {
            s = table[i].str;
            break;
        }
    }

    return s;
}

void RagnaPrefs::read_colorspace(QString s)
{
    this->colorspace = s2code(colorspace_table, s, V4L2_COLORSPACE_DEFAULT);
}

void RagnaPrefs::read_quantization(QString s)
{
    this->quantization = s2code(quantization_table, s,
            V4L2_QUANTIZATION_DEFAULT);
}

void RagnaPrefs::read_xfer_func(QString s)
{
    this->xfer_func = s2code(xfer_func_table, s, V4L2_XFER_FUNC_DEFAULT);
}

void RagnaPrefs::read_ycbcr(QString s)
{
    this->ycbcr_enc = s2code(ycbcr_enc_table, s, V4L2_YCBCR_ENC_DEFAULT);
}

const char *RagnaPrefs::colorspace2s(__u32 v)
{
    return code2s(colorspace_table, v);
}

const char *RagnaPrefs::quantization2s(__u32 v)
{
    return code2s(quantization_table, v);
}

const char *RagnaPrefs::xfer_func2s(__u32 v)
{
    return code2s(xfer_func_table, v);
}

const char *RagnaPrefs::ycbcr2s(__u32 v)
{
    return code2s(ycbcr_enc_table, v);
}
