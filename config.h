/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx 	= 3; 		/* border pixel of windows */
static const unsigned int snap 		= 32; 		/* snap pixel */
static const unsigned int gappih 	= 20; 		/* horiz inner gap between windows */
static const unsigned int gappiv 	= 10; 		/* vert inner gap between windows */
static const unsigned int gappoh 	= 10; 		/* horiz outer gap between windows and screen edge */
static const unsigned int gappov 	= 20; 		/* vert outer gap between windows and screen edge */
static const int swallowfloating 	= 0; 		/* 1 means swallow floating windows by default */
static const int smartgaps 			= 0; 		/* 1 means no outer gap when there is only one window */
static const int showbar 			= 1; 		/* 0 means no bar */
static const int topbar 			= 1; 		/* 0 means bottom bar */
static const char *fonts[] 			= { "monospace:size=10", "JoyPixels:pixelsize=10:antialias=true:autohint=true" };
static char dmenufont[] 			= "monospace:size=10";
static char normbgcolor[] 			= "#222222";
static char normbordercolor[] 		= "#444444";
static char normfgcolor[] 			= "#bbbbbb";
static char selfgcolor[] 			= "#eeeeee";
static char selbordercolor[] 		= "#770000";
static char selbgcolor[] 			= "#005577";
static char *colors[][3] = {
		/* 				 fg 		  bg 		   border   */
		[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
		[SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
static Sp scratchpads[] = {
	/* name 		cmd */
	{"spterm", 		spcmd1},
	{"spranger", 	spcmd2},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	*/
	/* class 	instance 	title 			tags mask 	isfloating 	isterminal 	noswallow 	monitor */
	{ "Gimp", 	NULL, 		NULL, 			1 << 8, 	0, 			0, 			0, 			-1 },
	{ "St", 	NULL, 		NULL, 			0, 			0, 			1, 			0, 			-1 },
	{ NULL, 	NULL, 		"Event Tester", 0, 			0, 			0, 			1, 			-1 },
	{ NULL, 	"spterm", 	NULL, 			SPTAG(0), 	1, 			1, 			0, 			-1 },
	{ NULL, 	"spcalc", 	NULL, 			SPTAG(1), 	1, 			1, 			0, 			-1 },
};

/* layout(s) */
static const float mfact 	 = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster 	 = 1; 	 /* number of clients in master area */
static const int resizehints = 1; 	 /* 1 means respect size hints in tiled resizals */
#define FORCE_VSPLIT 1 	/* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol 	arrange 					function */
 	{ "[]= tile",	tile },						/* Default: Master on left, slaves on right */
	{ "TTT bstack",	bstack },					/* Master on top, slaves on bottom */
	{ "[@] spiral",	spiral },					/* Fibonacci spiral */
	{ "[\\] dwindle",	dwindle },					/* Decreasing in size right and leftward */
	{ "H[] deck",	deck },						/* Master on left, slaves in monocle-like mode on right */
 	{ "[M] monocle",	monocle },					/* All windows on top of eachother */
	{ "|M| centered",	centeredmaster },			/* Master in middle, slaves on sides */
	{ ">M> floatingcentered",	centeredfloatingmaster },	/* Same but master floats */
	{ "><> floating",	NULL },						/* no layout function means floating behavior */
	{ NULL,		NULL },
};

/* key definitions */
#define MODKEY Mod4Mask /* set ALT as modifier */
#define TAGKEYS(KEY,TAG) \
	{ MODKEY, 						KEY, 	view, 			{.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask, 			KEY, 	toggleview, 	{.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask, 			KEY, 	tag, 			{.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY, 	toggletag, 		{.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD, 	XK_v, 	ACTION##stack, 	{.i = 0 } },
	/* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] 	= { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[] 	= { "st", NULL };
static const char *browsercmd[] = { "chromium", NULL };
static const char *emacscmd[] = { "emacs", NULL };

#include <X11/XF86keysym.h>
#include "shiftview.c"
static Key keys[] = {
	/* modifier 			key 			function 		argument 				description*/
	{ MODKEY,				XK_0,			view,			{.ui = ~0 } },
	{ MODKEY|ShiftMask,		XK_0,			tag,			{.ui = ~0 } },
	{ MODKEY,				XK_Tab,			view,			{0} }, 					/* idk */
	//{ MODKEY,				XK_q,			quit,			{0} }, 					

	/* App shortcuts */
	{ MODKEY,				XK_w,			spawn,		 	{.v = browsercmd } }, 	/* open browser */
	{ MODKEY,				XK_d,			spawn, 			{.v = dmenucmd } }, 	/* open dmenu */
	{ MODKEY,				XK_t, 			spawn,			{.v = termcmd } }, 		/* open terminal */
	{ MODKEY,				XK_n,			spawn,			{.v = emacscmd } }, 	/* open terminal */

	/* Layouts */
/*	{ MODKEY,				XK_t,			setlayout,		{.v = &layouts[0]} }, 	 tile */
/*	{ MODKEY|ShiftMask,		XK_t,			setlayout,		{.v = &layouts[1]} }, bstack */
/*	{ MODKEY,				XK_y,			setlayout,		{.v = &layouts[2]} }, 	 spiral */
/*	{ MODKEY|ShiftMask,		XK_y,			setlayout,		{.v = &layouts[3]} }, 	 dwindle */
/*	{ MODKEY,				XK_u,			setlayout,		{.v = &layouts[4]} }, 	 deck */
/*	{ MODKEY|ShiftMask,		XK_u,			setlayout,		{.v = &layouts[5]} }, 	 monocle */
/*	{ MODKEY,				XK_i,			setlayout,		{.v = &layouts[6]} }, 	 centeredmaster */
/*	{ MODKEY|ShiftMask,		XK_i,			setlayout,		{.v = &layouts[7]} }, 	 centeredfloatingmaster */
/*	{ MODKEY|ShiftMask,		XK_f,			setlayout,		{.v = &layouts[8]} }, 	 floating layout */
	{ MODKEY|ControlMask, 	XK_comma, 		cyclelayout,    {.i = -1 } }, 			 /*cycles layout forward */
	{ MODKEY|ControlMask, 	XK_period, 		cyclelayout,    {.i = +1 } }, 			 /*cycles layout backward */

	/* dwm settings */
	{ MODKEY,				XK_b,			togglebar,		{0} }, 					/* toggle top bar */

	/* Window handling */
	{ MODKEY|ShiftMask,		XK_space,		togglefloating,	{0} }, 					/* toggle floating mode */
	{ MODKEY|ShiftMask,		XK_Return,		togglescratch,	{.ui = 0} }, 			/* toggle scratchpad */
	{ MODKEY|ShiftMask,				XK_c,			killclient,		{0} }, 					/* kill window */
	{ MODKEY,				XK_o,			incnmaster, 	{.i = +1 } }, 			/* promote window to master */
	{ MODKEY|ShiftMask,		XK_o,			incnmaster, 	{.i = -1 } }, 			/* demote master window */
	{ MODKEY,				XK_f,			togglefullscr,	{0} }, 					/* toggle fullscreen */
	{ MODKEY,				XK_h,			setmfact,		{.f = -0.05} }, 		/* dec size of master */
	{ MODKEY,				XK_l,			setmfact, 		{.f = +0.05} }, 		/* inc size of master */
	{ MODKEY,				XK_semicolon,	shiftview,		{ .i = 1 } }, 			/* idk */
	{ MODKEY,				XK_s,			togglesticky,	{0} }, 					/* idk */
	{ MODKEY, 				XK_c, 		shiftview, 		{.i =  1 } },
	{ MODKEY, 				XK_g, 		shiftview, 		{.i = -1 } },

	/* window navigation */
	{ MODKEY,				XK_space,		zoom,			{0} }, 					/* switch master and last */
	{ MODKEY,				XK_comma,		focusmon,		{.i = -1 } }, 			/* focus prev monitor */
	{ MODKEY|ShiftMask,		XK_comma,		tagmon,			{.i = -1 } }, 
	{ MODKEY,				XK_period,		focusmon,		{.i = +1 } }, 			/* focus next monitor */
	{ MODKEY|ShiftMask,		XK_period,		tagmon,			{.i = +1 } },

	/* gaps */
	{ MODKEY,				XK_z,			incrgaps,		{.i = +3 } }, 			/* inc gaps between windows */
	{ MODKEY|ShiftMask,				XK_z,			incrgaps,		{.i = -3 } }, 			/* dec gaps between windows */
	{ MODKEY,	              XK_q,      incrovgaps,     {.i = +3 } }, /* increase vertical outer gaps */
	{ MODKEY|ShiftMask,              XK_q,      incrovgaps,     {.i = -3 } }, /* decrease vetical outer gaps */
	{ MODKEY,				XK_a,			togglegaps,		{0} }, 					/* toggle gaps */
	{ MODKEY|ShiftMask,		XK_a,			defaultgaps,	{0} }, 					/* reset to default gaps */

	/* V is automatically bound above in STACKKEYS */

	{ 0, XF86XK_AudioRaiseVolume,			spawn,			SHCMD("amixer set Master 10%+") }, /* raise audio volume by 5 */
	{ 0, XF86XK_AudioLowerVolume,			spawn,			SHCMD("amixer set Master 10%-") }, /* decrease audio volume by 5 */
	{ ShiftMask, XF86XK_AudioRaiseVolume,			spawn,			SHCMD("amixer set Master 1%+") }, /* raise audio volume by 5 */
	{ ShiftMask, XF86XK_AudioLowerVolume,			spawn,			SHCMD("amixer set Master 1%-") }, /* decrease audio volume by 5 */
	{ 0, XF86XK_AudioMute,					spawn,			SHCMD("amixer set Master toggle") }, /* toggle audio */
	{ 0, XF86XK_Launch1,					spawn,			SHCMD("xset dpms force off") },
	{ 0, XF86XK_TouchpadToggle,				spawn,			SHCMD("(synclient | grep 'TouchpadOff.*1' && synclient TouchpadOff=0) || synclient TouchpadOff=1") }, /* toggle touchpad */
	{ 0, XF86XK_TouchpadOff,				spawn,			SHCMD("synclient TouchpadOff=1") }, /* turn off touchpad */
	{ 0, XF86XK_TouchpadOn,					spawn,			SHCMD("synclient TouchpadOff=0") }, /* turn on touchpad */
	{ 0, XF86XK_MonBrightnessUp,			spawn,			SHCMD("xbacklight -inc 15") }, /* inc monitor brightness by 15*/
	{ ShiftMask, XF86XK_MonBrightnessUp,			spawn,			SHCMD("xbacklight -inc 1") }, /* inc monitor brightness by 1*/
	{ 0, XF86XK_MonBrightnessDown,			spawn,			SHCMD("xbacklight -dec 15") }, /* dec monitor brightness by 15 */
	{ ShiftMask, XF86XK_MonBrightnessDown,			spawn,			SHCMD("xbacklight -dec 1") }, /* dec monitor brightness by 1*/
	STACKKEYS(MODKEY, 						focus)
	STACKKEYS(MODKEY|ShiftMask, 			push)
	TAGKEYS(				XK_parenleft,			0)
	TAGKEYS(				XK_parenright,			1)
	TAGKEYS(				XK_braceright,			2)
	TAGKEYS(				XK_plus,			3)
	TAGKEYS(				XK_braceleft,			4)
	TAGKEYS(				XK_bracketright,			5)
	TAGKEYS(				XK_bracketleft,			6)
	TAGKEYS(				XK_exclam,			7)
	TAGKEYS(				XK_equal,			8)

	/* { MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } }, */
	/* { MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } }, */
	/* { MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} }, */
	/* { MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } }, */
	/* { MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } }, */
	/* { MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } }, */
	/* { MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } }, */
	/* { MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } }, */
	/* { MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } }, */

};

