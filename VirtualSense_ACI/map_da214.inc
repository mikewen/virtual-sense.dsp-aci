****************************************************************************************************
* Memory Mapping
****************************************************************************************************

****************************************************************************************************
* REVISION HISTORY:
*
*    date        user     description
* ----------    ------    ------------
* MM/DD/2002    userid    initial checkin
*
****************************************************************************************************

****************************************************************************************************
* SECTION 0 : Memory mapping
****************************************************************************************************
*
* MEMORY  MAPPING:
*
* MMRS are mapped from 0x0000 to 0x005f in first MDP.
*
*       A[23,22] bits split the addressable space into 4 x 2 Mwords segments:
*                                                ---------------------
*                                                        22  22_111 1_1111_1100_0000_000  0
*                                         Byte Address   32  10_987 6_5432_1098_7654_321  0
*
*                                                        22  2_1111 1111_1100_0000_0000
*                                     DSP Word Address   21  0_9876 5432_1098_7654_3210
*                                                       ------------------------------------
*                                                       | S |  P   |    WORD ADDRESS    | U |
*                                                       | E |  A   |                    | N |
*                                                       | G |  G   |                    | U |
*                                                       | M |  E   |                    | S |
*                                                       | E |      |                    | E |
*                                                       | N |      |                    | D |
*                                                       | T |      |                    |   |
* Addresses shown below are word addresses
*
*         -----------------       <-- START @ of DARAM0: 00  0_0000_0000_0000_0000_0000  X
*         |    DARAM0     |
*         | L=4 kwords    |
*         -----------------       <-- END @ of DARAM0  : 00  0_0000_0000_1111_1111_1111  X
*
*
*               ---- up to------
*
*
*         -----------------       <-- START @ of DARAM7: 00  0_0000_0111_0000_0000_0000  X
*         |   DARAM7      |
*         | L=4 kwords    |
*         -----------------       <-- END @ of DARAM7  : 00  0_0000_0111_1111_1111_1111  X
*
*         -----------------       <-- START @ of SARAM0: 00  0_0000_1000_0000_0000_0000  X
*         | SARAM0        |
*         | L=4 kwords    |
*         -----------------       <-- END @ of SARAM0  : 00  0_0000_1000_1111_1111_1111  X
*
*         -----------------      <-- START @ of SARAM16: 00  0_0000_1000_1111_1111_1111  X
*         | SARAM16       |
*         | L=4 kwords    |
*         -----------------       <-- END @ of SARAM16 : 00  0_0000_0000_0000_0000_0000  X
*
*         -----------------       <-- START @   CE0    : 00  0_0001_1000_1111_1111_1111  X
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XX EXTERNAL XXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         -----------------       <-- END @  CE0       : 01 11111 1111 1111 1111 1111  X
*
* 01      ----------------- <-- START @ CE1      : 01 00000 0000 0000 0000 0000  X
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XX EXTERNAL XXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         ----------------- <-- END @ CE1        : 01 11111 1111 1111 1111 1111  X
*
* 10      -----------------       <-- START @  CE2     : 10 00000 0000 0000 0000 0000  X
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XX HARD     XXX|
*         |XX CACHABLE XXX|
*         |XX SEGMENT  XXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XX EXTERNAL XXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         -----------------       <-- END @ CE2        : 10 11111 1111 1111 1111 1111  X
*
* 11      -----------------       <-- START @  CE3     : 11 00000 0000 0000 0000 0000  X
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XX HARD     XXX|
*         |XX CACHABLE XXX|
*         |XX SEGMENT  XXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XX EXTERNAL XXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         |XXXXXXXXXXXXXXX|
*         -----------------       <-- END @ CE3        : 11 11111 1111 1111 0111 1111  X
*
*         -----------------       <-- START @ of BOOTCO: 11 11111 1111 1111 1000 0000  X
*         | BOOTCODE      |
*         | L=128 words   |
*         -----------------       <-- END @ of BOOTCO  : 11 11111 1111 1111 1111 1111  X
*
* Addressable Memory Space: 8 Mwords = 4 Segments of ( 32 pages of 64kwords ).
* 24 bit Address bus [23:0].
* Word granularity => bit 0 unused.
*
* Data Address field:
*
*       |       |       |       |       |       |       |
*       -------------------------------------------------
*       | | | | | | | | | | | | | | | | | | | | | | | |X|
*       -------------------------------------------------
*       |             |                               | |
*       |   [23:17]   |             [16:1]            |0|
*       |             |                               | |
*
*              |                       |               |
*              |                       |               |
*              V                       V               V
*
*          Main Data            16 bit pointer      Unused
*            Page              (word addressing)
*
*              |                       |
*              |                       |
*              V                       V
*
*            MDP             Direct Access
*            MDP             Indirect Access CDP
*            MDP05           Indirect Access AR[0-5]
*            MDP67           Indirect Access AR[6-7]
*
*       Main Data Page registers MDP, MDP05, or MDP67:
*
*       |       |       |       |       |
*       ---------------------------------
*       |X|X|X|X|X|X|X|X|X| | | | | | | |
*       ---------------------------------
*       |                 | | |         |
*       |     [15:7]      |6|5|  [4:0]  |
*       |                 | | |         |
*
*               |           |      |
*               |           |      |
*               V           V      V
*
*             Unused     Segment  Page
*                        [0-3]    [0-31]
*
* In order to address a specific word in memory space (external or internal),
* two symbols are used:
*
* - A symbol for MDP, MDP05, or MDP67 registers: this symbol indicates in
*   which Segment and in which page in this segment we want to address
*   the word.
*
*   For example: "S0_P24" indicates that we want to address a word in (Segment 0-Page 24)
*   "S0_P24" substitution symbol is assigned "0x0018" value by:
*
*   .asg    0x0018,S0_P24           ; Start Address of Segment 0, Page 24
*
*   The following instruction loads MDP, MDP05, or MDP67 with "0x0018" value:
*
*           @#MDP = #S0_P24               ;  4 ; ST_MWK
*         || mmap()                       ;  1 ; MMAP__
*
*   or
*           @#MDP05 = #S0_P24             ;  4 ; ST_MWK
*         || mmap()                       ;  1 ; MMAP__
*
*         or
*           @#MDP67 = #S0_P24             ;  4 ; ST_MWK
*         || mmap()                       ;  1 ; MMAP__
*
*         Main Data Page registers MDP, MDP05, or MDP67 are loaded as following:
*
*         |       |       |       |       |
*         ---------------------------------
*         |X|X|X|X|X|X|X|X|X|0|0|1|1|0|0|0|
*         ---------------------------------
*         |                 | | |         |
*         |     [15:7]      |6|5|  [4:0]  |
*         |                 | | |         |
*
*                 |           |      |
*                 |           |      |
*                 V           V      V
*
*             Masked       Segment  Page
*                          [0-3]    [0-31]
*
*   Chosen Segment is then: "00" => Segment 0
*   Chosen Page is then   : "11000" => Page 24
*
* - A symbol for indirect adressing by Ar(n) pointer:
*
*   Once MDP registers are loaded with the value of the Segment and the value of the
*   page, we have to define which word to address in the 64kw range of the page.
*
*   This can be done by assigning a symbol to the value of the address
*   on which AR(n) is pointing.
*
*   For example:
*
*   .asg    0x0900,TA0_DARAM0    ; Test Address 0 of DARAM0
*
*         assigns 0x0900 value to the test address symbol TA0_DARAM0.
*
*
****************************************************************************************************

*MEMORY MAPPING:

*Segment "00" ("segment 0") pages mapping:

        .asg    0x0000,S0_P00          ; Start Address of Segment 0, Page 0
        .asg    0x0001,S0_P01          ; Start Address of Segment 0, Page 1
        .asg    0x0002,S0_P02          ; Start Address of Segment 0, Page 2
        .asg    0x0003,S0_P03          ; Start Address of Segment 0, Page 3
        .asg    0x0004,S0_P04          ; Start Address of Segment 0, Page 4
        .asg    0x0005,S0_P05          ; Start Address of Segment 0, Page 5
        .asg    0x0006,S0_P06          ; Start Address of Segment 0, Page 6
        .asg    0x0007,S0_P07          ; Start Address of Segment 0, Page 7
        .asg    0x0008,S0_P08          ; Start Address of Segment 0, Page 8
        .asg    0x0009,S0_P09          ; Start Address of Segment 0, Page 9
        .asg    0x000a,S0_P10          ; Start Address of Segment 0, Page 10
        .asg    0x000b,S0_P11          ; Start Address of Segment 0, Page 11
        .asg    0x000c,S0_P12          ; Start Address of Segment 0, Page 12
        .asg    0x000d,S0_P13          ; Start Address of Segment 0, Page 13
        .asg    0x000e,S0_P14          ; Start Address of Segment 0, Page 14
        .asg    0x000f,S0_P15          ; Start Address of Segment 0, Page 15
        .asg    0x0010,S0_P16          ; Start Address of Segment 0, Page 16
        .asg    0x0011,S0_P17          ; Start Address of Segment 0, Page 17
        .asg    0x0012,S0_P18          ; Start Address of Segment 0, Page 18
        .asg    0x0013,S0_P19          ; Start Address of Segment 0, Page 19
        .asg    0x0014,S0_P20          ; Start Address of Segment 0, Page 20
        .asg    0x0015,S0_P21          ; Start Address of Segment 0, Page 21
        .asg    0x0016,S0_P22          ; Start Address of Segment 0, Page 22
        .asg    0x0017,S0_P23          ; Start Address of Segment 0, Page 23
        .asg    0x0018,S0_P24          ; Start Address of Segment 0, Page 24
        .asg    0x0019,S0_P25          ; Start Address of Segment 0, Page 25
        .asg    0x001a,S0_P26          ; Start Address of Segment 0, Page 26
        .asg    0x001b,S0_P27          ; Start Address of Segment 0, Page 27
        .asg    0x001c,S0_P28          ; Start Address of Segment 0, Page 28
        .asg    0x001d,S0_P29          ; Start Address of Segment 0, Page 29
        .asg    0x001e,S0_P30          ; Start Address of Segment 0, Page 30
        .asg    0x001f,S0_P31          ; Start Address of Segment 0, Page 31

