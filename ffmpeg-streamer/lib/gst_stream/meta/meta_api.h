#ifndef META_API_H
#define META_API_H

#include <gst/gst.h>

struct CoordMetaData {
	GstMeta meta{};
	GstClockTime pts{};
	GstClockTime dts{};
};

namespace meta {
	class SMetaType {
	public:
		static SMetaType& getInstance() {
			static SMetaType instance;
			return instance;
		}

		[[nodiscard]] GType get_type() const {
			return api_type;
		}

		static const GstMetaInfo* meta_info_get() {
			static const GstMetaInfo* meta_info = nullptr;

			if (g_once_init_enter(&meta_info)) {
				const GstMetaInfo* mi = gst_meta_register(meta::SMetaType::getInstance().get_type(),
				                                          "CoordMetaData",
				                                          sizeof(CoordMetaData),
				                                          meta_init,
				                                          meta_free,
				                                          meta_transform);
				g_once_init_leave(&meta_info, mi);
			}
			return meta_info;
		}

		static void meta_free(GstMeta* meta, GstBuffer* buffer) {
			GST_DEBUG("free called on buffer %p, coord meta %p", buffer, meta);
		}

		static gboolean meta_init(GstMeta* meta, gpointer params, GstBuffer* buffer) {
			GST_DEBUG("init called on buffer %p, coord meta %p", buffer, meta);

			return TRUE;
		}

		static gboolean meta_transform(GstBuffer* transbuf, GstMeta* meta, GstBuffer* buffer, GQuark type, gpointer data)
		{
			GST_DEBUG("transform %s called from buffer %p to %p, meta %p", g_quark_to_string(type), buffer, transbuf,
			          meta);
			g_print("Transform func: %s", __func__);
			auto source_meta = reinterpret_cast<CoordMetaData *>(meta);

			if (GST_META_TRANSFORM_IS_COPY(type)) {
				const auto* copy_data = static_cast<GstMetaTransformCopy *>(data);

				auto dest_meta = reinterpret_cast<CoordMetaData *>(gst_buffer_add_meta(
					transbuf, meta_info_get(), nullptr));

				if (dest_meta) {
					if (copy_data->offset == 0 && !copy_data->region) {
						g_print("Copy data region.\n");
						dest_meta->dts = source_meta->dts;
						dest_meta->pts = source_meta->pts;
					}
				} else {
					source_meta->pts = -1;
					source_meta->dts = -1;
				}
			}
			else {
				return FALSE;
			}

			return TRUE;
		}

		SMetaType(const SMetaType&) = delete;
		SMetaType& operator=(const SMetaType&) = delete;

	private:
		GType api_type{};

		SMetaType() {
			//api_type = gst_meta_api_type_register("MetaDataAPI", nullptr);
			static const gchar* tags[] = {nullptr};

			if (g_once_init_enter(&api_type)) {
				const GType type = gst_meta_api_type_register("MetaDataAPI", tags);
				g_once_init_leave(&api_type, type);
			}
		}
	};
}

#endif //META_API_H
