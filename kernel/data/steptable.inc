/*
 *  Uzebox Step Table
 *  Copyright (C) 2008  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Step Table
// ---------------------------
// CPU=28.63636Mhz
// Mixing Freqency=15734.263736263736 Hz

.word 0x0022 // Note: 0 (C-1),  MIDI note: 0, Freq:8.175798915643707, Step:0.13302208209341956
.word 0x0024 // Note: 1 (C#-1), MIDI note: 1, Freq:8.661957218027252, Step:0.1409319867128105
.word 0x0026 // Note: 2 (D-1),  MIDI note: 2, Freq:9.177023997418988, Step:0.14931223873695731
.word 0x0028 // Note: 3 (D#-1), MIDI note: 3, Freq:9.722718241315027, Step:0.15819080647797057
.word 0x002b // Note: 4 (E-1),  MIDI note: 4, Freq:10.300861153527183, Step:0.16759732133034314
.word 0x002d // Note: 5 (F-1),  MIDI note: 5, Freq:10.913382232281373, Step:0.17756317666297453
.word 0x0030 // Note: 6 (F#-1), MIDI note: 6, Freq:11.562325709738577, Step:0.1881216325916212
.word 0x0033 // Note: 7 (G-1),  MIDI note: 7, Freq:12.249857374429663, Step:0.19930792698144137
.word 0x0036 // Note: 8 (G#-1), MIDI note: 8, Freq:12.978271799373287, Step:0.21115939305009443
.word 0x0039 // Note: 9 (A-1),  MIDI note: 9, Freq:13.75, Step:0.2237155839638837
.word 0x003d // Note: 10 (A#-1),MIDI note: 10, Freq:14.567617547440307, Step:0.2370184048427729
.word 0x0040 // Note: 11 (B-1), MIDI note: 11, Freq:15.433853164253883, Step:0.25111225261482845
.word 0x0044 // Note: 12 (C-2), MIDI note: 12, Freq:16.351597831287414, Step:0.2660441641868391
.word 0x0048 // Note: 13 (C#-2),MIDI note: 13, Freq:17.323914436054505, Step:0.281863973425621
.word 0x004c // Note: 14 (D-2), MIDI note: 14, Freq:18.354047994837977, Step:0.29862447747391463
.word 0x0051 // Note: 15 (D#-2),MIDI note: 15, Freq:19.445436482630054, Step:0.31638161295594114
.word 0x0056 // Note: 16 (E-2), MIDI note: 16, Freq:20.601722307054366, Step:0.33519464266068627
.word 0x005b // Note: 17 (F-2), MIDI note: 17, Freq:21.826764464562746, Step:0.35512635332594905
.word 0x0060 // Note: 18 (F#-2),MIDI note: 18, Freq:23.124651419477154, Step:0.3762432651832424
.word 0x0066 // Note: 19 (G-2), MIDI note: 19, Freq:24.499714748859326, Step:0.39861585396288274
.word 0x006c // Note: 20 (G#-2),MIDI note: 20, Freq:25.956543598746574, Step:0.42231878610018886
.word 0x0073 // Note: 21 (A-2), MIDI note: 21, Freq:27.5, Step:0.4474311679277674
.word 0x0079 // Note: 22 (A#-2),MIDI note: 22, Freq:29.13523509488062, Step:0.4740368096855459
.word 0x0081 // Note: 23 (B-2), MIDI note: 23, Freq:30.86770632850775, Step:0.5022245052296567
.word 0x0088 // Note: 24 (C-3), MIDI note: 24, Freq:32.70319566257483, Step:0.5320883283736783
.word 0x0090 // Note: 25 (C#-3),MIDI note: 25, Freq:34.64782887210902, Step:0.5637279468512421
.word 0x0099 // Note: 26 (D-3), MIDI note: 26, Freq:36.70809598967594, Step:0.597248954947829
.word 0x00a2 // Note: 27 (D#-3),MIDI note: 27, Freq:38.89087296526011, Step:0.6327632259118823
.word 0x00ac // Note: 28 (E-3), MIDI note: 28, Freq:41.20344461410874, Step:0.6703892853213727
.word 0x00b6 // Note: 29 (F-3), MIDI note: 29, Freq:43.653528929125486, Step:0.710252706651898
.word 0x00c1 // Note: 30 (F#-3),MIDI note: 30, Freq:46.24930283895431, Step:0.7524865303664848
.word 0x00cc // Note: 31 (G-3), MIDI note: 31, Freq:48.999429497718666, Step:0.7972317079257657
.word 0x00d8 // Note: 32 (G#-3),MIDI note: 32, Freq:51.91308719749314, Step:0.8446375722003776
.word 0x00e5 // Note: 33 (A-3), MIDI note: 33, Freq:55.0, Step:0.8948623358555348
.word 0x00f3 // Note: 34 (A#-3),MIDI note: 34, Freq:58.27047018976124, Step:0.9480736193710918
.word 0x0101 // Note: 35 (B-3), MIDI note: 35, Freq:61.7354126570155, Step:1.0044490104593133
.word 0x0110 // Note: 36 (C-4), MIDI note: 36, Freq:65.40639132514966, Step:1.0641766567473565
.word 0x0121 // Note: 37 (C#-4),MIDI note: 37, Freq:69.29565774421803, Step:1.1274558937024841
.word 0x0132 // Note: 38 (D-4), MIDI note: 38, Freq:73.41619197935188, Step:1.194497909895658
.word 0x0144 // Note: 39 (D#-4),MIDI note: 39, Freq:77.78174593052022, Step:1.2655264518237646
.word 0x0157 // Note: 40 (E-4), MIDI note: 40, Freq:82.40688922821748, Step:1.3407785706427453
.word 0x016c // Note: 41 (F-4), MIDI note: 41, Freq:87.30705785825097, Step:1.420505413303796
.word 0x0181 // Note: 42 (F#-4),MIDI note: 42, Freq:92.49860567790861, Step:1.5049730607329697
.word 0x0198 // Note: 43 (G-4), MIDI note: 43, Freq:97.99885899543733, Step:1.5944634158515314
.word 0x01b0 // Note: 44 (G#-4),MIDI note: 44, Freq:103.82617439498628, Step:1.6892751444007552
.word 0x01ca // Note: 45 (A-4), MIDI note: 45, Freq:110.0, Step:1.7897246717110695
.word 0x01e5 // Note: 46 (A#-4),MIDI note: 46, Freq:116.54094037952248, Step:1.8961472387421836
.word 0x0202 // Note: 47 (B-4), MIDI note: 47, Freq:123.47082531403103, Step:2.008898020918627
.word 0x0221 // Note: 48 (C-5), MIDI note: 48, Freq:130.8127826502993, Step:2.128353313494713
.word 0x0241 // Note: 49 (C#-5),MIDI note: 49, Freq:138.59131548843604, Step:2.254911787404968
.word 0x0264 // Note: 50 (D-5), MIDI note: 50, Freq:146.8323839587038, Step:2.3889958197913166
.word 0x0288 // Note: 51 (D#-5),MIDI note: 51, Freq:155.56349186104043, Step:2.531052903647529
.word 0x02ae // Note: 52 (E-5), MIDI note: 52, Freq:164.81377845643496, Step:2.6815571412854906
.word 0x02d7 // Note: 53 (F-5), MIDI note: 53, Freq:174.61411571650194, Step:2.841010826607592
.word 0x0303 // Note: 54 (F#-5),MIDI note: 54, Freq:184.99721135581723, Step:3.0099461214659393
.word 0x0330 // Note: 55 (G-5), MIDI note: 55, Freq:195.99771799087463, Step:3.1889268317030623
.word 0x0361 // Note: 56 (G#-5),MIDI note: 56, Freq:207.65234878997256, Step:3.3785502888015104
.word 0x0394 // Note: 57 (A-5), MIDI note: 57, Freq:220.0, Step:3.579449343422139
.word 0x03cb // Note: 58 (A#-5),MIDI note: 58, Freq:233.08188075904496, Step:3.792294477484367
.word 0x0405 // Note: 59 (B-5), MIDI note: 59, Freq:246.94165062806206, Step:4.017796041837254
.word 0x0442 // Note: 60 (C-6), MIDI note: 60, Freq:261.6255653005986, Step:4.256706626989426
.word 0x0483 // Note: 61 (C#-6),MIDI note: 61, Freq:277.1826309768721, Step:4.509823574809936
.word 0x04c7 // Note: 62 (D-6), MIDI note: 62, Freq:293.6647679174076, Step:4.777991639582633
.word 0x0510 // Note: 63 (D#-6),MIDI note: 63, Freq:311.12698372208087, Step:5.062105807295058
.word 0x055d // Note: 64 (E-6), MIDI note: 64, Freq:329.6275569128699, Step:5.363114282570981
.word 0x05af // Note: 65 (F-6), MIDI note: 65, Freq:349.2282314330039, Step:5.682021653215184
.word 0x0605 // Note: 66 (F#-6),MIDI note: 66, Freq:369.99442271163446, Step:6.019892242931879
.word 0x0661 // Note: 67 (G-6), MIDI note: 67, Freq:391.99543598174927, Step:6.377853663406125
.word 0x06c2 // Note: 68 (G#-6),MIDI note: 68, Freq:415.3046975799451, Step:6.757100577603021
.word 0x0729 // Note: 69 (A-6), MIDI note: 69, Freq:440.0, Step:7.158898686844278
.word 0x0796 // Note: 70 (A#-6),MIDI note: 70, Freq:466.1637615180899, Step:7.584588954968734
.word 0x0809 // Note: 71 (B-6), MIDI note: 71, Freq:493.8833012561241, Step:8.035592083674509
.word 0x0883 // Note: 72 (C-7), MIDI note: 72, Freq:523.2511306011972, Step:8.513413253978852
.word 0x0905 // Note: 73 (C#-7),MIDI note: 73, Freq:554.3652619537442, Step:9.019647149619871
.word 0x098e // Note: 74 (D-7), MIDI note: 74, Freq:587.3295358348151, Step:9.555983279165266
.word 0x0a20 // Note: 75 (D#-7),MIDI note: 75, Freq:622.2539674441618, Step:10.124211614590118
.word 0x0aba // Note: 76 (E-7), MIDI note: 76, Freq:659.2551138257398, Step:10.726228565141962
.word 0x0b5d // Note: 77 (F-7), MIDI note: 77, Freq:698.4564628660078, Step:11.364043306430368
.word 0x0c0a // Note: 78 (F#-7),MIDI note: 78, Freq:739.9888454232689, Step:12.039784485863757
.word 0x0cc1 // Note: 79 (G-7), MIDI note: 79, Freq:783.9908719634986, Step:12.755707326812251
.word 0x0d84 // Note: 80 (G#-7),MIDI note: 80, Freq:830.6093951598903, Step:13.514201155206042
.word 0x0e51 // Note: 81 (A-7), MIDI note: 81, Freq:880.0, Step:14.317797373688556
.word 0x0f2b // Note: 82 (A#-7),MIDI note: 82, Freq:932.3275230361799, Step:15.169177909937469
.word 0x1012 // Note: 83 (B-7), MIDI note: 83, Freq:987.7666025122483, Step:16.071184167349017
.word 0x1107 // Note: 84 (C-8), MIDI note: 84, Freq:1046.5022612023945, Step:17.026826507957704
.word 0x120a // Note: 85 (C#-8),MIDI note: 85, Freq:1108.7305239074883, Step:18.039294299239742
.word 0x131d // Note: 86 (D-8), MIDI note: 86, Freq:1174.6590716696303, Step:19.111966558330533
.word 0x1440 // Note: 87 (D#-8),MIDI note: 87, Freq:1244.5079348883235, Step:20.248423229180233
.word 0x1574 // Note: 88 (E-8), MIDI note: 88, Freq:1318.5102276514797, Step:21.452457130283925
.word 0x16ba // Note: 89 (F-8), MIDI note: 89, Freq:1396.9129257320155, Step:22.728086612860736
.word 0x1814 // Note: 90 (F#-8),MIDI note: 90, Freq:1479.9776908465378, Step:24.079568971727515
.word 0x1983 // Note: 91 (G-8), MIDI note: 91, Freq:1567.981743926997, Step:25.5114146536245
.word 0x1b07 // Note: 92 (G#-8),MIDI note: 92, Freq:1661.2187903197805, Step:27.028402310412083
.word 0x1ca3 // Note: 93 (A-8), MIDI note: 93, Freq:1760.0, Step:28.635594747377112
.word 0x1e57 // Note: 94 (A#-8),MIDI note: 94, Freq:1864.6550460723597, Step:30.338355819874938
.word 0x2024 // Note: 95 (B-8), MIDI note: 95, Freq:1975.533205024496, Step:32.14236833469803
.word 0x220e // Note: 96 (C-9), MIDI note: 96, Freq:2093.004522404789, Step:34.05365301591541
.word 0x2414 // Note: 97 (C#-9),MIDI note: 97, Freq:2217.461047814977, Step:36.07858859847949
.word 0x2639 // Note: 98 (D-9), MIDI note: 98, Freq:2349.31814333926, Step:38.22393311666106
.word 0x287f // Note: 99 (D#-9),MIDI note: 99, Freq:2489.015869776647, Step:40.496846458360466
.word 0x2ae8 // Note: 100 (E-9),  MIDI note: 100, Freq:2637.0204553029594, Step:42.90491426056785
.word 0x2d75 // Note: 101 (F-9),  MIDI note: 101, Freq:2793.825851464031, Step:45.45617322572147
.word 0x3029 // Note: 102 (F#-9), MIDI note: 102, Freq:2959.9553816930757, Step:48.15913794345503
.word 0x3306 // Note: 103 (G-9),  MIDI note: 103, Freq:3135.9634878539946, Step:51.022829307249005
.word 0x360f // Note: 104 (G#-9), MIDI note: 104, Freq:3322.437580639561, Step:54.05680462082417
.word 0x3945 // Note: 105 (A-9),  MIDI note: 105, Freq:3520.0, Step:57.271189494754225
.word 0x3cad // Note: 106 (A#-9), MIDI note: 106, Freq:3729.3100921447194, Step:60.676711639749875
.word 0x4049 // Note: 107 (B-9),  MIDI note: 107, Freq:3951.066410048992, Step:64.28473666939605
.word 0x441b // Note: 108 (C-10), MIDI note: 108, Freq:4186.009044809578, Step:68.10730603183082
.word 0x4828 // Note: 109 (C#-10),MIDI note: 109, Freq:4434.922095629954, Step:72.15717719695898
.word 0x4c73 // Note: 110 (D-10), MIDI note: 110, Freq:4698.63628667852, Step:76.44786623332212
.word 0x50fe // Note: 111 (D#-10),MIDI note: 111, Freq:4978.031739553294, Step:80.99369291672093
.word 0x55cf // Note: 112 (E-10), MIDI note: 112, Freq:5274.040910605919, Step:85.8098285211357
.word 0x5aea // Note: 113 (F-10), MIDI note: 113, Freq:5587.651702928062, Step:90.91234645144294
.word 0x6051 // Note: 114 (F#-10),MIDI note: 114, Freq:5919.910763386151, Step:96.31827588691006
.word 0x660c // Note: 115 (G-10), MIDI note: 115, Freq:6271.926975707989, Step:102.04565861449801
.word 0x6c1d // Note: 116 (G#-10),MIDI note: 116, Freq:6644.875161279122, Step:108.11360924164833
.word 0x728b // Note: 117 (A-10), MIDI note: 117, Freq:7040.0, Step:114.54237898950845
.word 0x795a // Note: 118 (A#-10),MIDI note: 118, Freq:7458.620184289437, Step:121.35342327949972
.word 0x8092 // Note: 119 (B-10), MIDI note: 119, Freq:7902.132820097988, Step:128.56947333879216
.word 0x8837 // Note: 120 (C-11), MIDI note: 120, Freq:8372.018089619156, Step:136.21461206366163
.word 0x9050 // Note: 121 (C#-11),MIDI note: 121, Freq:8869.844191259906, Step:144.31435439391794
.word 0x98e5 // Note: 122 (D-11), MIDI note: 122, Freq:9397.272573357044, Step:152.8957324666443
.word 0xa1fd // Note: 123 (D#-11),MIDI note: 123, Freq:9956.063479106588, Step:161.98738583344186
.word 0xab9f // Note: 124 (E-11), MIDI note: 124, Freq:10548.081821211836, Step:171.61965704227137
.word 0xb5d3 // Note: 125 (F-11), MIDI note: 125, Freq:11175.303405856126, Step:181.82469290288591
.word 0xc0a3 // Note: 126 (F#-11),MIDI note: 126, Freq:11839.821526772303, Step:192.63655177382012
