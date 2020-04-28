#ifndef _CORE_COLOR_H_
#define _CORE_COLOR_H_

typedef union
{
	uint32_t rgba;
	struct
	{
		uint8_t a;
		uint8_t b;
		uint8_t g;
		uint8_t r;
	};
} Color;

#endif
