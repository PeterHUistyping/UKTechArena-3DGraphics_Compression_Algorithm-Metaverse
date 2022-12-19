# LZSS Understanding and Improving
Contest Website: https://huawei-techarena-uk.bemyapp.com/2022

Original Rules: https://github.com/UKTechArena

Original Team Project: https://github.com/UKTechArena/crazy-thursday
## Score
```
Average compression ratio: 0.83859 percent
Average decompression time: 24.6667 milliseconds
Average image quality metric 100 decibels

Weighted compression score (35%): 29.3506
Weighted time score (25%): 24.3833
Weighted quality score (20%): 20

Total score (80%): 73.7339
```
## LZSS Compression Algorithm (Explained by a Demo)
```
First byte of the Literal run: flag L4-L0, which flag indicates which level is used now.
    000                           // level 1
    *(ubyte*)OUTPUT_ |= (1 << 7); // level 2
	*(ubyte*)OUTPUT_ |= (1 << 6); // level 3
	*(ubyte*)OUTPUT_ |= (1 << 5); // level 4

	*(ubyte*)OUTPUT_ |= (1 << 5); // level 5
    *(ubyte*)OUTPUT_ |= (1 << 7); 
Reuse:  
```
```
***Level 1: Supported Windows Size 8192 2^13 (0-8191)
	Windows Size 0-8191
Original Match-> Encoded Len
  1-2 Bytes   -> 1 Bytes (Literal Run)
  3-8 Bytes   -> 2 Bytes (Short Match)
  9-264 Bytes -> 3 Bytes (Long Match)
| Instruction type |  Decode[0]  |  Decode[1] |   Decode[2]  |   Decode[3]  |
|------------------|-------------|------------|--------------|--------------|
| Literal run      | 000 L4-L0   |            |              |              |
| Short match 	   | M2-M0 W12-W8|    W7-W0   |              |              |
| Long match       | 111   W12-W8|    M7-M0   |     W7-W0    |              |

	Demo 
Original:
		------------
		I am Sam \n
		Sam I am \n
		That
		------------
Encoded:
--------------------------------------------------------------------------
		28 I am Sam \n 
   		20 03                00 20         	40 0C                  04 \nThat
--------------------------------------------------------------------------
	     |short match|        |literal run|         |short match|         |literal run|
High 3bits  Match length 001+2           1            Match length 010+2=4          5
                Offset 3           Copy space(20)         Offset 12             Copy \nThat
		  Sam                 [space]             I am                    \nThat


That Sam-I-am!
That Sam-I-am!
I do not like
that Sam-I-am!
Do you like green eggs and ham?
I do not like them, Sam-I-am.
I do not like green eggs and ham.
```
```
***** Level 2: Extended Windows & Infinite Match Length
	Supported Windows Size 65535 + 8191  (0-65535 + 8191 )
	Windows Size 0-8190 (8191 as flag)
Original Data-> Encoded Len
  1-2 Bytes   -> 1 Bytes (Literal Run)
  3-8 Bytes   -> 2 Bytes (Short Match)
  9+ Bytes -> 3 Bytes (Long Match)

* distance >= MAX_L2_DISTANCE 8191 ( 13 bits of 1 for the high )
  5-8 Bytes   -> 4 Bytes (Extended Windows Size: Short Match)
	Windows Size 8191 - (65535 + 8191 - 1)    (shouldn' be all 1, to avoid unable to detect end!)
  9-264 Bytes -> 4+ Bytes (Extended Windows Size: Long Match)
			Depends on Match length

| Instruction type |  Decode[0]  |  Decode[1] |   Decode[2]  |   Decode[3]  |   Decode[4]  |
|------------------|-------------|------------|--------------|--------------|--------------|
| Literal run      | 000 L4-L0   |            |              |              |
| Short match 	   | M2-M0 W12-W8|   W7-W0    |              |              |
| Long match       | 111   W12-W8|  M7-M0...  |     W7-W0    |              |
| Extended Windows*| ----------------------------------------------------------------------|
| Short match      | M2-M0 11111 |  1111 1111 |    W15-W8    |     W7-W0    |
| Long match       | 111   11111 |  M7-M0...  |   1111 1111  |    W15-W8    |     W7-W0    |
 ...Depends on Match length 
 
```
```
******* Level 3: Extra Windows Size
	Supported Windows Size 65535 + 8191 - 1 (0-65535 + 8191 - 1)
Windows Size 0-8190
	Original Data-> Encoded Len
  1-2 Bytes   -> 1 Bytes (Literal Run)
  3-8 Bytes   -> 2 Bytes (Short Match)
  9+ Bytes -> 3 Bytes (Long Match)

* distance >= MAX_L2_DISTANCE 8191 ( 13 bits of 1 for the high )
  5-8 Bytes   -> 4 Bytes (Extended Windows Size: Short Match)
	Windows Size 8191 - (65535 + 8191 - 2)    (shouldn' be all 1, to avoid unable to detect end!)
  9-264 Bytes -> 4+ Bytes (Extended Windows Size: Long Match)
			Depends on Match length

* distance >= MAX_L3_DISTANCE 8191 ( 13 bits of 1 for the high )
  5-8 Bytes   -> 4 Bytes (Extended Windows Size: Short Match)
	Windows Size (65535 + 8191 - 1) - (65535 + 65535 + 8191 - 2)    (shouldn' be all 1, to avoid unable to detect end!)
  9-264 Bytes -> 4+ Bytes (Extended Windows Size: Long Match)
			Depends on Match length

| Instruction type |  Decode[0]  |  Decode[1] |   Decode[2]  | Decode[3] | Decode[4] | Decode[5] | Decode[6] |
|------------------|-------------|------------|--------------|-----------|-----------|-----------|-----------|
| Literal run      | 000   L4-L0 |            |              |           |           |           |           |
| Short match 	   | M2-M0 W12-W8|   W7-W0    |              |           |           |           |           |
| Long match       | 111   W12-W8|  M7-M0...  |     W7-W0    |           |           |           |           |
| Extended Windows*| ----------------------------------------------------------------------------------------|
| Short match      | M2-M0 11111 | 1111 1111  |    W15-W8    |   W7-W0   |           |           |           |
| Long match       | 111   11111 |  M7-M0...  |  1111 1111   |  W15-W8   |   W7-W0   |     		 |           |
| Extended Windows2| ----------------------------------------------------------------------------------------|
| Extra Short match| M2-M0 11111 | 1111 1111  |  1111 1111   | 1111 1111 |  W15-W8   |   W7-W0   |           | 
| Extra Long match | 111   11111 |  M7-M0...  |  1111 1111   | 1111 1111 | 1111 1111 |   W15-W8	 |   W7-W0   |
 ...Depends on Match length 
```
```
** level4: Ultra Windows Size
	Larger Windows Size
```
```
** level5: Direct Match
| Instruction type |  Decode[0]  |  Decode[1] |   Decode[2]  | Decode[3] | Decode[4] | Decode[5] | Decode[6] |
|------------------|-------------|------------|--------------|-----------|-----------|-----------|-----------|
| Literal run      | 000   L4-L0 |            |              |           |           |           |           |
| Short match 	   | M2-M0 W12-W8|   W7-W0    |              |           |           |           |           |

| Long match       | 111   W12-W8|  M7-M0...  |     W7-W0    |           |           |           |           |
| Direct match     | 110   W12-W8|  D15-D8    |     D7-D0    |  W7-W0    |           |           |            
| Long match       | 111   W12-W8|  1111 1111 |     W7-W0    |           |           |           |           |
| Extended Windows*| ----------------------------------------------------------------------------------------|
| Short match      | M2-M0 11111 | 1111 1111  |    W15-W8    |   W7-W0   |           |           |           |
| Long match       | 111   11111 |  M7-M0...  |  1111 1111   |  W15-W8   |   W7-W0   |     		 |           |
| Extended Windows2| ----------------------------------------------------------------------------------------|
| Extra Short match| M2-M0 11111 | 1111 1111  |  1111 1111   | 1111 1111 |  W15-W8   |   W7-W0   |           | 
| Extra Long match | 111   11111 |  M7-M0...  |  1111 1111   | 1111 1111 | 1111 1111 |   W15-W8	 |   W7-W0   |

 ...Depends on Match length 

				Original Data	-> 		Encoded Len
| Literal run | 1-2 Bytes      -> 		1 Bytes (Literal Run)
| 000   L4-L0 |            |              |           |           |           |           |

 
| Short match |	3-7 Bytes  ->	2 Bytes (Short Match)  [001-101 +6]
| M2-M0 W12-W8|   W7-W0    |              |           |           |           |           |
------------------------------------------------- -1*125977 (110) -------------------------------------------------
| Long match 1| 8-262 Bytes 	->  3 Bytes / 1 Bytes for Match_Len    [00-FE +255]
| 111   W12-W8|  M7-M0...  |     W7-W0    |           |           |           |           |
match_len>260 30 0000
match_len>260+256. 146146
match_len>260+256+256 8181
	Max 61042 (238)
match_len>65797 0
| Direct match| 263-65797 Bytes -> 4 Bytes  / 2 Bytes for Match_Len    [262+2^16-1]    +2^16-1
| 110   W12-W8|  D15-D8    |     D7-D0    |  W7-W0    |           |           |            
-----------------------------------len>260+256---- +i*146146 -[i:1-238]---------------------------------------------
| Long match 2| 65798+ Bytes 	-> 	4+ Bytes  / 2+ Bytes for Match_Len      
| 111   W12-W8|  1111 1111 |   M15-M8...   |  M7-M0... ｜   W7-W0  |           |            |              |
```
```
*** Level6: Direct Long Match   For 34MB   
-----------------------------------len>260+256---- +i*146146 -[i:1-238]---------------------------------------------
| Long match 2| 65798+ Bytes 	-> 	4+ Bytes  / 2-5 Bytes for Match_Len
Explain in detail: First three rows: 256*3-1      
					 Stop when not 1111 1111			
| 111   W12-W8|  1111 1111 |   M15-M8...   |  M7-M0... 	｜ W7-W0   |            |           |               |
| 111   W12-W8|  1111 1111 |   M23-M16..   |  M15-M8...	| M7-M0... ｜  W7-W0    |           |               | 
| 111   W12-W8|  1111 1111 |   M31-M24..   |  M23-M16.. | M15-M8...|  M7-M0... ｜  W7-W0    |               |   
| 111   W12-W8|  1111 1111 |   0000 0000   |  D31-D24   |  D23-D16 |  D15-D8    |  D7-D0    |     W7-W0    |
```
```
*** Level6: Direct Long Match   For 34MB   
-----------------------------------len>260+256---- +i*146146 -[i:1-238]---------------------------------------------
| Long match 2| 65798+ Bytes 	-> 	4+ Bytes  / 2-5 Bytes for Match_Len
Explain in detail: First three rows: 256*3-1      
					 Stop when not 1111 1111			
| 111   W12-W8|  1111 1111 |   M15-M8...   |  M7-M0... 	｜ W7-W0   |            |           |               |
| 111   W12-W8|  1111 1111 |   M23-M16..   |  M15-M8...	| M7-M0... ｜  W7-W0    |           |               | 
| 111   W12-W8|  1111 1111 |   0000 0000   |  D23-D16 |  D15-D8    |  D7-D0    |     W7-W0    |
```