*Segment "O1" ("segment 1") pages mapping:

        .asg    0x0020,S1_P00          ; Start Address of Segment 1, Page 0
        .asg    0x0021,S1_P01          ; Start Address of Segment 1, Page 1
        .asg    0x0022,S1_P02          ; Start Address of Segment 1, Page 2
        .asg    0x0023,S1_P03          ; Start Address of Segment 1, Page 3
        .asg    0x0024,S1_P04          ; Start Address of Segment 1, Page 4
        .asg    0x0025,S1_P05          ; Start Address of Segment 1, Page 5
        .asg    0x0026,S1_P06          ; Start Address of Segment 1, Page 6
        .asg    0x0027,S1_P07          ; Start Address of Segment 1, Page 7
        .asg    0x0028,S1_P08          ; Start Address of Segment 1, Page 8
        .asg    0x0029,S1_P09          ; Start Address of Segment 1, Page 9
        .asg    0x002a,S1_P10          ; Start Address of Segment 1, Page 10
        .asg    0x002b,S1_P11          ; Start Address of Segment 1, Page 11
        .asg    0x002c,S1_P12          ; Start Address of Segment 1, Page 12
        .asg    0x002d,S1_P13          ; Start Address of Segment 1, Page 13
        .asg    0x002e,S1_P14          ; Start Address of Segment 1, Page 14
        .asg    0x002f,S1_P15          ; Start Address of Segment 1, Page 15
        .asg    0x0030,S1_P16          ; Start Address of Segment 1, Page 16
        .asg    0x0031,S1_P17          ; Start Address of Segment 1, Page 17
        .asg    0x0032,S1_P18          ; Start Address of Segment 1, Page 18
        .asg    0x0033,S1_P19          ; Start Address of Segment 1, Page 19
        .asg    0x0034,S1_P20          ; Start Address of Segment 1, Page 20
        .asg    0x0035,S1_P21          ; Start Address of Segment 1, Page 21
        .asg    0x0036,S1_P22          ; Start Address of Segment 1, Page 22
        .asg    0x0037,S1_P23          ; Start Address of Segment 1, Page 23
        .asg    0x0038,S1_P24          ; Start Address of Segment 1, Page 24
        .asg    0x0039,S1_P25          ; Start Address of Segment 1, Page 25
        .asg    0x003a,S1_P26          ; Start Address of Segment 1, Page 26
        .asg    0x003b,S1_P27          ; Start Address of Segment 1, Page 27
        .asg    0x003c,S1_P28          ; Start Address of Segment 1, Page 28
        .asg    0x003d,S1_P29          ; Start Address of Segment 1, Page 29
        .asg    0x003e,S1_P30          ; Start Address of Segment 1, Page 30
        .asg    0x003f,S1_P31          ; Start Address of Segment 1, Page 31

*Segment "10" ("segment 2") pages mapping (hard cachable segment):

        .asg    0x0040,S2_P00          ; Start Address of Segment 2, Page 0
        .asg    0x0041,S2_P01          ; Start Address of Segment 2, Page 1
        .asg    0x0042,S2_P02          ; Start Address of Segment 2, Page 2
        .asg    0x0043,S2_P03          ; Start Address of Segment 2, Page 3
        .asg    0x0044,S2_P04          ; Start Address of Segment 2, Page 4
        .asg    0x0045,S2_P05          ; Start Address of Segment 2, Page 5
        .asg    0x0046,S2_P06          ; Start Address of Segment 2, Page 6
        .asg    0x0047,S2_P07          ; Start Address of Segment 2, Page 7
        .asg    0x0048,S2_P08          ; Start Address of Segment 2, Page 8
        .asg    0x0049,S2_P09          ; Start Address of Segment 2, Page 9
        .asg    0x004a,S2_P10          ; Start Address of Segment 2, Page 10
        .asg    0x004b,S2_P11          ; Start Address of Segment 2, Page 11
        .asg    0x004c,S2_P12          ; Start Address of Segment 2, Page 12
        .asg    0x004d,S2_P13          ; Start Address of Segment 2, Page 13
        .asg    0x004e,S2_P14          ; Start Address of Segment 2, Page 14
        .asg    0x004f,S2_P15          ; Start Address of Segment 2, Page 15
        .asg    0x0050,S2_P16          ; Start Address of Segment 2, Page 16
        .asg    0x0051,S2_P17          ; Start Address of Segment 2, Page 17
        .asg    0x0052,S2_P18          ; Start Address of Segment 2, Page 18
        .asg    0x0053,S2_P19          ; Start Address of Segment 2, Page 19
        .asg    0x0054,S2_P20          ; Start Address of Segment 2, Page 20
        .asg    0x0055,S2_P21          ; Start Address of Segment 2, Page 21
        .asg    0x0056,S2_P22          ; Start Address of Segment 2, Page 22
        .asg    0x0057,S2_P23          ; Start Address of Segment 2, Page 23
        .asg    0x0058,S2_P24          ; Start Address of Segment 2, Page 24
        .asg    0x0059,S2_P25          ; Start Address of Segment 2, Page 25
        .asg    0x005a,S2_P26          ; Start Address of Segment 2, Page 26
        .asg    0x005b,S2_P27          ; Start Address of Segment 2, Page 27
        .asg    0x005c,S2_P28          ; Start Address of Segment 2, Page 28
        .asg    0x005d,S2_P29          ; Start Address of Segment 2, Page 29
        .asg    0x005e,S2_P30          ; Start Address of Segment 2, Page 30
        .asg    0x005f,S2_P31          ; Start Address of Segment 2, Page 31

*Segment "11" ("segment 3") pages mapping:

        .asg    0x0060,S3_P00          ; Start Address of Segment 3, Page 0
        .asg    0x0061,S3_P01          ; Start Address of Segment 3, Page 1
        .asg    0x0062,S3_P02          ; Start Address of Segment 3, Page 2
        .asg    0x0063,S3_P03          ; Start Address of Segment 3, Page 3
        .asg    0x0064,S3_P04          ; Start Address of Segment 3, Page 4
        .asg    0x0065,S3_P05          ; Start Address of Segment 3, Page 5
        .asg    0x0066,S3_P06          ; Start Address of Segment 3, Page 6
        .asg    0x0067,S3_P07          ; Start Address of Segment 3, Page 7
        .asg    0x0068,S3_P08          ; Start Address of Segment 3, Page 8
        .asg    0x0069,S3_P09          ; Start Address of Segment 3, Page 9
        .asg    0x006a,S3_P10          ; Start Address of Segment 3, Page 10
        .asg    0x006b,S3_P11          ; Start Address of Segment 3, Page 11
        .asg    0x006c,S3_P12          ; Start Address of Segment 3, Page 12
        .asg    0x006d,S3_P13          ; Start Address of Segment 3, Page 13
        .asg    0x006e,S3_P14          ; Start Address of Segment 3, Page 14
        .asg    0x006f,S3_P15          ; Start Address of Segment 3, Page 15
        .asg    0x0070,S3_P16          ; Start Address of Segment 3, Page 16
        .asg    0x0071,S3_P17          ; Start Address of Segment 3, Page 17
        .asg    0x0072,S3_P18          ; Start Address of Segment 3, Page 18
        .asg    0x0073,S3_P19          ; Start Address of Segment 3, Page 19
        .asg    0x0074,S3_P20          ; Start Address of Segment 3, Page 20
        .asg    0x0075,S3_P21          ; Start Address of Segment 3, Page 21
        .asg    0x0076,S3_P22          ; Start Address of Segment 3, Page 22
        .asg    0x0077,S3_P23          ; Start Address of Segment 3, Page 23
        .asg    0x0078,S3_P24          ; Start Address of Segment 3, Page 24
        .asg    0x0079,S3_P25          ; Start Address of Segment 3, Page 25
        .asg    0x007a,S3_P26          ; Start Address of Segment 3, Page 26
        .asg    0x007b,S3_P27          ; Start Address of Segment 3, Page 27
        .asg    0x007c,S3_P28          ; Start Address of Segment 3, Page 28
        .asg    0x007d,S3_P29          ; Start Address of Segment 3, Page 29
        .asg    0x007e,S3_P30          ; Start Address of Segment 3, Page 30
        .asg    0x007f,S3_P31          ; Start Address of Segment 3, Page 31

