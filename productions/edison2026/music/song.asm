; Clinkster music converted from .\productions\edison2026\music\edison2026_3.xrns 2026-07-18 13:27:39

%define USES_SINE 1
%define USES_SAWTOOTH 1
%define USES_SQUARE 1
%define USES_PARABOLA 0
%define USES_TRIANGLE 0
%define USES_NOISE 1
%define USES_VELOCITY 0
%define USES_LONG_NOTES 0
%define USES_DELAY 1
%define USES_PANNING 0
%define USES_INDEXDECAY 0
%define USES_GAIN 1

%define SUBSAMPLES_PER_TICK 17640
%define MAX_INSTRUMENT_SUBSAMPLES 2818048
%define MAX_TOTAL_INSTRUMENT_SAMPLES 2162688
%define MAX_RELEASE_SUBSAMPLES 589824
%define TOTAL_SAMPLES 4390912
%define MAX_TRACK_INSTRUMENT_RENDERS 9

%define MAX_DELAY_LENGTH 16758
%define LEFT_DELAY_LENGTH 16758
%define RIGHT_DELAY_LENGTH 16758
%define DELAY_STRENGTH 0.10000000

%define NUMTRACKS 9
%define LOGNUMTICKS 10
%define MUSIC_LENGTH 960
%define TICKS_PER_SECOND 10.00000000


	section instdata data align=1

_InstrumentData:
	; 00:  snare / 06|snare
	db	0,3,26,10,0,6,20,0,0,32,-4,-82,-72,-70,48,-64,-9,-27
	db	24,1,0,-1
	db	-1
	; 01:  bassdrum / 00|bassdrum
	db	0,0,0,0,0,0,1,0,0,30,26,0,-70,0,40,-59,9,-27
	db	28,1,0,-1
	; 02:  hihat / 09|hihat
	db	0,0,26,25,0,35,12,0,0,9,72,34,51,-66,53,-59,-11,-27
	db	24,1,0,-1
	; 03:  #hihat / 01|clap
	db	2,3,0,0,0,96,1,0,0,6,-120,-120,0,0,0,-59,5,-27
	db	24,1,0,11,1,0,-1
	; 04:  introstrings / 10|introstrings
	db	1,2,5,6,0,8,17,12,15,24,0,0,0,0,0,-80,-32,32
	db	50,95,32,16,0,4,16,0,4,4,0,1,4,0,2,1,0,1,24,4,0,-1
	; 05:  introstrings / 10|introstrings
	db	1,2,5,6,0,8,17,12,15,24,0,0,0,0,0,-80,-32,32
	db	53,16,0,1,95,32,16,0,4,24,4,0,3,4,0,0,4,0,3,1,0,-1
	; 06:  introstrings / 10|introstrings
	db	1,2,5,6,0,8,17,12,15,24,0,0,0,0,0,-80,-32,32
	db	60,1,0,1,24,4,0,4,4,0,1,4,0,-1
	; 07:  introstrings / 10|introstrings
	db	1,2,5,6,0,8,17,12,15,24,0,0,0,0,0,-80,-32,32
	db	62,1,0,-1
	; 08:  #introstrings / 11|#introstrings
	db	1,2,5,6,0,8,17,12,17,24,0,0,0,0,0,-59,-6,21
	db	62,1,0,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,4,1,0,1,1,0,-1
	db	-1,-1

	section notepos data align=1