## Update
Compress / Decompress\
Using namespace std;
Record time spent:
```
chrono::time_point<std::chrono::system_clock> begin_time=     
                        std::chrono::system_clock::now();
    //sleep(10);
    auto end_time = std::chrono::system_clock::now();
    chrono::duration<double, std::milli> duration_mili = end_time - begin_time;
    
    printf("PrintDuration : duration_mili duration = %ld ms", (long)duration_mili.count());
```
Updated Concurrent implementation
	Unit Reading of I/O
	Multinputle Threads of Decompression
	

### Optimization
Ongoing TESTING here
### WHAT Have Been Done
Implemented from LZSS, using Object Oriented Programming.

Branch prediction [[likely]]

#define LEVEL1_MAX 65536 
    IN
    int Compress_LZ(const void* INPUT, int length, void* OUTPUT) {
    if (length < LEVEL1_MAX) return Compress_level1(INPUT, length, OUTPUT);
    return Compress_level2(INPUT, length, OUTPUT);
    }

Two Interfaces:
    LZSS_Comp_once();
    LZSS_Compression(Compress_Twice); 
        Defined in  LZSS_helper.h: bool Compress_Twice;

Changed the I/O (tested) and removed anything unrelated to the algorithm

Deleted duplicate v, vn, vt

Treating every input in compression as ubyte (uint8_t)
## How to build and test
### Creating Your Repository
Please look at the original document. 

Run the `scripts/build_non_ai.sh` script to build the executables.

```
bash scripts/build_non_ai.sh
```

### Testing the Executables

Use `scripts/test_obj.sh` instead of `scripts/test_non_ai.sh` script runs all the sample models through the encoder and decoder.

```
bash scripts/test_non_ai.sh
```
