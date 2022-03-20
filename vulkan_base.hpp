#pragma once

#include <vulkan/vulkan.h>

#include <atomic>

#include <och_err.h>
#include <och_fio.h>
#include <och_virtual_keys.h>

#include "heap_buffer.h"

struct required_extension_layer_list
{
	static constexpr uint32_t max_cnt = 8;

	const char* m_inst_extensions[max_cnt] {
#ifdef OCH_VALIDATE
			"VK_EXT_debug_utils",
#endif // OCH_VALIDATE
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
	};

	const char* m_inst_layers[max_cnt] {
#ifdef OCH_VALIDATE
			"VK_LAYER_KHRONOS_validation",
#endif // OCH_VALIDATE
	};

#ifdef OCH_VALIDATE
	uint32_t m_inst_extension_cnt = 3;

	uint32_t m_inst_layer_cnt = 1;
#else
	uint32_t m_inst_extension_cnt = 2;

	uint32_t m_inst_layer_cnt = 0;
#endif // OCH_VALIDATE

	const char* m_dev_extensions[max_cnt] {
		"VK_KHR_swapchain",
	};

	const char* m_dev_layers[max_cnt] {
		"VK_LAYER_KHRONOS_validation",
	};

	uint32_t m_dev_extension_cnt = 1;

	uint32_t m_dev_layer_cnt = 1;

	och::status add_instance_extensions(const char** exts, uint32_t cnt)
	{
		if (m_inst_extension_cnt + cnt > max_cnt)
			return to_status(och::error::argument_too_large);

		for (uint32_t i = 0; i != cnt; ++i)
			m_inst_extensions[m_inst_extension_cnt++] = exts[i];

		return{};
	};

	och::status add_instance_layers(const char** layers, uint32_t cnt)
	{
		if (m_inst_layer_cnt + cnt > max_cnt)
			return to_status(och::error::argument_too_large);

		for (uint32_t i = 0; i != cnt; ++i)
			m_inst_layers[m_inst_layer_cnt++] = layers[i];

		return{};
	};

	och::status add_device_extensions(const char** exts, uint32_t cnt)
	{
		if (m_dev_extension_cnt + cnt > max_cnt)
			return to_status(och::error::argument_too_large);

		for (uint32_t i = 0; i != cnt; ++i)
			m_dev_extensions[m_dev_extension_cnt++] = exts[i];

		return{};
	};

	och::status add_device_layers(const char** layers, uint32_t cnt)
	{
		if (m_dev_layer_cnt + cnt > max_cnt)
			return to_status(och::error::argument_too_large);

		for (uint32_t i = 0; i != cnt; ++i)
			m_dev_layers[m_dev_layer_cnt++] = layers[i];

		return{};
	};

	const char* const* inst_extensions() const noexcept { return m_inst_extension_cnt ? m_inst_extensions : nullptr; }

	uint32_t inst_extension_cnt() const noexcept { return m_inst_extension_cnt; }

	const char* const* inst_layers() const noexcept { return m_inst_layer_cnt ? m_inst_layers : nullptr; }

	uint32_t inst_layer_cnt() const noexcept { return m_inst_layer_cnt; }

	const char* const* dev_extensions() const noexcept { return m_dev_extension_cnt ? m_dev_extensions : nullptr; }

	uint32_t dev_extension_cnt() const noexcept { return m_dev_extension_cnt; }

	const char* const* dev_layers() const noexcept { return m_dev_layer_cnt ? m_dev_layers : nullptr; }

	uint32_t dev_layer_cnt() const noexcept { return m_dev_layer_cnt; }

	och::status check_instance_support(bool& has_support) const noexcept
	{
		{
			uint32_t avl_ext_cnt;
			check(vkEnumerateInstanceExtensionProperties(nullptr, &avl_ext_cnt, nullptr));
			heap_buffer<VkExtensionProperties> avl_exts(avl_ext_cnt);
			check(vkEnumerateInstanceExtensionProperties(nullptr, &avl_ext_cnt, avl_exts.data()));

			for (uint32_t i = 0; i != m_inst_extension_cnt; ++i)
			{
				for (uint32_t j = 0; j != avl_ext_cnt; ++j)
					if (!strcmp(m_inst_extensions[i], avl_exts[j].extensionName))
						goto EXT_AVAILABLE;

				has_support = false;

				return {};

			EXT_AVAILABLE:;
			}
		}

		{
			uint32_t avl_layer_cnt;
			check(vkEnumerateInstanceLayerProperties(&avl_layer_cnt, nullptr));
			heap_buffer<VkLayerProperties> avl_layers(avl_layer_cnt);
			check(vkEnumerateInstanceLayerProperties(&avl_layer_cnt, avl_layers.data()));

			for (uint32_t i = 0; i != m_inst_layer_cnt; ++i)
			{
				for (uint32_t j = 0; j != avl_layer_cnt; ++j)
					if (!strcmp(m_inst_layers[i], avl_layers[j].layerName))
						goto LAYER_AVAILABLE;

				has_support = false;

				return{};

			LAYER_AVAILABLE:;
			}
		}

		has_support = true;

		return {};
	}

