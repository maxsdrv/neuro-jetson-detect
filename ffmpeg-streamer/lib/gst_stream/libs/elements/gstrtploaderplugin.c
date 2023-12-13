#include "gstrtploaderplugin.h"

GST_DEBUG_CATEGORY_STATIC (gst_plugin_template_debug);
#define GST_CAT_DEFAULT gst_plugin_template_debug

enum {
    PROP_0 = 0,
    PROP_SILENT
};


#define gst_custom_rtp_payloader_parent_class parent_class
G_DEFINE_TYPE(GstCustomRTPPayloader, gst_custom_rtp_payloader, GST_TYPE_RTP_BASE_PAYLOAD);

static void gst_custom_rtp_payloader_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* paramSpec);
static void gst_custom_rtp_payloader_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* paramSpec);

static gboolean gst_custom_rtp_payloader_sink_event(GstPad* pad, GstObject* parent, GstEvent* event);
static GstFlowReturn gst_custom_rtp_payloader_chain(GstPad* pad, GstObject* object, GstBuffer* buf);
static GstFlowReturn gst_custom_rtp_payloader_handler_buffer(GstRTPBasePayload* base_payload, GstBuffer* buffer);

static gboolean gst_custom_rtp_payloader_set_caps(GstRTPBasePayload* base, GstCaps* caps);

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE(
        "sink",
        GST_PAD_SINK,
        GST_PAD_ALWAYS,
        GST_STATIC_CAPS("video/x-raw, format=(string)I420")
);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE(
        "src",
        GST_PAD_SRC,
        GST_PAD_ALWAYS,
        GST_STATIC_CAPS("application/x-rtp, media=(string)video")
);

static void gst_custom_rtp_payloader_class_init(GstCustomRTPPayloaderClass* klass) {
    GstRTPBasePayloadClass* base_payload_class = GST_RTP_BASE_PAYLOAD_CLASS(klass);
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    GstElementClass* element_class = GST_ELEMENT_CLASS(klass);

    gst_element_class_add_pad_template(element_class,
                                       gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(element_class,
                                       gst_static_pad_template_get(&sink_factory));

    object_class->set_property = gst_custom_rtp_payloader_set_property;
    object_class->get_property = gst_custom_rtp_payloader_get_property;

    g_object_class_install_property(object_class, PROP_SILENT,
                                    g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                                                         FALSE, G_PARAM_READWRITE));

    gst_element_class_set_static_metadata(element_class,
                                          "Custom RTP Payloader",
                                          "Codec/Payloader/Network",
                                          "Packs video frames and metadata into RTP packets",
                                          "maxim@gstreamer.com");


    base_payload_class->handle_buffer = gst_custom_rtp_payloader_handler_buffer;
    base_payload_class->set_caps = gst_custom_rtp_payloader_set_caps;
}

static void gst_custom_rtp_payloader_init(GstCustomRTPPayloader* self) {
    self->silent = FALSE;
}

static GstFlowReturn gst_custom_rtp_payloader_handler_buffer(GstRTPBasePayload* base, GstBuffer* buffer) {
    GstCustomRTPPayloader* self = GST_PLUGIN_TEMPLATE(base);

    GstBuffer* rtp_packet = gst_rtp_base_payload_allocate_output_buffer(base, gst_buffer_get_size(buffer), 0, 0);

    return gst_rtp_base_payload_push(base, rtp_packet);
}

static gboolean gst_custom_rtp_payloader_set_caps(GstRTPBasePayload* base, GstCaps* caps) {
    const int clock_rate = 90000;

    GST_DEBUG_OBJECT(base, "Setting caps: %", GST_PTR_FORMAT, caps);

    gst_rtp_base_payload_set_options(base, "video", TRUE, "RAW", clock_rate);

    return gst_rtp_base_payload_set_outcaps(base, NULL);
}

static void gst_custom_rtp_payloader_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* paramSpec) {
    GstCustomRTPPayloader* self = GST_PLUGIN_TEMPLATE(object);

    switch (prop_id) {
        case PROP_SILENT:
            self->silent = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, paramSpec);
            break;
    }
}

static void gst_custom_rtp_payloader_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* paramSpec) {
    GstCustomRTPPayloader* self = GST_PLUGIN_TEMPLATE(object);

    switch (prop_id) {
        case PROP_SILENT:
            g_value_set_boolean (value, self->silent);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, self);
            break;
    }
}

static gboolean gst_custom_rtp_payloader_sink_event(GstPad* pad, GstObject* parent, GstEvent* event) {
    return TRUE;
}

static GstFlowReturn gst_custom_rtp_payloader_chain(GstPad* pad, GstObject* object, GstBuffer* buf) {

    return GST_FLOW_OK;
}

static gboolean plugin_init(GstPlugin* plugin) {
    GST_DEBUG_CATEGORY_INIT(gst_plugin_template_debug, "plugin", 0, "Payloader plugin");

    if (!gst_element_register(plugin, "plugin_payloader", GST_RANK_NONE, GST_TYPE_PLUGIN_TEMPLATE)) {
        return FALSE;
    }
    return TRUE;
}

#ifndef PACKAGE
#define PACKAGE "myfirstplugin"
#endif

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    plugin_payloader,
    "Custom RTP Payloader",
    plugin_init,
    "1.0",
    "LGPL",
    "Gstreamer",
    "http://gstreamer.net/"
)