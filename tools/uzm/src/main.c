/*
 * uzm -- makes and checks Uzeamp's video files (.UZM). Standalone: C and FFmpeg's libraries,
 * the same files as demos/Uzeamp/tools/uzm.py, byte for byte.
 */
#include <stdlib.h>
#include <string.h>

#include <libavcodec/version.h>
#include <libavfilter/version.h>
#include <libavformat/version.h>
#include <libavutil/avutil.h>

#include "media.h"
#include "uzm.h"

#ifdef _WIN32
#include <windows.h>
#endif

static const char *USAGE =
"uzm " UZM_VERSION_STRING " -- Uzeamp's video files (.UZM)\n"
"\n"
"  uzm encode IN OUT.UZM [options]     a video file (anything FFmpeg reads)\n"
"      --rate auto|60|30|25|24          pictures a second (default auto: from IN's frame rate).\n"
"                                       Each picture is stored once and shown 1-3 times\n"
"      --ivtc                           IN carries film with its frames repeated (24 fps\n"
"                                       animation in a 60 fps file): each film frame once\n"
"      --title T --artist A             shown by the player (default title: IN's name)\n"
"      --start S --duration D           a part of IN (seconds, or [HH:]MM:SS[.m])\n"
"      --dither ours|bayerN|fs|sierra2|sierra2_4a|sierra3|burkes|atkinson|none\n"
"                                       ours (default): Sierra 2-4A, each channel on its own,\n"
"                                       its error kept. The others are FFmpeg's paletteuse\n"
"      --temporal                       each showing of a picture dithered on its own, aiming\n"
"                                       at what the ones before missed (stored at 60)\n"
"      --black N                        levels 0..N become black\n"
"      --cut N                          pixels whose brightest channel is <= N become black\n"
"      --pre F / --post F               FFmpeg filters before / after the scale (pixel art:\n"
"                                       --pre scale=384:224:flags=area --post unsharp=3:3:0.6)\n"
"      --scaler S                       the scale's method (default lanczos)\n"
"      --letterbox                      keep IN's aspect: black bars, top and bottom\n"
"      --crop W:H:X:Y                   a part of IN's picture first\n"
"      --loudness N|off                 the sound's loudness in LUFS (default -15; two-pass\n"
"                                       EBU R128 on the mono mix); off: the source's level\n"
"      --compress                       a film's range made smaller before the loudness\n"
"      --audio N                        IN's audio track N (0 the first)\n"
"  uzm test OUT.UZM [seconds] [--rate R]\n"
"                                       a test pattern: colour bars, ramps, a moving box, the\n"
"                                       picture's number, a 440 Hz tone (default 10 s at 60)\n"
"  uzm info FILE.UZM                    the header, and a check of every sector\n"
"  uzm frame FILE.UZM N OUT.png [--frame]\n"
"                                       picture N as an image, 4x (--frame: N is a console\n"
"                                       frame, 60.05 a second)\n"
"  uzm sound FILE.UZM OUT.wav           the sound as the player plays it, frame by frame\n"
"  uzm cadence IN [--start S] [--duration D]\n"
"                                       which rate IN's pictures really have (--ivtc or not)\n"
"  uzm version                          this program's and FFmpeg's versions\n"
"\n"
"  -v before the command: FFmpeg's messages too.\n";

static int run(int argc, char **argv)
{
	int verbose = 0;
	while (argc > 1 && (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--verbose"))) {
		verbose = 1;
		argv++;
		argc--;
	}
	if (argc < 2 || !strcmp(argv[1], "help") || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
		fputs(USAGE, argc < 2 ? stderr : stdout);
		return argc < 2 ? 2 : 0;
	}
	const char *cmd = argv[1];
	int n = argc - 2;
	char **a = argv + 2;
	media_init(verbose);
	dither_init();
	if (!strcmp(cmd, "encode")) return cmd_encode(n, a);
	if (!strcmp(cmd, "cadence")) return cmd_cadence(n, a);
	if (!strcmp(cmd, "info") && n == 1) return cmd_info(a[0]);
	if (!strcmp(cmd, "sound") && n == 2) return cmd_sound(a[0], a[1]);
	if (!strcmp(cmd, "frame") && (n == 3 || (n == 4 && !strcmp(a[3], "--frame"))))
		return cmd_frame(a[0], strtol(a[1], NULL, 10), a[2], n == 4);
	if (!strcmp(cmd, "test") && n >= 1) {
		int rate = 60;
		double secs = 10.0;
		for (int i = 1; i < n; i++) {
			if (!strcmp(a[i], "--rate") && i + 1 < n) rate = atoi(a[++i]);
			else if (!strncmp(a[i], "--", 2)) die("unknown option %s", a[i]);
			else secs = atof(a[i]);
		}
		return cmd_test(a[0], secs, rate);
	}
	if (!strcmp(cmd, "version")) {
		printf("uzm %s\nFFmpeg libraries: avformat %s, avcodec %s, avfilter %s, avutil %s\n",
		       UZM_VERSION_STRING, AV_STRINGIFY(LIBAVFORMAT_VERSION), AV_STRINGIFY(LIBAVCODEC_VERSION),
		       AV_STRINGIFY(LIBAVFILTER_VERSION), AV_STRINGIFY(LIBAVUTIL_VERSION));
		return 0;
	}
	fputs(USAGE, stderr);
	return 2;
}

#ifdef _WIN32
/* the arguments as UTF-8, whatever the console's code page */
int wmain(int argc, wchar_t **wargv)
{
	char **argv = xmalloc(((size_t)argc + 1) * sizeof *argv);
	for (int i = 0; i < argc; i++) {
		int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);
		argv[i] = xmalloc((size_t)len);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], len, NULL, NULL);
	}
	argv[argc] = NULL;
	SetConsoleOutputCP(CP_UTF8);
	return run(argc, argv);
}
#else
int main(int argc, char **argv) { return run(argc, argv); }
#endif