*Segment 00 - Page 0 mapping:
*In this page (64kw):
*definition of symbols for each type of memory:

        .asg    0x7f00,REMAP_RESET_VECTOR  ; Address of Reset vector

        .asg    0x0080,STA_DARAM0      ; Start Address of DARAM0, 0x80 words reserved for MMR and Scratch
        .asg    0x0081,SECT_DARAM0     ; Start Address of the section DARAM0
        .asg    0x0800,TA0_DARAM0      ; Test Address 0 of DARAM0
        .asg    0x0801,TA1_DARAM0      ; Test Address 1 of DARAM0
        .asg    0x0802,TA2_DARAM0      ; Test Address 2 of DARAM0
        .asg    0x0803,TA3_DARAM0      ; Test Address 3 of DARAM0
        .asg    0x0fff,END_DARAM0      ; End Address of DARAM0
        .asg    0x0668,A_CDP_DARAM0    ; Test Coef DARAM0

        .asg    0x1000,STA_DARAM1      ; Start Address of DARAM1
        .asg    0x1001,SECT_DARAM1     ; Start Address of the section DARAM1
        .asg    0x1800,TA0_DARAM1      ; Test Address 0 of DARAM1
        .asg    0x1801,TA1_DARAM1      ; Test Address 1 of DARAM1
        .asg    0x1802,TA2_DARAM1      ; Test Address 2 of DARAM1
        .asg    0x1803,TA3_DARAM1      ; Test Address 3 of DARAM1
        .asg    0x1fff,END_DARAM1      ; End Address of DARAM1
        .asg    0x1668,A_CDP_DARAM1    ; Test Coef DARAM1

        .asg    0x2000,STA_DARAM2      ; Start Address of DARAM2
        .asg    0x2001,SECT_DARAM2     ; Start Address of the section DARAM2
        .asg    0x2800,TA0_DARAM2      ; Test Address 0 of DARAM2
        .asg    0x2801,TA1_DARAM2      ; Test Address 1 of DARAM2
        .asg    0x2802,TA2_DARAM2      ; Test Address 2 of DARAM2
        .asg    0x2803,TA3_DARAM2      ; Test Address 3 of DARAM2
        .asg    0x2fff,END_DARAM2      ; End Address of DARAM2
        .asg    0x2668,A_CDP_DARAM2    ; Test Coef DARAM2

        .asg    0x3000,STA_DARAM3      ; Start Address of DARAM3
        .asg    0x3001,SECT_DARAM3     ; Start Address of the section DARAM3
        .asg    0x3800,TA0_DARAM3      ; Test Address 0 of DARAM3
        .asg    0x3801,TA1_DARAM3      ; Test Address 1 of DARAM3
        .asg    0x3802,TA2_DARAM3      ; Test Address 2 of DARAM3
        .asg    0x3803,TA3_DARAM3      ; Test Address 3 of DARAM3
        .asg    0x3fff,END_DARAM3      ; End Address of DARAM3
        .asg    0x3668,A_CDP_DARAM3    ; Test Coef DARAM3

        .asg    0x4001,SECT_DARAM4     ; Start Address of the section DARAM4
        .asg    0x4800,TA0_DARAM4      ; Test Address 0 of DARAM4
        .asg    0x4801,TA1_DARAM4      ; Test Address 1 of DARAM4
        .asg    0x4802,TA2_DARAM4      ; Test Address 2 of DARAM4
        .asg    0x4803,TA3_DARAM4      ; Test Address 3 of DARAM4
        .asg    0x4fff,END_DARAM4      ; End Address of DARAM4
        .asg    0x4668,A_CDP_DARAM4    ; Test Coef DARAM4

        .asg    0x5000,STA_DARAM5      ; Start Address of DARAM5
        .asg    0x5001,SECT_DARAM5     ; Start Address of the section DARAM5
        .asg    0x5800,TA0_DARAM5      ; Test Address 0 of DARAM5
        .asg    0x5801,TA1_DARAM5      ; Test Address 1 of DARAM5
        .asg    0x5802,TA2_DARAM5      ; Test Address 2 of DARAM5
        .asg    0x5803,TA3_DARAM5      ; Test Address 3 of DARAM5
        .asg    0x5fff,END_DARAM5      ; End Address of DARAM5
        .asg    0x5668,A_CDP_DARAM5    ; Test Coef DARAM5

        .asg    0x6000,STA_DARAM6      ; Start Address of DARAM6
        .asg    0x6001,SECT_DARAM6     ; Start Address of the section DARAM6
        .asg    0x6800,TA0_DARAM6      ; Test Address 0 of DARAM6
        .asg    0x6801,TA1_DARAM6      ; Test Address 1 of DARAM6
        .asg    0x6802,TA2_DARAM6      ; Test Address 2 of DARAM6
        .asg    0x6803,TA3_DARAM6      ; Test Address 3 of DARAM6
        .asg    0x6fff,END_DARAM6      ; End Address of DARAM6
        .asg    0x6668,A_CDP_DARAM6    ; Test Coef DARAM6

        .asg    0x7000,STA_DARAM7      ; Start Address of DARAM7
        .asg    0x7001,SECT_DARAM7     ; Start Address of the section DARAM7
        .asg    0x7800,TA0_DARAM7      ; Test Address 0 of DARAM7
        .asg    0x7801,TA1_DARAM7      ; Test Address 1 of DARAM7
        .asg    0x7802,TA2_DARAM7      ; Test Address 2 of DARAM7
        .asg    0x7803,TA3_DARAM7      ; Test Address 3 of DARAM7
        .asg    0x7fff,END_DARAM7      ; End Address of DARAM7
        .asg    0x7668,A_CDP_DARAM7    ; Test Coef DARAM7

        .asg    0x8000,STA_DARAM8      ; Start Address of DARAM7
        .asg    0x8001,SECT_DARAM8     ; Start Address of the section DARAM7
        .asg    0x8800,TA0_DARAM8      ; Test Address 0 of DARAM7
        .asg    0x8801,TA1_DARAM8      ; Test Address 1 of DARAM7
        .asg    0x8802,TA2_DARAM8      ; Test Address 2 of DARAM7
        .asg    0x8803,TA3_DARAM8      ; Test Address 3 of DARAM7
        .asg    0x8fff,END_DARAM8      ; End Address of DARAM7
        .asg    0x8668,A_CDP_DARAM8    ; Test Coef DARAM7

        .asg    0x9000,STA_DARAM9      ; Start Address of DARAM7
        .asg    0x9001,SECT_DARAM9     ; Start Address of the section DARAM7
        .asg    0x9800,TA0_DARAM9      ; Test Address 0 of DARAM7
        .asg    0x9801,TA1_DARAM9      ; Test Address 1 of DARAM7
        .asg    0x9802,TA2_DARAM9      ; Test Address 2 of DARAM7
        .asg    0x9803,TA3_DARAM9      ; Test Address 3 of DARAM7
        .asg    0x9fff,END_DARAM9      ; End Address of DARAM7
        .asg    0x9668,A_CDP_DARAM9    ; Test Coef DARAM7

        .asg    0xA000,STA_DARAM10      ; Start Address of DARAM7
        .asg    0xA001,SECT_DARAM10     ; Start Address of the section DARAM7
        .asg    0xA800,TA0_DARAM10      ; Test Address 0 of DARAM7
        .asg    0xA801,TA1_DARAM10      ; Test Address 1 of DARAM7
        .asg    0xA802,TA2_DARAM10      ; Test Address 2 of DARAM7
        .asg    0xA803,TA3_DARAM10      ; Test Address 3 of DARAM7
        .asg    0xAfff,END_DARAM10      ; End Address of DARAM7
        .asg    0xA668,A_CDP_DARAM10    ; Test Coef DARAM7

        .asg    0xB000,STA_DARAM11      ; Start Address of DARAM7
        .asg    0xB001,SECT_DARAM11     ; Start Address of the section DARAM7
        .asg    0xB800,TA0_DARAM11      ; Test Address 0 of DARAM7
        .asg    0xB801,TA1_DARAM11      ; Test Address 1 of DARAM7
        .asg    0xB802,TA2_DARAM11      ; Test Address 2 of DARAM7
        .asg    0xB803,TA3_DARAM11      ; Test Address 3 of DARAM7
        .asg    0xBfff,END_DARAM11      ; End Address of DARAM7
        .asg    0xB668,A_CDP_DARAM11    ; Test Coef DARAM7



****** HERE map MAIN  in SARAM0 ******

        .asg    0x8000,STA_SARAM0      ; Start Address of SARAM0


****** HERE map when DMA available in SARAM0 ******

        .asg    0x8800,DM0_SARAM0      ; Start Address 0 of SARAM0
                                       ; for DMA transfers
        .asg    0x8900,DM1_SARAM0      ; Start Address 1 of SARAM0
                                       ; for DMA transfers
        .asg    0x8a00,DM2_SARAM0      ; Start Address 2 of SARAM0
                                       ; for DMA transfers
        .asg    0x8b00,DM3_SARAM0      ; Start Address 3 of SARAM0
                                       ; for DMA transfers
        .asg    0x8c00,DM4_SARAM0      ; Start Address 4 of SARAM0
                                       ; for DMA transfers
        .asg    0x8d00,DM5_SARAM0      ; Start Address 5 of SARAM0
                                       ; for DMA transfers
        .asg    0x8e00,DM6_SARAM0      ; Start Address 6 of SARAM0
                                       ; for DMA transfers
        .asg    0x8f00,DM7_SARAM0      ; Start Address 7 of SARAM0
                                       ; for DMA transfers

*Segment 03 - Page 31 mapping:

        .asg    0xC000,STA_D_PDROM      ; Start Address  of PDROM
        .asg    0xFF00,TA0_D_PDROM      ; Test Address 0 of PDROM
        .asg    0xFF01,TA1_D_PDROM      ; Test Address 1 of PDROM
        .asg    0xFF02,A_CDP_PDROM     ; Test Address 2 of PDROM
        .asg    0xFF03,TA2_D_PDROM      ; Test Address 3 of PDROM
        .asg    0xFF04,TA3_D_PDROM      ; Test Address 4 of PDROM



*Definition of external memory test addresses:

*Segment 00 - Page 2 mapping:

        .asg    0x8000,STA_EXTERN0      ; Start Address of EXTERN0 (CE0)
        .asg    0x8001,SEC_EXTERN0      ; Start Address of section EXTERN0 (CE0)

*Segment 00 - Page 31 mapping:

        .asg    0xFFFF,END_EXTERN0      ; END Address of EXTERN0 (CE0)

*Segment 01 - Page 0 mapping:

        .asg    0x0080,STA_EXTERN1      ; Start Address of EXTERN1 (CE1)
        .asg    0x0081,SEC_EXTERN1      ; Start Address of section EXTERN1 (CE1)

*Segment 01 - Page 31 mapping:

        .asg    0xFFFF,END_EXTERN1      ; END Address of EXTERN1 (CE1)

*Segment 10 - Page 0 mapping:

        .asg    0x0080,STA_EXTERN2      ; Start Address of EXTERN2 (CE2)
        .asg    0x0081,SEC_EXTERN2      ; Start Address of section EXTERN2 (CE2)

*Segment 10 - Page 31 mapping:

        .asg    0xFFFF,END_EXTERN2      ; END Address of EXTERN2 (CE2)

*Segment 11 - Page 0 mapping:

        .asg    0x0080,STA_EXTERN3      ; Start Address of EXTERN3 (CE3)
        .asg    0x0081,SEC_EXTERN3      ; Start Address of section EXTERN3 (CE3)

*Segment 11 - Page 31 mapping:

        .asg    0xBFFF,END_EXTERN3      ; END Address of EXTERN3 (CE3)



