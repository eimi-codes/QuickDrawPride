#include "graphics.h"
#include "flags.h"

static GraphicsCommand crab_pride_flag_commands[]={
	GraphicsCommand(Bitmap),
	GraphicsCommand(EndCommandList)
};

static GraphicsCommand trans_crab_flag_commands[]={
	GraphicsCommand(Bitmap),
	GraphicsCommand(EndCommandList)
};

static GraphicsCommand autistic_pride_flag_commands[]={
	GraphicsCommand(Bitmap),
	GraphicsCommand(EndCommandList)
};

Flag crab_pride_flag = Flag(
	"crab-pride",
	"The Cool Crab Pride Flag",
	"Created by Foone Turing in 2022, based on the Cool Crab from Print Shop Deluxe",
	"crab|crabs|gay-crabs",
	crab_pride_flag_commands
);

Flag trans_crab_flag = Flag(
	"trans-crab",
	"The Cool Crab Trans Flag",
	"Created by Foone Turing in 2022, based on the Cool Crab from Print Shop Deluxe",
	"transcrab",
	trans_crab_flag_commands
);

Flag autistic_pride_flag = Flag(
	"autistic",
	"The Autistic Pride Flag",
	"Created by Autistic Empire, under a Creative Commons Attribution-ShareAlike 4.0 International License: https://www.autisticempire.com/autistic-pride/",
	"autism|autism-pride|autistic-pride",
	autistic_pride_flag_commands,
	RGB(248,243,234),
	4,
	LAYOUT_CENTER
);