	och::status check_device_support(VkPhysicalDevice dev, bool& has_support) const noexcept
	{
		{
			uint32_t avl_ext_cnt;
			check(vkEnumerateDeviceExtensionProperties(dev, nullptr, &avl_ext_cnt, nullptr));
			heap_buffer<VkExtensionProperties> avl_exts(avl_ext_cnt);
			check(vkEnumerateDeviceExtensionProperties(dev, nullptr, &avl_ext_cnt, avl_exts.data()));

			for (uint32_t i = 0; i != m_dev_extension_cnt; ++i)
			{
				for (uint32_t j = 0; j != avl_ext_cnt; ++j)
					if (!strcmp(m_dev_extensions[i], avl_exts[j].extensionName))
						goto EXT_AVAILABLE;

				has_support = false;

				return {};

			EXT_AVAILABLE:;
			}
		}

		{
			uint32_t avl_layer_cnt;
			check(vkEnumerateDeviceLayerProperties(dev, &avl_layer_cnt, nullptr));
			heap_buffer<VkLayerProperties> avl_layers(avl_layer_cnt);
			check(vkEnumerateDeviceLayerProperties(dev, &avl_layer_cnt, avl_layers.data()));

			for (uint32_t i = 0; i != m_dev_layer_cnt; ++i)
			{
				for (uint32_t j = 0; j != avl_layer_cnt; ++j)
					if (!strcmp(m_dev_layers[i], avl_layers[j].layerName))
						goto LAYER_AVAILABLE;

				has_support = false;

				return {};

			LAYER_AVAILABLE:;
			}
		}

		has_support = true;

		return {};
	}
};



struct queue_family_info
{
	static constexpr uint32_t MAX_QUEUE_CNT = 4;

	uint32_t family_index;

	uint32_t cnt;

	uint32_t offset; // Used if compute and graphics queue families are merged.

	VkQueue queues[MAX_QUEUE_CNT];

	VkQueue& operator[](size_t n) noexcept { return queues[n + offset]; }

	const VkQueue& operator[](size_t n) const noexcept { return queues[n - offset]; }
};



using physical_device_suitable_callback_fn = bool (*) (const VkPhysicalDevice physical_device) noexcept;



struct vulkan_context_create_info
{
	const char* app_name = "[[???]]";
	uint32_t window_width = 1440;
	uint32_t window_height = 810;
	uint32_t requested_general_queues = 1;
	uint32_t requested_compute_queues = 0;
	uint32_t requested_transfer_queues = 0;
	VkImageUsageFlags swapchain_image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	uint32_t requested_api_version = VK_API_VERSION_1_0;
	bool allow_compute_graphics_queue_merge = true;
	bool allow_window_resizing = true;
	const VkPhysicalDeviceFeatures2* enabled_device_features2 = nullptr;
	physical_device_suitable_callback_fn physical_device_suitable_callback = nullptr;
	och::iohandle debug_output_handle = och::get_stdout();
	required_extension_layer_list required_features_and_extensions{};
};



struct vulkan_context
{
	static inline const wchar_t* WINDOW_CLASS_NAME = L"och_vulkan_context_window_class";

	static constexpr uint32_t MAX_SWAPCHAIN_IMAGE_CNT = 4;

	static constexpr uint32_t MESSAGE_PUMP_THREAD_TERMINATION_MESSAGE = 0x419; // WM_USER + 0x19

	static constexpr uint32_t MAX_MESSAGE_PUMP_INITIALIZATION_TIME_MS = 2000;



	struct
	{
		std::atomic<bool> framebuffer_resized;
		std::atomic<bool> is_window_closed;
		bool fully_initialized : 1;
		bool separate_compute_and_general_queue : 1;
	} m_flags{};

	static_assert(sizeof(m_flags) <= sizeof(uint64_t));

	queue_family_info m_general_queues{};

	queue_family_info m_compute_queues{};

	queue_family_info m_transfer_queues{};