*Segment 11 - Page 31 mapping:
*In this page (64kw):
*definition of start and end addresses for boot code:

        .asg    0x7fff80,RESET_VECTOR  ; Address of Reset vector
        .asg    0x7fff84,NMI_VECTOR    ; Address of NMI interrupt
        .asg    0x7fff88,PINT0_VECTOR  ; Address of PINT0 peripheral interrupt
        .asg    0x7fff8c,PINT1_VECTOR  ; Address of PINT1 peripheral interrupt
        .asg    0x7fff8c,DMA_VECTOR    ; Address of re-looped DMA interrupt
        .asg    0x7fff90,PINT2_VECTOR  ; Address of PINT2 peripheral interrupt
        .asg    0x7fff94,PINT3_VECTOR  ; Address of PINT3 peripheral interrupt
        .asg    0x7fff98,PINT4_VECTOR  ; Address of PINT4 peripheral interrupt
        .asg    0x7fff9c,PINT5_VECTOR  ; Address of PINT5 peripheral interrupt
        .asg    0x7fffa0,PINT6_VECTOR  ; Address of PINT6 peripheral interrupt
        .asg    0x7fffa4,PINT7_VECTOR  ; Address of PINT7 peripheral interrupt
        .asg    0x7fffa8,PINT8_VECTOR  ; Address of PINT8 peripheral interrupt
        .asg    0x7fffac,PINT9_VECTOR  ; Address of PINT9 peripheral interrupt
        .asg    0x7fffb0,PINT10_VECTOR ; Address of PINT10 peripheral interrupt
        .asg    0x7fffb4,PINT11_VECTOR ; Address of PINT11 peripheral interrupt
        .asg    0x7fffb8,PINT12_VECTOR ; Address of PINT12 peripheral interrupt
        .asg    0x7fffbc,PINT13_VECTOR ; Address of PINT13 peripheral interrupt
        .asg    0x7fffc0,MINT0_VECTOR  ; Address of MINT0 MCU interrupt
        .asg    0x7fffc4,MINT1_VECTOR  ; Address of MINT1 MCU interrupt
        .asg    0x7fffc8,MINT2_VECTOR  ; Address of MINT2 MCU interrupt
        .asg    0x7fffcc,MINT3_VECTOR  ; Address of MINT3 MCU interrupt
        .asg    0x7fffd0,MINT4_VECTOR  ; Address of MINT4 MCU interrupt
        .asg    0x7fffd4,MINT5_VECTOR  ; Address of MINT5 MCU interrupt
        .asg    0x7fffd8,MINT6_VECTOR  ; Address of MINT6 MCU interrupt
        .asg    0x7fffdc,MINT7_VECTOR  ; Address of MINT7 MCU interrupt
        .asg    0x7fffe0,BERR_VECTOR   ; Address of Bus error interrupt

*For any (Segment + Page):
*Definition of external memory test addresses:


        .asg    0xac00,TA0_EXTERN      ; Test Address 0 of EXTERN
        .asg    0xac01,TA1_EXTERN      ; Test Address 1 of EXTERN
        .asg    0xac02,TA2_EXTERN      ; Test Address 2 of EXTERN
        .asg    0xac03,TA3_EXTERN      ; Test Address 3 of EXTERN
        .asg    0xac04,TA4_EXTERN      ; Test Address 4 of EXTERN

        .asg    0x8000,SECT_DMA_EXTERN   ; Start Address 1 of EXTERN3
                                        ; for DMA transfers

        .asg    0x0800,DM3_EXTERN1      ; Start Address 1 of EXTERN3
                                        ; for DMA transfers
        .asg    0x0B00,DM3_EXTERN2      ; Start Address 2 of EXTERN3
                                        ; for DMA transfers
        .asg    0x0A00,DM3_EXTERN3      ; Start Address 3 of EXTERN3
                                        ; for DMA transfers
        .asg    0x0B00,DM3_EXTERN4      ; Start Address 4 of EXTERN3
                                        ; for DMA transfers

****************************************************************************************************
*       SECTION 1 : I/O space mapping
****************************************************************************************************
*
* In case of peripheral access through readport() or writeport() qualification, all main
* data page registers (MDP, MDP05, MDP67) are masked and the MSB of the address
* is forced to "0".
*
*       Data Address field:
*
*       |       |       |       |       |       |       |
*       -------------------------------------------------
*       |X|X|X|X|X|X|X|0| | | | | | | | | | | | | | | |X|
*       -------------------------------------------------
*       |             |                               | |
*       |   [23:17]   |             [16:1]            |0|
*       |             |                               | |
*
*              |                       |               |
*              |                       |               |
*              V                       V               V
*
*          Main Data            16 bit pointer      Unused
*            Page              (word addressing)
*           Masked
*
* I/O SPACE MAPPING:
*
*       -----------------------------------
*       | Start Addr. |  Peripheral Name  |
*       ----------------------------------
*       -----------------------------------
*       | 0x0000      |  UMA Registers    |
*       |             |                   |
*       -----------------------------------
*       | 0x0C00      |  DMA0             |
*       |             |                   |
*       -----------------------------------
*       | 0x0E00      |  DMA1             |
*       |             |                   |
*       -----------------------------------
*       | 0x1800      |  Timer0           |
*       |             |                   |
*       -----------------------------------
*       | 0x1840      |  Timer1           |
*       |             |                   |
*       -----------------------------------
*       | 0x1880      |  Timer2           |
*       |             |                   |
*       -----------------------------------
*       | 0x1900      |  RTC              |
*       |             |                   |
*       -----------------------------------
*       | 0x1C00      |  System Control   |
*       |             |                   |
*       -----------------------------------
*       | 0x2000      |  Emulation Trace  |
*       |             |                   |
*       -----------------------------------
*       | 0x2800      |  I2S              |
*       |             |                   |
*       -----------------------------------
*       | 0x3000      |  SPI              |
*       |             |                   |
*       -----------------------------------
*       | 0x3C00      |  I2C              |
*       |             |                   |
*       -----------------------------------
*       | 0x9B00      |  Power Management |
*       |             |                   |
*       -----------------------------------
*       | 0x9C00      |  UART             |
*       |             |                   |
*       -----------------------------------
*       | 0x9D00      |  Analog           |
*       |             |                   |
*       -----------------------------------
*       | 0xA000      |  Coproc RAM0      |
*       |             |                   |
*       -----------------------------------
*       | 0xA000      |  Coproc RAM0      |
*       |             |                   |
*       -----------------------------------
*       | 0xA400      |  Coproc RAM1      |
*       |             |                   |
*       -----------------------------------
*       | 0xA800      |  Coproc RAM2      |
*       |             |                   |
*       -----------------------------------
*       | 0xAC00      |  Coproc RAM3      |
*       |             |                   |
*       -----------------------------------
*       | 0xB000      |  Coproc ROM       |
*       |             |                   |
*       -----------------------------------
*       | 0xB800      |  Analysis RAM0    |
*       |             |                   |
*       -----------------------------------
*       | 0xBA00      |  Analysis RAM1    |
*       |             |                   |
*       -----------------------------------
*       | 0xBC00      |  Analysis RAM2    |
*       |             |                   |
*       -----------------------------------
*       | 0xC000      |  Coproc Control   |
*       |             |                   |
*       -----------------------------------
*
*                                      .
*                                      .
*                                      .
*
*       ---------------------------------------------------------------
*       | 0xfC00      | Peripheral_63              | ...              |
*       |             | (1 kwords)                 |                  |
*       ---------------------------------------------------------------
*
* Addressable I/O Space: 64 K = 32 x 2Kbytes 0r 32 x 1 Kwords
*
****************************************************************************************************

**********************************************************************************************
* UMA I/O SPACE MAPPING:
**********************************************************************************************

