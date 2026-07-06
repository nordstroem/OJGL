; Clinkster music converted from C:\Users\Johan\Downloads\Clinkster\Clinkster\examples\songs\Bstrr_Maytz-Trask.xrns 2026-07-06 11:53:42

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

%define SUBSAMPLES_PER_TICK 15748
%define MAX_INSTRUMENT_SUBSAMPLES 1769472
%define MAX_TOTAL_INSTRUMENT_SAMPLES 1114112
%define MAX_RELEASE_SUBSAMPLES 196608
%define TOTAL_SAMPLES 7012352
%define MAX_TRACK_INSTRUMENT_RENDERS 18

%define MAX_DELAY_LENGTH 14962
%define LEFT_DELAY_LENGTH 14962
%define RIGHT_DELAY_LENGTH 14962
%define DELAY_STRENGTH 0.10000000

%define NUMTRACKS 12
%define LOGNUMTICKS 11
%define MUSIC_LENGTH 1760
%define TICKS_PER_SECOND 11.20000000


	section instdata data align=1

_InstrumentData:
	; 00:  #bassdrum / 06|snare
	db	0,3,26,10,0,6,20,0,0,32,-4,-82,-72,-70,48,-64,-9,-27
	db	57,1,0,-1
	; 01:  Clap / 03|Clap
	db	1,0,21,14,0,10,14,0,0,15,-4,42,50,-39,72,-59,-9,-27
	db	33,1,0,-1
	; 02:  ambient / 01|clap
	db	2,3,0,0,0,96,1,0,0,14,-120,-120,0,0,0,-59,5,-27
	db	52,1,0,-1
	; 03:  #ambient / 0C|lead
	db	1,2,0,0,0,65,1,0,0,14,26,0,-70,0,45,-59,6,-27
	db	28,1,0,2,1,0,1,1,0,2,1,0,3,1,0,0,1,0,1,1,0,1,1,0,2,1,0,1,1,0,-1
	; 04:  ##ambient / 07|bass high
	db	1,0,0,0,0,0,1,0,0,12,7,-120,-47,-70,69,-59,17,-27
	db	28,1,0,2,1,0,1,1,0,4,1,0,-1
	db	-1
	; 05:  bassdrum / 00|bassdrum
	db	0,0,0,0,0,0,1,0,0,30,26,0,-70,0,40,-59,9,-27
	db	33,1,0,-1
	; 06:  hihat / 08|hihat
	db	0,0,26,25,0,35,12,0,0,9,72,34,51,-66,53,-59,-11,-27
	db	57,1,0,-1
	; 07:  #hihat / 0B|Hihat
	db	2,0,26,16,0,10,14,0,0,6,84,43,-29,-39,48,-59,-3,-27
	db	45,1,0,-1
	; 08:  introstrings / 0F|introstrings
	db	1,2,5,6,0,8,17,12,32,24,0,0,0,0,0,-59,-6,11
	db	52,92,64,0,0,32,0,3,32,0,6,1,0,0,1,0,3,1,0,-1
	; 09:  introstrings / 0F|introstrings
	db	1,2,5,6,0,8,17,12,32,24,0,0,0,0,0,-59,-6,11
	db	55,92,64,0,1,32,0,2,32,0,6,1,0,1,1,0,2,1,0,-1
	; 10:  introstrings / 0F|introstrings
	db	1,2,5,6,0,8,17,12,32,24,0,0,0,0,0,-59,-6,11
	db	59,32,0,0,26,0,1,16,6,0,1,44,26,16,0,0,6,0,5,1,0,0,1,0,1,1,0,1,1,0,0,1,0,-1
	; 11:  introstrings / 0F|introstrings
	db	1,2,5,6,0,8,17,12,32,24,0,0,0,0,0,-59,-6,11
	db	52,2,0,2,4,2,1,0,1,4,2,1,0,1,2,1,0,0,2,1,0,1,2,1,0,1,2,1,0,0,4,2,1,0,-1
	db	-1,-1

	section notepos data align=1

