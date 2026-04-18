/*
 * Chinese AVS video decoder dispatcher.
 *
 * This keeps the native FFmpeg AVS1-P2/JiZhun decoder for regular CAVS
 * streams and uses the imported AVS+/Guangdian decoder only for profile 0x48.
 */

#include "libavutil/mem.h"

#include "avcodec.h"
#include "codec_internal.h"
#include "decode.h"
#include "startcode.h"

#define CAVS_START_CODE         0x000001b0
#define CAVS_PROFILE_JIZHUN     0x20
#define CAVS_PROFILE_GUANGDIAN  0x48

extern const FFCodec ff_cavs_native_decoder;
extern const FFCodec ff_cavs_lib_decoder;

typedef enum CAVSDecoderMode {
    CAVS_DECODER_MODE_UNSET = 0,
    CAVS_DECODER_MODE_NATIVE,
    CAVS_DECODER_MODE_LIB,
} CAVSDecoderMode;

typedef struct CAVSDispatchContext {
    CAVSDecoderMode mode;
    void *native_priv;
    void *lib_priv;
} CAVSDispatchContext;

static const AVProfile cavs_profiles[] = {
    { CAVS_PROFILE_JIZHUN,    "JiZhun" },
    { CAVS_PROFILE_GUANGDIAN, "Guangdian/AVS+" },
    { FF_PROFILE_UNKNOWN },
};

static av_cold int cavs_dispatch_close(AVCodecContext *avctx);

static int cavs_find_profile(const uint8_t *buf, int buf_size)
{
    const uint8_t *buf_end = buf + buf_size;
    const uint8_t *buf_ptr = buf;
    uint32_t stc = 0;

    while (buf_ptr < buf_end) {
        buf_ptr = avpriv_find_start_code(buf_ptr, buf_end, &stc);
        if (stc == CAVS_START_CODE && buf_ptr < buf_end)
            return buf_ptr[0];
    }

    return AVERROR_INVALIDDATA;
}

static int cavs_dispatch_call_init(AVCodecContext *avctx, const FFCodec *codec,
                                   void *priv)
{
    void *dispatch_priv = avctx->priv_data;
    int ret = 0;

    avctx->priv_data = priv;
    if (codec->init)
        ret = codec->init(avctx);
    avctx->priv_data = dispatch_priv;

    return ret;
}

static int cavs_dispatch_call_close(AVCodecContext *avctx, const FFCodec *codec,
                                    void *priv)
{
    void *dispatch_priv = avctx->priv_data;
    int ret = 0;

    if (!priv)
        return 0;

    avctx->priv_data = priv;
    if (codec->close)
        ret = codec->close(avctx);
    avctx->priv_data = dispatch_priv;

    return ret;
}

static av_cold int cavs_dispatch_init(AVCodecContext *avctx)
{
    CAVSDispatchContext *s = avctx->priv_data;
    int ret;

    s->native_priv = av_mallocz(ff_cavs_native_decoder.priv_data_size);
    s->lib_priv = av_mallocz(ff_cavs_lib_decoder.priv_data_size);
    if (!s->native_priv || !s->lib_priv)
        return AVERROR(ENOMEM);

    ret = cavs_dispatch_call_init(avctx, &ff_cavs_native_decoder, s->native_priv);
    if (ret < 0)
        goto fail;

    ret = cavs_dispatch_call_init(avctx, &ff_cavs_lib_decoder, s->lib_priv);
    if (ret < 0)
        goto fail;

    avctx->pix_fmt = AV_PIX_FMT_YUV420P;

    return 0;

fail:
    cavs_dispatch_close(avctx);
    return ret;
}

static av_cold int cavs_dispatch_close(AVCodecContext *avctx)
{
    CAVSDispatchContext *s = avctx->priv_data;
    int ret = 0;

    ret = cavs_dispatch_call_close(avctx, &ff_cavs_native_decoder, s->native_priv);
    ret |= cavs_dispatch_call_close(avctx, &ff_cavs_lib_decoder, s->lib_priv);

    av_freep(&s->native_priv);
    av_freep(&s->lib_priv);

    return ret;
}

static void cavs_dispatch_flush(AVCodecContext *avctx)
{
    CAVSDispatchContext *s = avctx->priv_data;
    void *dispatch_priv = avctx->priv_data;

    if (s->native_priv && ff_cavs_native_decoder.flush) {
        avctx->priv_data = s->native_priv;
        ff_cavs_native_decoder.flush(avctx);
    }
    if (s->lib_priv && ff_cavs_lib_decoder.flush) {
        avctx->priv_data = s->lib_priv;
        ff_cavs_lib_decoder.flush(avctx);
    }

    avctx->priv_data = dispatch_priv;
}

static const FFCodec *cavs_dispatch_pick_decoder(CAVSDispatchContext *s,
                                                 const AVPacket *avpkt)
{
    int profile;

    if (s->mode == CAVS_DECODER_MODE_NATIVE)
        return &ff_cavs_native_decoder;
    if (s->mode == CAVS_DECODER_MODE_LIB)
        return &ff_cavs_lib_decoder;

    profile = cavs_find_profile(avpkt->data, avpkt->size);
    if (profile == CAVS_PROFILE_GUANGDIAN) {
        s->mode = CAVS_DECODER_MODE_LIB;
    } else if (profile == CAVS_PROFILE_JIZHUN) {
        s->mode = CAVS_DECODER_MODE_NATIVE;
    } else {
        return &ff_cavs_native_decoder;
    }

    return s->mode == CAVS_DECODER_MODE_LIB ? &ff_cavs_lib_decoder
                                            : &ff_cavs_native_decoder;
}

static int cavs_dispatch_decode_frame(AVCodecContext *avctx, AVFrame *frame,
                                      int *got_frame, AVPacket *avpkt)
{
    CAVSDispatchContext *s = avctx->priv_data;
    const FFCodec *codec = cavs_dispatch_pick_decoder(s, avpkt);
    void *codec_priv = codec == &ff_cavs_lib_decoder ? s->lib_priv
                                                     : s->native_priv;
    void *dispatch_priv = avctx->priv_data;
    int ret;

    avctx->priv_data = codec_priv;
    ret = codec->cb.decode(avctx, frame, got_frame, avpkt);
    avctx->priv_data = dispatch_priv;

    return ret;
}

const FFCodec ff_cavs_decoder = {
    .p.name           = "cavs",
    .p.long_name      = NULL_IF_CONFIG_SMALL("Chinese AVS (Audio Video Standard)"),
    .p.type           = AVMEDIA_TYPE_VIDEO,
    .p.id             = AV_CODEC_ID_CAVS,
    .priv_data_size   = sizeof(CAVSDispatchContext),
    .init             = cavs_dispatch_init,
    .close            = cavs_dispatch_close,
    FF_CODEC_DECODE_CB(cavs_dispatch_decode_frame),
    .p.capabilities   = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_DELAY |
                        AV_CODEC_CAP_OTHER_THREADS,
    .caps_internal    = FF_CODEC_CAP_INIT_CLEANUP |
                        FF_CODEC_CAP_AUTO_THREADS |
                        FF_CODEC_CAP_SETS_FRAME_PROPS,
    .p.pix_fmts       = (const enum AVPixelFormat[]) { AV_PIX_FMT_YUV420P,
                                                       AV_PIX_FMT_NONE },
    .p.profiles       = NULL_IF_CONFIG_SMALL(cavs_profiles),
    .flush            = cavs_dispatch_flush,
};