*           WORD ADDRESS, NAME  ; DESCRIPTION
        .asg    0x0000, CMR_        ; XPORT Control Mode Register
        .asg    0x0001, ICR_        ; SYSTEM Idle Control Register
        .asg    0x0002, ISR_        ; SYSTEM Idle Status Register
        .asg    0x0003, IMSCR_
        .asg    0x0004, IMSSTR_
        ;   0x0005 - 0x000D     ; Reserved
        .asg    0x000E, APISIZE_    ; SYSTEM Shared Memory Enable Register pins
        .asg    0x000F, BOOTMOD_    ; SYSTEM Boot Mode Register pins
        ;   0x0010 - 0x00FF     ; Reserved
        .asg    0x0100, XCR_        ; XPORT XPORT Configuration Register
        ;   0x0101              ; Reserved
        .asg    0x0102, XERR_       ; XPORT XPORT Bus Error Register
        .asg    0x0103, XPSA0_      ; XPORT XPORT PSA Register 0
        .asg    0x0104, XPSA1_      ; XPORT XPORT PSA Register 1
        .asg    0x0105, XPSA2_      ; XPORT XPORT PSA Register 2
        ;   0x0106 - 0x01FF     ; Reserved
        .asg    0x0200, DPSA0_      ; DPORT PSA Configuration Register
        ;   0x0201              ; Reserved
        .asg    0x0203, DPSA0_      ; DPORT DPORT PSA Register 0
        .asg    0x0204, DPSA1_      ; DPORT DPORT PSA Register 1
        .asg    0x0205, DPSA2_      ; DPORT DPORT PSA Register 2
        ;   0x0206 - 0x0301     ; Reserved
        .asg    0x0302, IERR_       ; IPORT IPORT Bus Error Register
        .asg    0x0303, IPSA0_      ; IPORT IPORT PSA Register 0
        .asg    0x0304, IPSA1_      ; IPORT IPORT PSA Register 1
        ;   0x0305 - 0x030F     ; Reserved
        .asg    0x0310, TINT0_      ; SYSTEM Test Interrupt Register 0
        .asg    0x0311, TINT1_      ; SYSTEM Test Interrupt Register 1
        .asg    0x0312, TINT2_      ; SYSTEM Test Interrupt Register 0
        .asg    0x0313, TINT3_      ; SYSTEM Test Interrupt Register 1
        ;   0x0312 - 0x03FF     ; Reserved
        ;   0x0400 - 0x07F8  EMU EMU Emulation Registers
        .asg    0x0420, APSA_H      ; APSA[40 : 32]
        .asg    0x0423, APSA_M      ; APSA[31 : 16]
        .asg    0x0422, APSA_L      ; APSA[15 :  0]
        .asg    0x0426, APSA_CTL    ; APSA CNTL [4 : 0]
        .asg    0x0427, APSA_ID     ;
        .asg    0x0440, DPSA_H  ; DPSA[40 : 32]
        .asg    0x0443, DPSA_M      ; DPSA[31 : 16]
        .asg    0x0442, DPSA_L      ; DPSA[15 :  0]
        .asg    0x0446, DPSA_CTL    ; DPSA CNTL [4 : 0]
        .asg    0x0447, DPSA_ID
        .asg    0x05fe, TPSA_H      ; TPSA[40 : 32]
        .asg    0x05fd, TPSA_M      ; TPSA[31 : 16]
        .asg    0x05fc, TPSA_L      ; TPSA[15 :  0]
        .asg    0x05ff, TPSA_CTL    ; APSA CNTL [2 : 0]
        ;                           ;
        .asg    0x07F9, SPSA0_      ; SYSTEM PSA Register 0
        .asg    0x07FA, SPSA1_      ; SYSTEM PSA Register 1
        .asg    0x07FB, TIDLE_      ; SYSTEM Idle Testing Register
        .asg    0x07FC, RESET_      ; SYSTEM System Reset Register
        .asg    0x07FD, CONFIG_     ; SYSTEM System Configuration Register
        .asg    0x07FE, TMCR0_      ; TPORT Test Mode Control Register 0
        .asg    0x07FF, TMCR1_      ; TPORT Test Mode Control Register 1
        .asg    0x0800, DCR_        ; DPORT DPORT Configuration Register
        ;       0x0801              ; Reserved
        .asg    0x0802, DERR_       ; DPORT DPORT Bus Error Register
        ;       0x0803 - 0x0BFF     ; Reserved
        ;       0x0C00 - 0x13FF     ;
        .asg    0x1400, ICGCR_      ; ICACHE Global Control Register
        .asg    0x1401, ICFLAR0_    ; ICACHE Flush Line Address Register 0
        .asg    0x1402, ICFLAR1_    ; ICACHE Flush Line Address Register 1
        .asg    0x1403, ICNWCR_     ; ICACHE N-Way Control Register
        .asg    0x1404, ICSR_       ; ICACHE Status Register
        .asg    0x1405, ICRCR1_     ; ICACHE Ramset 1 Control Register
        .asg    0x1406, ICRTR1_     ; ICACHE Ramset 1 TAG Register
        .asg    0x1407, ICRCR2_     ; ICACHE Ramset 2 Control Register
        .asg    0x1408, ICRTR2_     ; ICACHE Ramset 2 TAG Register
        .asg    0x1409, INWMC_      ; ICACHE N-Way Miss Counter
        ;       0x1410 - 0x17FF     ; Reserved
        ;       0x1800 - 0x1FFF     ; XPORT External Request.
        ;       0x2000 - 0x2054     ; TRACE EMU Trace Registers N/A
        ;       0x2055 - 0x23FF     ; Reserved
        ;       0x2400 - 0x7FFF     ; XPORT External Request.
        ;       0x8000 - 0xFFFF     ; ICACHE RAM (ictest=1)
* Icache Data & Tag Banks
        .asg    0x008000, ICACHE_DATA0  ; Start Address of icache Data Bank0
        .asg    0x009000, ICACHE_DATA1  ; Start Address of icache Data Bank1
        .asg    0x00A000, ICACHE_DATA2  ; Start Address of icache Data Bank2
        .asg    0x00B000, ICACHE_TAG1   ; Start Address of icache Tag Bank1
        .asg    0x00B200, ICACHE_TAG2   ; Start Address of icache Tag Bank2


*API registers

*APIREG symbols used for API (not I/O mapped registers)

*API_IR symbols

        .asg    0x0000,API_IR

        .eval   (0000000000000000b), API_IR_RESET_VALUE

*API_SR symbols

        .asg    0x0001,API_SR

        .asg    00,API_SR_MODE_SAM_A_SAM_P
        .asg    01,API_SR_MODE_SAM_A_HOM_P
        .asg    10,API_SR_MODE_HOM_A_SAM_P
        .asg    11,API_SR_MODE_HOM_A_HOM_P
        .asg    00,API_SR_MODE_RESET

        .asg    0,API_SR_WRITE_POSTING_DIS
        .asg    1,API_SR_WRITE_POSTING_ENA
        .asg    0,API_SR_WRITE_POSTING_RESET

        .eval   (0000000000000:API_SR_MODE_RESET::API_SR_WRITE_POSTING_RESET:b), API_SR_RESET_VALUE

*RHEA registers

*REA_CMR symbols

        .asg    0x0000,REA_CMR         ; Address of REA_CMR

        .asg    0,REA_CMR_MODE_SAM
        .asg    1,REA_CMR_MODE_HOM
        .asg    0,REA_CMR_MODE_RESET

        .asg    0,REA_CMR_BUS_ERROR_CLEARED
        .asg    1,REA_CMR_BUS_ERROR_SET
        .asg    0,REA_CMR_BUS_ERROR_RESET

        .asg    0,REA_CMR_PRIORITY_ROTATING
        .asg    1,REA_CMR_PRIORITY_FIRST
        .asg    1,REA_CMR_PRIORITY_RESET

        .asg    001,REA_CMR_WAIT_STATES_1_RESET
        .asg    001,REA_CMR_WAIT_STATES_2_RESET

        .asg    0000000,REA_CMR_TIME_OUT_DIS
        .asg    1111111,REA_CMR_TIME_OUT_RESET

        .eval   (:REA_CMR_TIME_OUT_RESET::REA_CMR_WAIT_STATES_2_RESET::REA_CMR_WAIT_STATES_1_RESET::REA_CMR_PRIORITY_RESET::REA_CMR_BUS_ERROR_RESET::REA_CMR_MODE_RESET:b), REA_CMR_RESET_VALUE
        .eval   (:REA_CMR_TIME_OUT_RESET::REA_CMR_WAIT_STATES_2_RESET::REA_CMR_WAIT_STATES_1_RESET::REA_CMR_PRIORITY_RESET::REA_CMR_BUS_ERROR_RESET::REA_CMR_MODE_HOM:b),REA_CMR_HOM_RESET_VALUE

*REA_ICR symbols

        .asg    0x0001,REA_ICR         ; Address of REA_ICR

        .asg    0,REA_ICR_CPU_UNSET
        .asg    1,REA_ICR_CPU_SET
        .asg    0,REA_ICR_CPU_RESET

        .asg    0,REA_ICR_DMA_UNSET
        .asg    1,REA_ICR_DMA_SET
        .asg    0,REA_ICR_DMA_RESET

        .asg    0,REA_ICR_CACHE_UNSET
        .asg    1,REA_ICR_CACHE_SET
        .asg    0,REA_ICR_CACHE_RESET

        .asg    0,REA_ICR_PERH_UNSET
        .asg    1,REA_ICR_PERH_SET
        .asg    0,REA_ICR_PERH_RESET

        .asg    0,REA_ICR_DPLL_UNSET
        .asg    1,REA_ICR_DPLL_SET
        .asg    0,REA_ICR_DPLL_RESET

        .asg    0,REA_ICR_DOMAIN5_UNSET
        .asg    1,REA_ICR_DOMAIN5_SET
        .asg    0,REA_ICR_DOMAIN5_RESET

        .asg    0,REA_ICR_DOMAIN6_UNSET
        .asg    1,REA_ICR_DOMAIN6_SET
        .asg    0,REA_ICR_DOMAIN6_RESET

        .asg    0,REA_ICR_DOMAIN7_UNSET
        .asg    1,REA_ICR_DOMAIN7_SET
        .asg    0,REA_ICR_DOMAIN7_RESET

        .eval   (00000000:REA_ICR_DOMAIN7_RESET::REA_ICR_DOMAIN6_RESET::REA_ICR_DOMAIN5_RESET::REA_ICR_DPLL_RESET::REA_ICR_PERH_RESET::REA_ICR_CACHE_RESET::REA_ICR_DMA_RESET::REA_ICR_CPU_RESET:b),REA_ICR_RESET_VALUE

*REA_ISTR symbols

        .asg    0x0002,REA_ISTR        ; Address of REA_ISTR

        .asg    0,REA_ISTR_CPU_UNSET
        .asg    1,REA_ISTR_CPU_SET
        .asg    0,REA_ISTR_CPU_RESET

        .asg    0,REA_ISTR_DMA_UNSET
        .asg    1,REA_ISTR_DMA_SET
        .asg    0,REA_ISTR_DMA_RESET

        .asg    0,REA_ISTR_CACHE_UNSET
        .asg    1,REA_ISTR_CACHE_SET
        .asg    0,REA_ISTR_CACHE_RESET

        .asg    0,REA_ISTR_PERH_UNSET
        .asg    1,REA_ISTR_PERH_SET
        .asg    0,REA_ISTR_PERH_RESET

        .asg    0,REA_ISTR_DPLL_UNSET
        .asg    1,REA_ISTR_DPLL_SET
        .asg    0,REA_ISTR_DPLL_RESET

        .asg    0,REA_ISTR_DOMAIN5_UNSET
        .asg    1,REA_ISTR_DOMAIN5_SET
        .asg    0,REA_ISTR_DOMAIN5_RESET

        .asg    0,REA_ISTR_DOMAIN6_UNSET
        .asg    1,REA_ISTR_DOMAIN6_SET
        .asg    0,REA_ISTR_DOMAIN6_RESET

        .asg    0,REA_ISTR_DOMAIN7_UNSET
        .asg    1,REA_ISTR_DOMAIN7_SET
        .asg    0,REA_ISTR_DOMAIN7_RESET

        .eval   (00000000:REA_ISTR_DOMAIN7_RESET::REA_ISTR_DOMAIN6_RESET::REA_ISTR_DOMAIN5_RESET::REA_ISTR_DPLL_RESET::REA_ISTR_PERH_RESET::REA_ISTR_CACHE_RESET::REA_ISTR_DMA_RESET::REA_ISTR_CPU_RESET:b),REA_ISTR_RESET_VALUE

  .asg  21,NUMBER_PERIPH_INT