_NotePositions:
	; 00:  snare / 06|snare
	; position 0 - pattern 0
	db	4,8,8,8,8,8,8,8
	; position 1 - pattern 1
	db	8,8,8,8,8,8,8,8
	; position 2 - pattern 1
	db	8,8,8,8,8,8,8,8
	; position 3 - pattern 0
	db	8,8,8,8,8,8,8,8
	; position 4 - pattern 2
	db	8,8,8,8,8,8,8,8
	; position 5 - pattern 2
	db	8,8,8,8,8,8,8,8
	; position 6 - pattern 0
	db	8,8,8,8,8,8,8,8
	; position 7 - pattern 3
	db	8,8,8,8,8,8,8,8
	; position 8 - pattern 2
	db	8,8,8,8,8,8,8,8
	; position 9 - pattern 0
	db	8,8,8,8,8,8,8,8
	; position 10 - pattern 4
	db	8,8,8,8,8,8,8,8
	; position 11 - pattern 4
	db	8,8,8,8,8,8,8,8
	; position 12 - pattern 0
	db	8,8,8,8,8,8,8,8
	; position 13 - pattern 0
	db	8,8,8,8,8,8,8,8

	; 01:  bassdrum / 00|bassdrum
	; position 0 - pattern 0
	db	0,6,4,6,6,4,6,6,4,6,6,4
	; position 1 - pattern 1
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 2 - pattern 1
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 3 - pattern 0
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 4 - pattern 2
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 5 - pattern 2
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 6 - pattern 0
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 7 - pattern 3
	db	6,4,2,4,6,6,4,6,6,4,6,6,4
	; position 8 - pattern 2
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 9 - pattern 0
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 10 - pattern 4
	db	6,4,2,4,6,6,4,6,6,4,6,6,4
	; position 11 - pattern 4
	db	6,4,2,4,6,6,4,6,6,4,6,6,4
	; position 12 - pattern 0
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 13 - pattern 0
	db	6,6,4,6,6,4,6,6,4,6,6,4

	; 02:  hihat / 09|hihat
	; position 0 - pattern 0
	db	0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 1 - pattern 1
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 2 - pattern 1
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 3 - pattern 0
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 4 - pattern 2
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 5 - pattern 2
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 6 - pattern 0
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 7 - pattern 3
	db	2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2
	; position 8 - pattern 2
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 9 - pattern 0
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 10 - pattern 4
	db	2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	; position 11 - pattern 4
	db	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	; position 12 - pattern 0
	db	1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	; position 13 - pattern 0
	db	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2

	; 03:  #hihat / 01|clap
	; position 0 - pattern 0
	db	14,16,16,16
	; position 1 - pattern 1
	db	16,16,16,16
	; position 2 - pattern 1
	db	16,16,16,16
	; position 3 - pattern 0
	db	16,16,16,16
	; position 4 - pattern 2
	db	16,16,16,16
	; position 5 - pattern 2
	db	16,16,16,16
	; position 6 - pattern 0
	db	16,16,16,16
	; position 7 - pattern 3
	db	2,4,4,4,2,2,4,4,4,2,2,4,4,6,16
	; position 8 - pattern 2
	db	16,16,16,16
	; position 9 - pattern 0
	db	16,16,16,16
	; position 10 - pattern 4
	db	2,4,4,4,2,2,4,4,4,2,2,4,4,6,16
	; position 11 - pattern 4
	db	2,4,4,4,2,2,4,4,4,2,2,4,4,6,16
	; position 12 - pattern 0
	db	16,16,16,16
	; position 13 - pattern 0
	db	16,16,16,16

	; 04:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	-2,0,4,4,4,24,4,4
	; position 5 - pattern 2
	db	20,4,4,4,24,4,4
	; position 8 - pattern 2
	db	-1,148,4,4,4,24,4,4
	; position 10 - pattern 4
	db	84,16,16
	; position 11 - pattern 4
	db	32,16,16

	; 05:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	-2,0,4,4,4,24,4,4
	; position 5 - pattern 2
	db	20,4,4,4,24,4,4
	; position 8 - pattern 2
	db	-1,148,4,4,4,24,4,4
	; position 10 - pattern 4
	db	84,16,16
	; position 11 - pattern 4
	db	32,16,16

	; 06:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	-2,0,4,4,4,24,4,4
	; position 5 - pattern 2
	db	20,4,4,4,24,4,4
	; position 8 - pattern 2
	db	-1,148,4,4,4,24,4,4

	; 07:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	-2,4
	; position 5 - pattern 2
	db	64
	; position 8 - pattern 2
	db	-1,192

	; 08:  #introstrings / 11|#introstrings
	; position 1 - pattern 1
	db	64,2,4,2,2,4,2,2,24,6,6,6
	; position 2 - pattern 1
	db	4,2,4,2,2,4,2,2,24,6,6,6


	section notesamp data align=1

