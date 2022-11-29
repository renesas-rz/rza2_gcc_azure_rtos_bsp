	.section  "SourceImage", "a"
	.balign  32
	.global  g_jcu_example_image
g_jcu_example_image:
	.incbin  "../src/renesas/application/Image/image01.jpg"