*Peripheral interrupt number symbols

  .asg  00000,PERIPH_INT0
  .asg  00001,PERIPH_INT1
  .asg  00010,PERIPH_INT2
  .asg  00011,PERIPH_INT3
  .asg  00100,PERIPH_INT4
  .asg  00101,PERIPH_INT5
  .asg  00110,PERIPH_INT6
  .asg  00111,PERIPH_INT7
  .asg  01000,PERIPH_INT8
  .asg  01001,PERIPH_INT9
  .asg  01010,PERIPH_INT10
  .asg  01011,PERIPH_INT11
  .asg  01100,PERIPH_INT12
  .asg  01101,PERIPH_INT13
  .asg  01110,PERIPH_INT14
  .asg  01111,PERIPH_INT15
  .asg  10000,PERIPH_INT16
  .asg  10001,PERIPH_INT17
  .asg  10010,PERIPH_INT18
  .asg  10011,PERIPH_INT19
  .asg  10100,PERIPH_INT20
  .asg  10101,PERIPH_INT21

*REA_PER1 symbols
        .asg    0x0003,REA_PER1        ; Address of REA_PER1
        .eval   (000:PERIPH_INT1:000:PERIPH_INT0:b),REA_PER1_RESET_VALUE

*REA_PER2 symbols
        .asg    0x0004,REA_PER2        ; Address of REA_PER2
        .eval   (000:PERIPH_INT12:000:PERIPH_INT2:b),REA_PER2_RESET_VALUE

*REA_PER3 symbols
        .asg    0x0005,REA_PER3        ; Address of REA_PER3
        .eval   (000:PERIPH_INT7:000:PERIPH_INT6:b),REA_PER3_RESET_VALUE

*REA_PER4 symbols
        .asg    0x0006,REA_PER4        ; Address of REA_PER4
        .eval   (000:PERIPH_INT9:000:PERIPH_INT8:b),REA_PER4_RESET_VALUE

*REA_PER5 symbols
        .asg    0x0007,REA_PER5        ; Address of REA_PER5
        .eval   (000:PERIPH_INT15:000:PERIPH_INT14:b),REA_PER5_RESET_VALUE

*REA_PER6 symbols
        .asg    0x0008,REA_PER6        ; Address of REA_PER6
        .eval   (000:PERIPH_INT21:000:PERIPH_INT16:b),REA_PER6_RESET_VALUE

*REA_PER7 symbols
        .asg    0x0009,REA_PER7        ; Address of REA_PER7
        .eval   (000:PERIPH_INT4:000:PERIPH_INT3:b),REA_PER7_RESET_VALUE

*REA_PER8 symbols
        .asg    0x000A,REA_PER8        ; Address of REA_PER8
        .eval   (000:PERIPH_INT11:000:PERIPH_INT10:b),REA_PER8_RESET_VALUE

*REA_PER9 symbols
        .asg    0x000B,REA_PER9        ; Address of REA_PER9
        .eval   (000:PERIPH_INT18:000:PERIPH_INT17:b),REA_PER9_RESET_VALUE

*REA_PER10 symbols
        .asg    0x000C,REA_PER10        ; Address of REA_PER10
        .eval   (000:PERIPH_INT20:000:PERIPH_INT19:b),REA_PER10_RESET_VALUE

*REA_PER11 symbols
        .asg    0x000D,REA_PER11        ; Address of REA_PER11
        .eval   (000:PERIPH_INT5:000:PERIPH_INT13:b),REA_PER11_RESET_VALUE


*REA_API_SIZE symbols
        .asg    0x000E,REA_API_SIZE        ; Address of REA_API_SIZE
        .asg    1111111111111111,REA_API_SIZE_RESET
        .eval   (000000000000:REA_API_SIZE_RESET:b),REA_API_SIZE_RESET_VALUE


*REA_BOOT_MOD symbols
        .asg    0x000F,REA_BOOT_MOD        ; Address of REA_BOOT_MOD
* NO RESET VALUE FOR REA_BOOT_MOD BECAUSE IT DEPENDS ON EXTERNAL IMPLEMENTATION


*REA_BOOT_RHEA_PTR symbols
        .asg    0x0010,REA_BOOT_RHEA_PTR        ; Address of REA_BOOT_RHEA_PTR
* NO RESET VALUE FOR REA_BOOT_RHEA_PTR BECAUSE IT DEPENDS ON EXTERNAL IMPLEMENTATION


*REA_BOOT_RHEA_PTR2 symbols
        .asg    0x0011,REA_BOOT_RHEA_PTR2        ; Address of REA_BOOT_RHEA_PTR2
* NO RESET VALUE FOR REA_BOOT_RHEA_PTR2 BECAUSE IT DEPENDS ON EXTERNAL IMPLEMENTATION



*
*To be done next...
*






*************************************************
*Standard peripheral  symbols
*************************************************

        .asg    0x4000,STA_INEPEH      ; Start Address of Inexistant Peripheral INEPEH
        .asg    0x4000,TA0_INEPEH      ; Test Address 0 of Inexistant Peripheral INEPEH
        .asg    0x4001,TA1_INEPEH      ; Test Address 1 of Inexistant Peripheral INEPEH
        .asg    0x4002,TA2_INEPEH      ; Test Address 2 of Inexistant Peripheral INEPEH
        .asg    0x4003,TA3_INEPEH      ; Test Address 3 of Inexistant Peripheral INEPEH

*************************************************
* First peripheral
*************************************************

        .asg    0x4800,STA_STDPEH1      ; Start Address of Standard Peripheral STDPEH1

        .asg    0x4800,DM0_STDPEH1      ; Start Address 0 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4900,DM1_STDPEH1      ; Start Address 1 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4a00,DM2_STDPEH1      ; Start Address 2 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4b00,DM3_STDPEH1      ; Start Address 3 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4c00,DM4_STDPEH1      ; Start Address 4 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4d00,DM5_STDPEH1      ; Start Address 5 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4e00,DM6_STDPEH1      ; Start Address 6 of STDPEH1
                                        ; for DMA transfers
        .asg    0x4f00,DM7_STDPEH1      ; Start Address 7 of STDPEH1
                                        ; for DMA transfers

        .asg    0x4900,TA0_STDPEH1      ; Test Address 0 of Standard Peripheral STDPEH1
        .asg    0x4901,TA1_STDPEH1      ; Test Address 1 of Standard Peripheral STDPEH1
        .asg    0x4902,TA2_STDPEH1      ; Test Address 2 of Standard Peripheral STDPEH1
        .asg    0x4903,TA3_STDPEH1      ; Test Address 3 of Standard Peripheral STDPEH1
        .asg    0x4bff,END_STDPEH1      ; End Address of Standard Peripheral STDPEH1

*************************************************
* Second peripheral
*************************************************

        .asg    0xd800,STA_STDPEH2      ; Start Address of Standard Peripheral STDPEH2

        .asg    0xd800,DM0_STDPEH2      ; Start Address 0 of STDPEH2
                                        ; for DMA transfers
        .asg    0xd900,DM1_STDPEH2      ; Start Address 1 of STDPEH2
                                        ; for DMA transfers
        .asg    0xda00,DM2_STDPEH2      ; Start Address 2 of STDPEH2
                                        ; for DMA transfers
        .asg    0xdb00,DM3_STDPEH2      ; Start Address 3 of STDPEH2
                                        ; for DMA transfers
        .asg    0xdc00,DM4_STDPEH2      ; Start Address 4 of STDPEH2
                                        ; for DMA transfers
        .asg    0xdd00,DM5_STDPEH2      ; Start Address 5 of STDPEH2
                                        ; for DMA transfers
        .asg    0xde00,DM6_STDPEH2      ; Start Address 6 of STDPEH2
                                        ; for DMA transfers
        .asg    0xdf00,DM7_STDPEH2      ; Start Address 7 of STDPEH2
                                        ; for DMA transfers

        .asg    0xd900,TA0_STDPEH2      ; Test Address 0 of Standard Peripheral STDPEH2
        .asg    0xd901,TA1_STDPEH2      ; Test Address 1 of Standard Peripheral STDPEH2
        .asg    0xd902,TA2_STDPEH2      ; Test Address 2 of Standard Peripheral STDPEH2
        .asg    0xd903,TA3_STDPEH2      ; Test Address 3 of Standard Peripheral STDPEH2
        .asg    0xdbff,END_STDPEH2      ; End Address of Standard Peripheral STDPEH2


* GTA Modified Dec 21, 1999
****************************************************************************************************
* SECTION 2 : Memory Mapped Registers mapping
****************************************************************************************************
*
*        WORD ADDRESS ,NAME            ; DESCRIPTION            ; FIELD
*

        .asg    0x0000,IMR0_           ; Interrupt Enable Register 0      ; [15:00]
        .asg    0x0001,IFR0_           ; Interrupt Flag Register 0      ; [15:00]
        .asg    0x0002,ST0_            ; Status Register 0              ; [15:00]
        .asg    0x0003,ST1_            ; Status Register 1              ; [15:00]
        .asg    0x0004,ST3_            ; Status Register 3                              ; [15:00]