_NotePositions:
	; 00:  #bassdrum / 06|snare
	; position 15 - pattern 7
	db	-4,164,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 16 - pattern 8
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 17 - pattern 7
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 18 - pattern 8
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 19 - pattern 12
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 20 - pattern 13
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 21 - pattern 12
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 22 - pattern 14
	db	6,8,1,2,5,4,1,2,9,1,3,4,1
	; position 23 - pattern 7
	db	23,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 24 - pattern 8
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 25 - pattern 7
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5,4,1,2,3
	; position 26 - pattern 15
	db	6,8,1,2,5,4,1,2,9,1,3,4,1,2,5

	; 01:  Clap / 03|Clap
	; position 15 - pattern 7
	db	-4,164,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 16 - pattern 8
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 17 - pattern 7
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 18 - pattern 8
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 19 - pattern 12
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 20 - pattern 13
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 21 - pattern 12
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 22 - pattern 14
	db	7,6,6,1,3,2,7,7,6
	; position 23 - pattern 7
	db	26,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 24 - pattern 8
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 25 - pattern 7
	db	7,6,6,1,3,2,7,7,6,6,1,3,2,5,2
	; position 26 - pattern 15
	db	7,6,6,1,3,2,7,7,6,6,1,3,2

	; 02:  ambient / 01|clap
	; position 14 - pattern 11
	db	-4,156
	; position 15 - pattern 7
	db	8,8,8,8,8,8,8,8
	; position 16 - pattern 8
	db	8,8,8,8,8,8,8,8
	; position 17 - pattern 7
	db	8,8,8,8,8,8,8,8
	; position 18 - pattern 8
	db	8,8,8,8,8,8,8,8
	; position 19 - pattern 12
	db	8,8,8,8,8,8,8,8
	; position 20 - pattern 13
	db	8,8,8,8,8,8,8,8
	; position 21 - pattern 12
	db	8,8,8,8,8,8,8,8
	; position 22 - pattern 14
	db	8,8,8,8,8,8,4,6,6
	; position 23 - pattern 7
	db	8,8,8,8,8,8,8,8
	; position 24 - pattern 8
	db	8,8,8,8,8,8,8,8
	; position 25 - pattern 7
	db	8,8,8,8,8,8,8,8
	; position 26 - pattern 15
	db	8,8,8,8,8,8,4,6,6

	; 03:  #ambient / 0C|lead
	; position 9 - pattern 0
	db	-3,32,4,10,2,6,10,4,10,2,6,6
	; position 10 - pattern 3
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 11 - pattern 9
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 12 - pattern 10
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 13 - pattern 9
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 14 - pattern 11
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 15 - pattern 7
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 16 - pattern 8
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 17 - pattern 7
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 18 - pattern 8
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 19 - pattern 12
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 20 - pattern 13
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 21 - pattern 12
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 22 - pattern 14
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 23 - pattern 7
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 24 - pattern 8
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 25 - pattern 7
	db	4,4,10,2,6,10,4,10,2,6,6
	; position 26 - pattern 15
	db	4,4,10,2,6,10,4,10,2,6,6

	; 04:  ##ambient / 07|bass high
	; position 9 - pattern 0
	db	-3,38,32,18
	; position 10 - pattern 3
	db	14,32
	; position 11 - pattern 9
	db	32,32,18
	; position 12 - pattern 10
	db	14,32
	; position 13 - pattern 9
	db	32,32,18
	; position 14 - pattern 11
	db	14,32
	; position 15 - pattern 7
	db	32,32,18
	; position 16 - pattern 8
	db	14,32
	; position 17 - pattern 7
	db	32,32,18
	; position 18 - pattern 8
	db	14,32
	; position 19 - pattern 12
	db	32,32,18
	; position 20 - pattern 13
	db	14,32
	; position 21 - pattern 12
	db	32,32,18
	; position 22 - pattern 14
	db	14,32
	; position 23 - pattern 7
	db	32,32,18
	; position 24 - pattern 8
	db	14,32
	; position 25 - pattern 7
	db	32,32,18
	; position 26 - pattern 15
	db	14,32

	; 05:  bassdrum / 00|bassdrum
	; position 9 - pattern 0
	db	-3,32,6,6,4,6,2,4,4,6,6,4,10,2
	; position 10 - pattern 3
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 11 - pattern 9
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 12 - pattern 10
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 13 - pattern 9
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 14 - pattern 11
	db	4,6,6,4,6,2,4,4,6,6,4,12
	; position 15 - pattern 7
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 16 - pattern 8
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 17 - pattern 7
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 18 - pattern 8
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 19 - pattern 12
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 20 - pattern 13
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 21 - pattern 12
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 22 - pattern 14
	db	4,6,6,4,6,2,4,4,6,6,4,4,8
	; position 23 - pattern 7
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 24 - pattern 8
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 25 - pattern 7
	db	4,6,6,4,6,2,4,4,6,6,4,10,2
	; position 26 - pattern 15
	db	4,6,6,4,6,2,4,4,6,6,4,6,6

	; 06:  hihat / 08|hihat
	; position 15 - pattern 7
	db	-4,160,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 16 - pattern 8
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 17 - pattern 7
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 18 - pattern 8
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 19 - pattern 12
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 20 - pattern 13
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 21 - pattern 12
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 22 - pattern 14
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 23 - pattern 7
	db	18,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 24 - pattern 8
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 25 - pattern 7
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1
	; position 26 - pattern 15
	db	2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1,2,1,2,2,1

	; 07:  #hihat / 0B|Hihat
	; position 15 - pattern 7
	db	-4,160,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 16 - pattern 8
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 17 - pattern 7
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 18 - pattern 8
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 19 - pattern 12
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 20 - pattern 13
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 21 - pattern 12
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 22 - pattern 14
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4
	; position 23 - pattern 7
	db	18,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 24 - pattern 8
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 25 - pattern 7
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2,4,3,1
	; position 26 - pattern 15
	db	2,2,4,1,3,4,3,3,2,4,3,1,2,2,4,1,3,4,3,3,2

	; 08:  introstrings / 0F|introstrings
	; position 0 - pattern 1
	db	0,32
	; position 1 - pattern 2
	db	32
	; position 2 - pattern 1
	db	64,32
	; position 3 - pattern 2
	db	32
	; position 4 - pattern 4
	db	64,32
	; position 5 - pattern 5
	db	32
	; position 6 - pattern 4
	db	64,32
	; position 7 - pattern 5
	db	32
	; position 11 - pattern 9
	db	-1,226,6,4,22,6,4,16
	; position 12 - pattern 10
	db	6,6,4,22,6,4,16
	; position 13 - pattern 9
	db	6,6,4,22,6,4,16
	; position 14 - pattern 11
	db	6,6,4,22,6,4,6,6,4
	; position 15 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 16 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 17 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 18 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 23 - pattern 7
	db	-2,6,6,4,6,6,4,6,6,4,6,6,4
	; position 24 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 25 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 26 - pattern 15
	db	6,6,4,6,6,4,6,6,4,4,6,6

	; 09:  introstrings / 0F|introstrings
	; position 0 - pattern 1
	db	0,32
	; position 1 - pattern 2
	db	32
	; position 2 - pattern 1
	db	64,32
	; position 3 - pattern 2
	db	32
	; position 4 - pattern 4
	db	64,32
	; position 5 - pattern 5
	db	32
	; position 6 - pattern 4
	db	64,32
	; position 7 - pattern 5
	db	32
	; position 11 - pattern 9
	db	-1,226,6,4,22,6,4,16
	; position 12 - pattern 10
	db	6,6,4,22,6,4,16
	; position 13 - pattern 9
	db	6,6,4,22,6,4,16
	; position 14 - pattern 11
	db	6,6,4,22,6,4,6,6,4
	; position 15 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 16 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 17 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 18 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 23 - pattern 7
	db	-2,6,6,4,6,6,4,6,6,4,6,6,4
	; position 24 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 25 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 26 - pattern 15
	db	6,6,4,6,6,4,6,6,4,4,6,6

	; 10:  introstrings / 0F|introstrings
	; position 0 - pattern 1
	db	0,26,6,26
	; position 1 - pattern 2
	db	6,32,16
	; position 2 - pattern 1
	db	16,26,6,26
	; position 3 - pattern 2
	db	6,32,16
	; position 4 - pattern 4
	db	16,26,6,26
	; position 5 - pattern 5
	db	6,32,16
	; position 6 - pattern 4
	db	16,26,6,26
	; position 7 - pattern 5
	db	6,32,16
	; position 11 - pattern 9
	db	-1,178,6,4,22,6,4,16
	; position 12 - pattern 10
	db	6,6,4,22,6,4,16
	; position 13 - pattern 9
	db	6,6,4,22,6,4,16
	; position 14 - pattern 11
	db	6,6,4,22,6,4,6,6,4
	; position 15 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 16 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 17 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 18 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 23 - pattern 7
	db	-2,6,6,4,6,6,4,6,6,4,6,6,4
	; position 24 - pattern 8
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 25 - pattern 7
	db	6,6,4,6,6,4,6,6,4,6,6,4
	; position 26 - pattern 15
	db	6,6,4,6,6,4,6,6,4,4,6,6

	; 11:  introstrings / 0F|introstrings
	; position 4 - pattern 4
	db	-2,2,1,1,2,2,2,1,1,2,2,2,1,1,2,2,1,2,1,2,4,1,1,2,2,2,1,1,2,2,2,1,1,2,2,1,2,1,2
	; position 5 - pattern 5
	db	4,1,1,2,2,2,1,1,2,2,2,1,1,2,2,1,1,1,1,2,4,1,1,2,2,2,1,1,2,4,1,1,2,2,1,1,1,1,2
	; position 6 - pattern 4
	db	4,1,1,2,2,2,1,1,2,2,2,1,1,2,2,1,2,1,2,4,1,1,2,2,2,1,1,2,2,2,1,1,2,2,1,2,1,2
	; position 7 - pattern 5
	db	4,1,1,2,2,2,1,1,2,2,2,1,1,2,2,1,1,1,1,2,4,1,1,2,2,2,1,1,2,4,1,1,2,2,1,1,1,1,2


	section notesamp data align=1