_NoteSamples:
	; 00:  snare / 06|snare
	; position 0 - pattern 0
	db	0,0,0,0,0,0,0,0
	; position 1 - pattern 1
	db	0,0,0,0,0,0,0,0
	; position 2 - pattern 1
	db	0,0,0,0,0,0,0,0
	; position 3 - pattern 0
	db	0,0,0,0,0,0,0,0
	; position 4 - pattern 2
	db	0,0,0,0,0,0,0,0
	; position 5 - pattern 2
	db	0,0,0,0,0,0,0,0
	; position 6 - pattern 0
	db	0,0,0,0,0,0,0,0
	; position 7 - pattern 3
	db	0,0,0,0,0,0,0,0
	; position 8 - pattern 2
	db	0,0,0,0,0,0,0,0
	; position 9 - pattern 0
	db	0,0,0,0,0,0,0,0
	; position 10 - pattern 4
	db	0,0,0,0,0,0,0,0
	; position 11 - pattern 4
	db	0,0,0,0,0,0,0,0
	; position 12 - pattern 0
	db	0,0,0,0,0,0,0,0
	; position 13 - pattern 0
	db	0,0,0,0,0,0,0,0
	db	-1

	; 01:  bassdrum / 00|bassdrum
	; position 0 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 1 - pattern 1
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 2 - pattern 1
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 3 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 4 - pattern 2
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 5 - pattern 2
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 6 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 7 - pattern 3
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 8 - pattern 2
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 9 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 10 - pattern 4
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 11 - pattern 4
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 12 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 13 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 02:  hihat / 09|hihat
	; position 0 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 1 - pattern 1
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 2 - pattern 1
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 3 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 4 - pattern 2
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 5 - pattern 2
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 6 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 7 - pattern 3
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 8 - pattern 2
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 9 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 10 - pattern 4
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 11 - pattern 4
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 12 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 13 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 03:  #hihat / 01|clap
	; position 0 - pattern 0
	db	1,1,1,1
	; position 1 - pattern 1
	db	1,1,1,1
	; position 2 - pattern 1
	db	1,1,1,1
	; position 3 - pattern 0
	db	1,1,1,1
	; position 4 - pattern 2
	db	1,1,1,1
	; position 5 - pattern 2
	db	1,1,1,1
	; position 6 - pattern 0
	db	1,1,1,1
	; position 7 - pattern 3
	db	0,0,0,0,1,0,0,0,0,1,0,0,0,1,1
	; position 8 - pattern 2
	db	1,1,1,1
	; position 9 - pattern 0
	db	1,1,1,1
	; position 10 - pattern 4
	db	0,0,0,0,1,0,0,0,0,1,0,0,0,1,1
	; position 11 - pattern 4
	db	0,0,0,0,1,0,0,0,0,1,0,0,0,1,1
	; position 12 - pattern 0
	db	1,1,1,1
	; position 13 - pattern 0
	db	1,1,1,1
	db	-1

	; 04:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	5,4,5,7,5,8,6
	; position 5 - pattern 2
	db	5,4,5,7,5,8,6
	; position 8 - pattern 2
	db	5,4,5,7,5,8,6
	; position 10 - pattern 4
	db	2,3,1
	; position 11 - pattern 4
	db	2,3,0
	db	-1

	; 05:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	7,6,7,4,7,5,8
	; position 5 - pattern 2
	db	7,6,7,4,7,5,8
	; position 8 - pattern 2
	db	7,6,7,4,7,5,8
	; position 10 - pattern 4
	db	3,0,2
	; position 11 - pattern 4
	db	3,0,1
	db	-1

	; 06:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	4,3,4,1,4,2,0
	; position 5 - pattern 2
	db	4,3,4,1,4,2,0
	; position 8 - pattern 2
	db	4,3,4,1,4,2,0
	db	-1

	; 07:  introstrings / 10|introstrings
	; position 4 - pattern 2
	db	0
	; position 5 - pattern 2
	db	0
	; position 8 - pattern 2
	db	0
	db	-1

	; 08:  #introstrings / 11|#introstrings
	; position 1 - pattern 1
	db	0,2,3,0,2,6,4,7,0,5,1,2
	; position 2 - pattern 1
	db	0,2,3,0,2,6,4,7,0,5,1,2
	db	-1

