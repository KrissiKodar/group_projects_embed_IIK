#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc2a194f5, "module_layout" },
	{ 0x94882503, "class_unregister" },
	{ 0xd8bb65d6, "device_destroy" },
	{ 0xfe990052, "gpio_free" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xfe57199f, "gpiod_to_irq" },
	{ 0x8b02fce7, "gpiod_set_debounce" },
	{ 0x1968a16, "gpiod_direction_input" },
	{ 0x96d4cdbc, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x1bcd1cdc, "class_destroy" },
	{ 0x3c8a0e89, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x71148895, "__class_create" },
	{ 0xc739763, "__register_chrdev" },
	{ 0x1f64fd19, "gpiod_get_raw_value" },
	{ 0xd472d34d, "gpiod_set_raw_value" },
	{ 0x25ce6759, "gpiod_export" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x919f8e3d, "gpiod_unexport" },
	{ 0x18910b35, "gpio_to_desc" },
	{ 0x92997ed8, "_printk" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "4B9353DC9C70774D3085F95");