_NoteSamples:
	; 00:  #bassdrum / 06|snare
	; position 15 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 16 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 17 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 18 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 19 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 20 - pattern 13
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 21 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 22 - pattern 14
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 23 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 24 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 25 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 26 - pattern 15
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 01:  Clap / 03|Clap
	; position 15 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 16 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 17 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 18 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 19 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 20 - pattern 13
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 21 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 22 - pattern 14
	db	0,0,0,0,0,0,0,0,0
	; position 23 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 24 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 25 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 26 - pattern 15
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 02:  ambient / 01|clap
	; position 14 - pattern 11
	db	0
	; position 15 - pattern 7
	db	0,0,0,0,0,0,0,0
	; position 16 - pattern 8
	db	0,0,0,0,0,0,0,0
	; position 17 - pattern 7
	db	0,0,0,0,0,0,0,0
	; position 18 - pattern 8
	db	0,0,0,0,0,0,0,0
	; position 19 - pattern 12
	db	0,0,0,0,0,0,0,0
	; position 20 - pattern 13
	db	0,0,0,0,0,0,0,0
	; position 21 - pattern 12
	db	0,0,0,0,0,0,0,0
	; position 22 - pattern 14
	db	0,0,0,0,0,0,0,0,0
	; position 23 - pattern 7
	db	0,0,0,0,0,0,0,0
	; position 24 - pattern 8
	db	0,0,0,0,0,0,0,0
	; position 25 - pattern 7
	db	0,0,0,0,0,0,0,0
	; position 26 - pattern 15
	db	0,0,0,0,0,0,0,0,0
	db	-1

	; 03:  #ambient / 0C|lead
	; position 9 - pattern 0
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 10 - pattern 3
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 11 - pattern 9
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 12 - pattern 10
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 13 - pattern 9
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 14 - pattern 11
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 15 - pattern 7
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 16 - pattern 8
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 17 - pattern 7
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 18 - pattern 8
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 19 - pattern 12
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 20 - pattern 13
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 21 - pattern 12
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 22 - pattern 14
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 23 - pattern 7
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 24 - pattern 8
	db	0,4,0,0,4,1,6,1,7,5,4
	; position 25 - pattern 7
	db	2,7,2,2,7,2,7,2,3,8,9
	; position 26 - pattern 15
	db	0,4,0,0,4,1,6,1,7,5,4
	db	-1

	; 04:  ##ambient / 07|bass high
	; position 9 - pattern 0
	db	2,2,3
	; position 10 - pattern 3
	db	0,1
	; position 11 - pattern 9
	db	2,2,3
	; position 12 - pattern 10
	db	0,1
	; position 13 - pattern 9
	db	2,2,3
	; position 14 - pattern 11
	db	0,1
	; position 15 - pattern 7
	db	2,2,3
	; position 16 - pattern 8
	db	0,1
	; position 17 - pattern 7
	db	2,2,3
	; position 18 - pattern 8
	db	0,1
	; position 19 - pattern 12
	db	2,2,3
	; position 20 - pattern 13
	db	0,1
	; position 21 - pattern 12
	db	2,2,3
	; position 22 - pattern 14
	db	0,1
	; position 23 - pattern 7
	db	2,2,3
	; position 24 - pattern 8
	db	0,1
	; position 25 - pattern 7
	db	2,2,3
	; position 26 - pattern 15
	db	0,1
	db	-1

	; 05:  bassdrum / 00|bassdrum
	; position 9 - pattern 0
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 10 - pattern 3
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 11 - pattern 9
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 12 - pattern 10
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 13 - pattern 9
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 14 - pattern 11
	db	0,0,0,0,0,0,0,0,0,0,0,0
	; position 15 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 16 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 17 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 18 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 19 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 20 - pattern 13
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 21 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 22 - pattern 14
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 23 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 24 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 25 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 26 - pattern 15
	db	0,0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 06:  hihat / 08|hihat
	; position 15 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 16 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 17 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 18 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 19 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 20 - pattern 13
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 21 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 22 - pattern 14
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 23 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 24 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 25 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 26 - pattern 15
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 07:  #hihat / 0B|Hihat
	; position 15 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 16 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 17 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 18 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 19 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 20 - pattern 13
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 21 - pattern 12
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 22 - pattern 14
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 23 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 24 - pattern 8
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 25 - pattern 7
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	; position 26 - pattern 15
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	db	-1

	; 08:  introstrings / 0F|introstrings
	; position 0 - pattern 1
	db	3,2
	; position 1 - pattern 2
	db	1
	; position 2 - pattern 1
	db	3,2
	; position 3 - pattern 2
	db	1
	; position 4 - pattern 4
	db	3,2
	; position 5 - pattern 5
	db	1
	; position 6 - pattern 4
	db	3,2
	; position 7 - pattern 5
	db	0
	; position 11 - pattern 9
	db	6,6,6,5,5,5,5
	; position 12 - pattern 10
	db	4,4,4,4,4,4,4
	; position 13 - pattern 9
	db	6,6,6,5,5,5,5
	; position 14 - pattern 11
	db	4,4,4,4,4,4,4,4,4
	; position 15 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 16 - pattern 8
	db	4,4,4,4,4,4,4,4,4,4,4,4
	; position 17 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 18 - pattern 8
	db	4,4,4,4,4,4,4,4,4,4,4,4
	; position 23 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 24 - pattern 8
	db	4,4,4,4,4,4,4,4,4,4,4,4
	; position 25 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 26 - pattern 15
	db	4,4,4,4,4,4,4,4,4,4,4,4
	db	-1

	; 09:  introstrings / 0F|introstrings
	; position 0 - pattern 1
	db	3,2
	; position 1 - pattern 2
	db	1
	; position 2 - pattern 1
	db	3,2
	; position 3 - pattern 2
	db	1
	; position 4 - pattern 4
	db	3,2
	; position 5 - pattern 5
	db	1
	; position 6 - pattern 4
	db	3,2
	; position 7 - pattern 5
	db	0
	; position 11 - pattern 9
	db	6,6,6,5,5,5,5
	; position 12 - pattern 10
	db	4,4,4,4,4,4,4
	; position 13 - pattern 9
	db	6,6,6,5,5,5,5
	; position 14 - pattern 11
	db	4,4,4,4,4,4,4,4,4
	; position 15 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 16 - pattern 8
	db	4,4,4,4,4,4,4,4,4,4,4,4
	; position 17 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 18 - pattern 8
	db	4,4,4,4,4,4,4,4,4,4,4,4
	; position 23 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 24 - pattern 8
	db	4,4,4,4,4,4,4,4,4,4,4,4
	; position 25 - pattern 7
	db	6,6,6,6,6,6,5,5,5,5,5,5
	; position 26 - pattern 15
	db	4,4,4,4,4,4,4,4,4,4,4,4
	db	-1

	; 10:  introstrings / 0F|introstrings
	; position 0 - pattern 1
	db	5,7,1,3
	; position 1 - pattern 2
	db	0,2,6
	; position 2 - pattern 1
	db	5,7,1,3
	; position 3 - pattern 2
	db	0,2,6
	; position 4 - pattern 4
	db	5,7,1,3
	; position 5 - pattern 5
	db	0,2,6
	; position 6 - pattern 4
	db	5,7,1,3
	; position 7 - pattern 5
	db	0,2,4
	; position 11 - pattern 9
	db	11,11,11,9,9,9,10
	; position 12 - pattern 10
	db	8,8,8,10,10,10,11
	; position 13 - pattern 9
	db	11,11,11,9,9,9,10
	; position 14 - pattern 11
	db	8,8,8,10,10,10,11,11,11
	; position 15 - pattern 7
	db	11,11,11,11,11,12,9,9,9,9,9,10
	; position 16 - pattern 8
	db	8,8,8,8,8,8,10,10,10,11,11,11
	; position 17 - pattern 7
	db	11,11,11,11,11,12,9,9,9,9,9,10
	; position 18 - pattern 8
	db	8,8,8,8,8,8,10,10,10,11,11,11
	; position 23 - pattern 7
	db	11,11,11,11,11,12,9,9,9,9,9,10
	; position 24 - pattern 8
	db	8,8,8,8,8,8,10,10,10,11,11,11
	; position 25 - pattern 7
	db	11,11,11,11,11,12,9,9,9,9,9,10
	; position 26 - pattern 15
	db	8,8,8,8,8,8,10,10,10,11,11,11
	db	-1

	; 11:  introstrings / 0F|introstrings
	; position 4 - pattern 4
	db	14,10,5,13,13,14,10,5,9,13,14,10,5,9,14,9,10,5,15,17,10,5,16,16,17,10,5,9,16,17,10,5,16,17,5,6,16,4
	; position 5 - pattern 5
	db	8,3,0,7,7,8,3,0,2,7,8,3,0,2,8,3,8,3,0,1,12,3,0,11,11,12,3,0,1,14,3,0,2,14,3,14,3,0,1
	; position 6 - pattern 4
	db	14,10,5,13,13,14,10,5,9,13,14,10,5,9,14,9,10,5,15,17,10,5,16,16,17,10,5,9,16,17,10,5,16,17,5,6,16,4
	; position 7 - pattern 5
	db	8,3,0,7,7,8,3,0,2,7,8,3,0,2,8,3,8,3,0,1,12,3,0,11,11,12,3,0,1,14,3,0,2,14,3,14,3,0,2
	db	-1