*.......RESERVED.................................................................................
        .asg    0x0006,ST0_old_        ; Status Register 0 Protected            ; [15:00]
        .asg    0x0007,ST1_old_        ; Status Register 1 Protected            ; [15:00]
        .asg    0x0008,AC0_L           ; Accumulator 0 Low bits         ; [15:00]
        .asg    0x0009,AC0_H           ; Accumulator 0 High bits        ; [31:16]
        .asg    0x000a,AC0_G           ; Accumulator 0 Guard bits       ; [39:32]
        .asg    0x000b,AC1_L           ; Accumulator 1 Low bits         ; [15:00]
        .asg    0x000c,AC1_H           ; Accumulator 1 High bits        ; [31:16]
        .asg    0x000d,AC1_G           ; Accumulator 1 Guard bits       ; [39:32]
        .asg    0x000e,T3_             ; Temporary Register 3                           ; [15:00]
        .asg    0x000f,TRN0_           ; Transition Register 0          ; [15:00]
        .asg    0x0010,AR0_            ; Auxiliary Register 0           ; [15:00]
        .asg    0x0011,AR1_            ; Auxiliary Register 1           ; [15:00]
        .asg    0x0012,AR2_            ; Auxiliary Register 2           ; [15:00]
        .asg    0x0013,AR3_            ; Auxiliary Register 3           ; [15:00]
        .asg    0x0014,AR4_            ; Auxiliary Register 4           ; [15:00]
        .asg    0x0015,AR5_            ; Auxiliary Register 5           ; [15:00]
        .asg    0x0016,AR6_            ; Auxiliary Register 6           ; [15:00]
        .asg    0x0017,AR7_            ; Auxiliary Register 7           ; [15:00]
        .asg    0x0018,SP_             ; Stack Pointer                  ; [15:00]
        .asg    0x0019,BK03_           ; circular Buffer size register for AR[0-3]  ; [15:00]
        .asg    0x001a,BRC0_           ; Block Repeat Counter 0         ; [15:00]
        .asg    0x001b,RSA0_L          ; Repeat Start Adress Low bits 0           ; [15:00]
        .asg    0x001c,REA0_L          ; Repeat End Adress Low bits 0     ; [15:00]
        .asg    0x001d,ST3_PMST_       ; Processor Mode STatus register           ; [15:00]
        .asg    0x001e,XPC_            ; eXtension Program Counter register   ; [07:00]

*.......RESERVED.................................................................................

        .asg    0x0020,DR0_            ; Data Register 0        ; [15:00]
        .asg    0x0021,DR1_            ; Data Register 1                          ; [15:00]
        .asg    0x0022,DR2_            ; Data Register 2                          ; [15:00]
        .asg    0x0023,DR3_            ; Data Register 3                          ; [15:00]
        .asg    0x0024,AC2_L           ; Accumulator 2 Low bits                   ; [15:00]
        .asg    0x0025,AC2_H           ; Accumulator 2 High bits                ; [31:16]
        .asg    0x0026,AC2_G           ; Accumulator 2 Guard bits                 ; [39:32]
        .asg    0x0027,CDP_            ; Coefficient Data Pointer                       ; [15:00]
        .asg    0x0028,AC3_L           ; Accumulator 3 Low bits                         ; [15:00]
        .asg    0x0029,AC3_H           ; Accumulator 3 High bits      ; [31:16]
        .asg    0x002a,AC3_G           ; Accumulator 3 Guard bits                       ; [39:32]
        .asg    0x002b,DPH_            ; Main Data Page (dma & indirect from CDP)	; [06:00]
        .asg    0x002b,MDP_            ; Main Data Page (dma & indirect from CDP) ; [06:00]
*.......RESERVED.................................................................................
*.......RESERVED.................................................................................
*........asg    0x002c,MDP05_          ; Main Data Page (indirect from AR[0-5])   ; [06:00]
*........asg    0x002d,MDP67_          ; Main Data Page (indirect from AR[6-7])         ; [06:00]
        .asg    0x002e,DP_             ; memory Data Page start address           ; [15:00]
        .asg    0x002f,PDP_            ; Peripheral Data Page start address   ; [15:00]
        .asg    0x0030,BK47_           ; circular Buffer size register for AR[4-7]      ; [15:00]
        .asg    0x0031,BKC_            ; circular Buffer size register for CDP          ; [15:00]
        .asg    0x0032,BOF01_          ; circular Buffer Offset register for AR[0-1]  ; [15:00]
        .asg    0x0032,BSA01_          ; Circular buffer start address register for AR[0-1]	; [15:00]
        .asg    0x0033,BOF23_          ; circular Buffer Offset register for AR[2-3]    ; [15:00]
        .asg    0x0033,BSA23_          ; Circular buffer start address register for AR[2-3]	; [15:00]
        .asg    0x0034,BOF45_          ; circular Buffer Offset register for AR[4-5]    ; [15:00]
        .asg    0x0034,BSA45_          ; Circular buffer start address register for AR[4-5]   ; [15:00]
        .asg    0x0035,BOF67_          ; circular Buffer Offset register for AR[6-7]    ; [15:00]
        .asg    0x0035,BSA67_          ; Circular buffer start address register for AR[6-7]   ; [15:00]
        .asg    0x0036,BOFC_           ; circular Buffer Offset register for CDP        ; [15:00]
        .asg    0x0036,BSAC_           ; Circular buffer start address register for CDP       ; [15:00]

*.......RESERVED.................................................................................

        .asg    0x0038,TRN1_           ; Transition Register 1                          ; [15:00]
        .asg    0x0039,BRC1_           ; Block Repeat Counter 1                         ; [15:00]
        .asg    0x003a,BRS1_           ; Block Repeat Save 1                            ; [15:00]
        .asg    0x003b,CSR_            ; Computed Single Repeat register          ; [15:00]
        .asg    0x003c,RSA0_H          ; Repeat Start Adress High bits 0                ; [23:16]
        .asg    0x003d,RSA0_L          ; Repeat Start Adress Low bits 0                 ; [15:00]
        .asg    0x003e,REA0_H          ; Repeat End Adress High bits 0                  ; [23:16]
        .asg    0x003f,REA0_L          ; Repeat End Adress Low bits 0                   ; [15:00]
        .asg    0x0040,RSA1_H          ; Repeat Start Adress High bits 1                ; [23:16]
        .asg    0x0041,RSA1_L          ; Repeat Start Adress Low bits 1                 ; [15:00]
        .asg    0x0042,REA1_H          ; Repeat End Adress High bits 1                  ; [23:16]
        .asg    0x0043,REA1_L          ; Repeat End Adress Low bits 1                   ; [15:00]
        .asg    0x0044,RPTC_           ; single RePeaT Counter        ; [15:00]
        .asg    0x0045,IMR1_           ; Interrupt Mask Register 1                      ; [07:00]
        .asg    0x0046,IFR1_           ; Interrupt Flag Register 1                      ; [07:00]
        .asg    0x0047,DBIER0_         ; DeBug IntErrupt Register 0           ; [15:00]
        .asg    0x0048,DBIER1_         ; DeBug IntErrupt Register 1                     ; [07:00]
        .asg    0x0049,IVPD_           ; Interrupt Vector Pointer DSP     ; [15:00]
        .asg    0x004a,IVPH_           ; Interrupt Vector Pointer HOST                  ; [15:00]
        .asg    0x004b,ST2_            ; Status Register 2                              ; [08:00]
        .asg    0x004c,SSP_            ; System Stack Pointer       ; [15:00]
        .asg    0x004d,SP_new_         ; System Stack Pointer       ; [15:00]
        .asg    0x004e,SPH_            ; System Stack Pointer High      ; [06:00]
        .asg    0x004f,CDPH_           ; Coeffiecient Data Pointer High     ; [06:00]

*.......RESERVED.(from 0x004d to 0x005f).........................................................

;;;;;;;;(from 0x0050 to 0x005f  Emulation registers with LAIJIN2.2 version);;;;;;;;;;;;;;;;;;;;;;
        .asg    0x0050,HWAD_L           ; HWA data Register Low / Reserved in UMA1.0    ; [15:00]
        .asg    0x0051,HWAD_H           ; HWA data Register High/ Reserved in UMA1.0    ; [31:16]
        .asg    0x0052,DPC_L            ; Program counter Low                           ; [15:00]
        .asg    0x0053,DPC_H            ; Program counter High                          ; [31:16]
        .asg    0x0054,CFCT_LCRPC_L     ; Control Flow Context/Fast return registers L  ; [15:00]
        .asg    0x0055,CFCT_LCRPC_H     ; Control Flow Context/Fast return registers H  ; [31:16]
        ;       0x0056                  ; Reserved
        ;       0x0057                  ; Reserved
        .asg    0x0058,AC1_L2           ; Accumulator 1 Low bits  (For emu allow long)  ; [15:00]
        .asg    0x0059,AC1_H2           ; Accumulator 1 High bits (For emu allow long)  ; [31:16]
        .asg    0x005a,ACS_G            ; HWA address reg /ACC shadow guard reg         ; [15:00]
        .asg    0x005b,XARS_            ; XAR Shadow register                           ; [15:00]
        ;       0x005c                  ; Reserved
        ;       0x005d                  ; Reserved
        ;       0x005e                  ; Reserved
        .asg    0x005f,CPU_REV          ; Cpu and instruction set revision              ; [15:00]

        .asg    0x006e,SIM_INT          ; SIM Interrupts Register                        ; [16:00]
        .asg    0x006f,SIM_SIG          ; SIM Signature Register                         ; [16:00]

*
****************************************************************************************************

****************************************************************************************************
* DFT reserved
****************************************************************************************************

        .asg    0x7ff300,A_LDBYPASS       ; reserved LDBYPASS flag
        .asg    0x7ff301,A_PSACMP         ; reserved PSACMP flag
        .asg    0x7ff302,A_PASSFAIL_MMAP2 ; reserved PASSFAIL flag for MMAP2
        .asg    0x001302,A_PASSFAIL       ; reserved PASSFAIL flag
        .asg    0x7d6c00,A_TPORT_DUMP     ; reserved, address of tport dump register (byte addr 0xfad800)

****************************************************************************************************
*       SECTION 3 : General purpose...
****************************************************************************************************
***dde added
        .asg    0x037FFD,ERR_MON_ADDR ; WORD ADDRESS
****

*C vs. VHDL requirements !!! Save in DARAM2

        .asg    0x4000,A_ERR           ; "ERROR" counter write address for C vs. VHDL
        .asg    0x5ffc,A_SIG           ; "SIGNATURES" counter write address for C vs. VHDL

*Signature context save adddress

        .asg    0x4800,A_SIGSAVE       ; "SIGNATURES" counter save address

*Signature external adddress

        .asg    0xFFEE,FAIL_MON_ADDR   ; "SIGNATURES" counter value address (MDP = S0_P06)
**      .asg    0xFFFE,ERR_MON_ADDR    ; "SIGNATURES" result address (MDP = S0_P06)

*A_CDP is mapped in DARAM0

        .asg    0x0668,A_CDP           ; Coefficient address (pointed by CDP register)

