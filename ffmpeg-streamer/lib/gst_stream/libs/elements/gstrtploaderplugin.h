#ifndef GSTRTPLOADERPLUGIN_H
#define GSTRTPLOADERPLUGIN_H

#include <gst/gst.h>
#include <gst/rtp/gstrtpbasepayload.h>

G_BEGIN_DECLS

#define GST_TYPE_PLUGIN_TEMPLATE \
  (gst_custom_rtp_payloader_get_type())
#define GST_PLUGIN_TEMPLATE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PLUGIN_TEMPLATE,GstCustomRTPPayloader))
#define GST_PLUGIN_TEMPLATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PLUGIN_TEMPLATE,GstCustomRTPPayloaderClass))
#define GST_IS_PLUGIN_TEMPLATE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PLUGIN_TEMPLATE))
#define GST_IS_PLUGIN_TEMPLATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PLUGIN_TEMPLATE))

//#define GST_TYPE_CUSTOM_RTP_PAYLOADER (gst_custom_rtp_payloader_get_type())
//G_DECLARE_FINAL_TYPE(GstCustomRTPPayloader, gst_custom_rtp_payloader, GST, CUSTOM_RTP_PAYLOADER, GstRTPBasePayload)

typedef struct _GstCustomRTPPayloader {
    GstRTPBasePayload base_payload;

    gboolean silent;

} GstCustomRTPPayloader;

typedef struct _GstCustomRTPPayloaderClass {
    GstRTPBasePayloadClass parent_class;

}GstCustomRTPPayloaderClass;

GType gst_custom_rtp_payloader_get_type(void);

G_END_DECLS


#endif //GSTRTPLOADERPLUGIN_H