	VkExtent3D m_min_image_transfer_granularity{};



	VkPhysicalDeviceMemoryProperties m_memory_properties{};



	void* m_hwnd{};

	VkInstance m_instance{};

	VkPhysicalDevice m_physical_device{};

	VkDevice m_device{};

	VkSurfaceKHR m_surface{};

	VkDebugUtilsMessengerEXT m_debug_messenger{};

	och::iohandle m_debug_output_handle{};



	VkSwapchainKHR m_swapchain{};

	VkFormat m_swapchain_format{};

	VkColorSpaceKHR m_swapchain_colorspace{};

	VkPresentModeKHR m_swapchain_present_mode{};

	VkImageUsageFlags m_image_swapchain_usage{};

	uint32_t m_swapchain_image_cnt{};

	VkExtent2D m_swapchain_extent{};

	VkImage m_swapchain_images[MAX_SWAPCHAIN_IMAGE_CNT]{};

	VkImageView m_swapchain_image_views[MAX_SWAPCHAIN_IMAGE_CNT]{};



	void* m_message_pump_thread_handle{};

	uint32_t m_message_pump_thread_id{};

	void* m_message_pump_initialization_wait_event{}; // Signaled by the message pump thread once it has created its window

	void* m_message_pump_start_wait_event{}; // Signaled once the main thread wants the message pump thread to run

	const char* m_app_name{};





	std::atomic<uint8_t> m_input_char_tail{};

	std::atomic<uint8_t> m_input_char_head{};

	uint16_t m_mouse_x{};

	uint16_t m_mouse_y{};

	int32_t m_mouse_vscroll{};

	int32_t m_mouse_hscroll{};

	std::atomic<char32_t> m_input_char_queue[64]{};

	uint64_t m_pressed_keycodes[4]{};


	och::status create(const vulkan_context_create_info* create_info) noexcept;

	void destroy() const noexcept;


	och::status recreate_swapchain() noexcept;


	och::status suitable_memory_type_idx(uint32_t& out_memory_type_idx, uint32_t memory_type_mask, VkMemoryPropertyFlags property_flags) const noexcept;

	och::status load_shader_module_file(VkShaderModule& out_shader_module, const char* filename) const noexcept;

	och::status begin_onetime_command(VkCommandBuffer& out_command_buffer, VkCommandPool command_pool) const noexcept;

	och::status submit_onetime_command(VkCommandBuffer command_buffer, VkCommandPool command_pool, VkQueue submit_queue, bool wait_and_free = true) const noexcept;

	och::status create_buffer(VkBuffer& out_buffer, VkDeviceMemory& out_memory, VkDeviceSize bytes, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_idx_cnt = 0, const uint32_t* queue_family_indices = nullptr) const noexcept;

	och::status create_image_with_view(VkImageView& out_view, VkImage& out_image, VkDeviceMemory& out_memory, VkExtent3D extent, VkImageAspectFlags aspect, VkImageUsageFlags image_usage, VkImageType image_type, VkImageViewType view_type, VkFormat image_format, VkFormat view_format, VkMemoryPropertyFlags memory_properties, VkImageTiling image_tiling = VK_IMAGE_TILING_OPTIMAL, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_idx_cnt = 0, const uint32_t* queue_family_indices = nullptr) noexcept;

	och::status create_images_with_views(uint32_t image_cnt, VkImageView* out_views, VkImage* out_images, VkDeviceMemory& out_memory, VkExtent3D extent, VkImageAspectFlags aspect, VkImageUsageFlags image_usage, VkImageType image_type, VkImageViewType view_type, VkFormat image_format, VkFormat view_format, VkMemoryPropertyFlags memory_properties, VkImageTiling image_tiling = VK_IMAGE_TILING_OPTIMAL, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, uint32_t queue_family_idx_cnt = 0, const uint32_t* queue_family_indices = nullptr) noexcept;

	och::status begin_message_processing() noexcept;

	void end_message_processing() noexcept;


	och::status set_window_note(const char* note) noexcept;


	bool is_window_closed() const noexcept;

	bool is_framebuffer_resized() const noexcept;

	void enqueue_input_char(uint32_t utf16_cp) noexcept;

	char32_t get_input_char() noexcept;

	char32_t peek_input_char() const noexcept;

	void set_keycode(och::vk keycode) noexcept;

	void unset_keycode(och::vk keycode) noexcept;

	bool get_keycode(och::vk keycode) noexcept;

	void reset_pressed_keys() noexcept;

	void set_mouse_pos(uint16_t x, uint16_t y) noexcept;
};