*Symbols used in Wrappers test cases

        .asg    0x0002,LOFFSET         ; Long offset for indirect addressing with AR(n)
        .asg    0x0001,WOFFSET         ; Word offset for indirect addressing with AR(n)

*CACHE re-fill policy symbols

        .asg    0x0000,FILL_L          ; Line by line (32 bit line by 32 bit line)
        .asg    0x0002,FILL_H          ; Half block (2 x 32 bit lines)
        .asg    0x0003,FILL_B          ; Block (4 x 32 bit lines)

*FDBK symbols

;       .asg    0x7D7D00,FDBK_APIREG   ; Address of FDBK_APIREG for debug only
;       .asg    0x7D7D01,FDBK_APIREG1  ; Address of FDBK_APIREG1 for debug only
;       .asg    0x7D4900,FDBK_APIREG2  ; Address of FDBK_APIREG2 for debug only
;       .asg    0x003D00,FDBK_APITEST1 ; Address of first test signature
;       .asg    0x2B4900,FDBK_APITEST2 ; Address of first test signature
;       .asg    0x028012,FDBK_HDBUSO   ; Address of api_hdbuso_tr for FDBK testcases
;       .asg    0x00DEAD,FDBK_DEAD     ; Data Address bus value for FDBK testcases
;       .asg    0x2A0B00,FDBK_RET      ; Address used for the generation of RET instruction

        .asg    0x07FF,   TMCR_MM
        .asg    0x07FE,   TMCR_MGS3
        .asg    0x008000, FDBK_APIREG1 ; Address of 1st api feedback registers
        .asg    0x008001, FDBK_APIREG2 ; Address of 2nd api feedback registers
        .asg    0x008002, FDBK_APIREG3 ; Address of 3rd api feedback registers
        .asg    0x008003, FDBK_EMIFCNTL  ; Address of emif feedback control registers
        .asg    0x008004, FDBK_EMIFICACHE1;  Address of emif icache1 feedback registers
        .asg    0x008005, FDBK_EMIFICACHE2;  Address of emif icache2 feedback registers
        .asg    0x008006, FDBK_INTR1   ; Address of 1st interrupt feedback registers
        .asg    0x008007, FDBK_INTR2   ; Address of 2nd interrupt feedback registers
        .asg    0x008008, FDBK_INTR3   ; Address of 3rd interrupt feedback registers
        .asg    0x008009, FDBK_INTR4   ; Address of 4th interrupt feedback registers
        .asg    0x00800A, FDBK_MISC    ; Address of feedback miscellaneous registers
        .asg    0x00800B, FDBK_BOOTREG ; Address of feedback boot pointer registers
        .asg    0x00800C, FDBK_DMAREQ1 ; Address of 1st dma request feedback registers
        .asg    0x00800D, FDBK_DMAREQ2 ; Address of 2nd dma request feedback registers
        .asg    0x00800E, FDBK_PEDBUSI ; Address of rhea peripheral rhea data input bus fdbk registers
        .asg    0x00800F, FDBK_RHEASEL1 ; Address of 1st rheasel feedback registers
        .asg    0x008010, FDBK_RHEASEL2 ; Address of 2nd rheasel feedback registers
        .asg    0x008011, XPSA_APICNTL  ; Address of api xpsa control registers
        .asg    0x008012, XPSA_APIDBUS  ; Address of api xpsa dbus registers
        .asg    0x008013, XPSA_CPUCNTL  ; Address of cpu xpsa control registers
        .asg    0x008014, XPSA_CPUDBUS  ; Address of cpu xpsa dbus registers
        .asg    0x008015, XPSA_EMIFCNTL1 ; Address of emif xpsa1 control registers
        .asg    0x008016, XPSA_EMIFDBUS1 ; Address of emif xpsa1 dbus registers
        .asg    0x008017, XPSA_EMIFCNTL2 ; Address of emif xpsa2 control registers
        .asg    0x008018, XPSA_EMIFDBUS2 ; Address of emif xpsa2 dbus registers
        .asg    0x008019, XPSA_EMIFCNTL3 ; Address of emif xpsa3 control registers
        .asg    0x00801A, XPSA_EMIFDBUS3 ; Address of emif xpsa3 dbus registers
        .asg    0x00801B, XPSA_RHEACNTL1 ; Address of rhea xpsa1 control registers
        .asg    0x00801C, XPSA_RHEADBUS1 ; Address of rhea xpsa1 dbus registers
        .asg    0x00801D, XPSA_RHEACNTL2 ; Address of rhea xpsa2 control registers
        .asg    0x00801E, XPSA_RHEADBUS2 ; Address of rhea xpsa2 dbus registers
        .asg    0x00801F, XPSA_RHEACNTL3 ; Address of rhea xpsa3 control registers
        .asg    0x008020, XPSA_RHEADBUS3 ; Address of rhea xpsa3 dbus registers
        .asg    0x008021, XPSA_RHEADBUS1P ; Address of rhea xpsa1P dbus registers
        .asg    0x008022, XPSA_RHEADBUS2P ; Address of rhea xpsa2P dbus registers
        .asg    0x008023, XPSA_RHEADBUS3P ; Address of rhea xpsa3P dbus registers
* Icache Registers
        .asg    0x001400, ICACHE_GCR  ; Address of icache Global Control Register
        .asg    0x001401, ICACHE_FLAR0  ; Address of icache Flush Line Address Register 0
        .asg    0x001402, ICACHE_FLAR1  ; Address of icache Flush Line Address Register 1
        .asg    0x001403, ICACHE_NWCR   ; Address of icache N-Way Control Register
        .asg    0x001404, ICACHE_SR   ; Address of icache Status Register
        .asg    0x001405, ICACHE_RCR1 ; Address of icache Ramset Control Register 1
        .asg    0x001406, ICACHE_RTR1   ; Address of icache Ramset Tag Register 1
        .asg    0x001407, ICACHE_RCR2   ; Address of icache Ramset Control Register 2
        .asg    0x001408, ICACHE_RTR2   ; Address of icache Ramset Tag Register 2
* Icache LVB & LRU
        .asg    0x001600, ICACHE_LVB0   ; Start Address of icache LVBs for Bank0
        .asg    0x001620, ICACHE_LVB1   ; Start Address of icache LVBs for Bank1
        .asg    0x001640, ICACHE_LVB2   ; Start Address of icache LVBs for Bank2
        .asg    0x001660, ICACHE_LRU  ; Start Address of icache LRU bits
* Icache Data & Tag Banks
        .asg    0x008000, ICACHE_DATA0  ; Start Address of icache Data Bank0
        .asg    0x009000, ICACHE_DATA1  ; Start Address of icache Data Bank1
        .asg    0x00A000, ICACHE_DATA2  ; Start Address of icache Data Bank2
        .asg    0x00B000, ICACHE_TAG1   ; Start Address of icache Tag Bank1
        .asg    0x00B200, ICACHE_TAG2   ; Start Address of icache Tag Bank2

* Icemaker (internal) PSAs:
        .asg    0x0420,APSA_H ; APSA[40 : 32]
        .asg    0x0423,APSA_M ; APSA[31 : 16]
        .asg    0x0422,APSA_L   ; APSA[15 :  0]
        .asg    0x0426,APSA_CTL ; APSA CNTL [4 : 0]
        .asg    0x0427,APSA_ID

        .asg    0x0440,DPSA_H ; DPSA[40 : 32]
        .asg    0x0443,DPSA_M ; DPSA[31 : 16]
        .asg    0x0442,DPSA_L   ; DPSA[15 :  0]
        .asg    0x0446,DPSA_CTL ; DPSA CNTL [4 : 0]
        .asg    0x0447,DPSA_ID

        .asg    0x05fe,TPSA_H ; TPSA[40 : 32]
        .asg    0x05fd,TPSA_M ; TPSA[31 : 16]
        .asg    0x05fc,TPSA_L ; TPSA[15 :  0]
        .asg    0x05ff,TPSA_CTL ; APSA CNTL [2 : 0]
*Xmem configuration symbols


*Xmem configuration symbols

        .asg    0,XMEM_MODE_TIMED             ; Xmem in Timed Mode
        .asg    1,XMEM_MODE_HANDSHAKED        ; Xmem in Handshaked Mode
        .asg    1,XMEM_MODE_RESET             ; Xmem Mode at Reset
        .asg    0,XMEM_BURST_DIS              ; Xmem Burst disabled
        .asg    1,XMEM_BURST_ENA              ; Xmem Burst enabled
        .asg    1,XMEM_BURST_RESET            ; Xmem Burst at Reset
        .asg    00000000,XMEM_LATENCY_MIN     ; Xmem Latency Minimum value
        .asg    01001011,XMEM_LATENCY_MAX     ; Xmem Latency Maximum value
        .asg    00000100,XMEM_LATENCY_W_ST    ; Xmem Latency for Wait State insertion
        .asg    11111111,XMEM_LATENCY_DIS     ; Xmem Latency Maximum value (no ready back)
        .asg    00010000,XMEM_LATENCY_BERR    ; Xmem Latency for Bus Error generation
        .asg    00000010,XMEM_LATENCY_RESET   ; Xmem Latency at Reset
        .asg    00000010,XMEM_LATENCY_SBSRAM  ; Xmem Latency for SBSRAM memory

****************************************************************************************************
* ICR register values
****************************************************************************************************

; Values of Bit patterns to load in ICR to idle a domain
        .asg    0x0001, DSP_DOMAIN_IDLE
        .asg    0x0002, DMA_DOMAIN_IDLE
        .asg    0x0004, ICACHE_DOMAIN_IDLE
        .asg    0x0008, PERIPH_DOMAIN_IDLE
        .asg    0x03FF, DPLL_DOMAIN_IDLE
        .asg    0x0023, DPORT_DOMAIN_IDLE
        .asg    0x0043, XPORT_DOMAIN_IDLE
        .asg    0x0082, MPORT_DOMAIN_IDLE
        .asg    0x0105, IPORT_DOMAIN_IDLE
        .asg    0x0200, HWA_DOMAIN_IDLE


*
****************************************************************************************************
