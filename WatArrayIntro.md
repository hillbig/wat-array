# Introduction #

wat-array is a C++ library for myriad array operations using wavelet trees.

wat-array supports fundamental operations in arrays such as  `count` /   `mode` /   `max` /  `min` /  `list` with a position range and an alphabet range.
For example, you can find the k-th largest value in the array `A[sp...ep]` for any `sp`, `ep`, and `k`.
All these operations are supported in `O(log s)` time and constant for `n` where n is the length of array, and `s` is the number of alphabet (except for `ListModeRange`).

Another virtue of wat-array is its small working space. The working space of wat-array is equal to that of the original input array, and wat-array does not require the original array information (such data structure or index is called self-index). Therefore, the user can replace the original array with wat-array. Formally, let `A[0...n)` be the input array where `0<=A[i]<s`. Then, wat-array requires `n log s` bits of space (the base of log is 2) plus small overhead.

For example, `n=10^8` and `k=10^3`, the working space of wat-array is `10^8 * log(10^3) = 1Gbit = 125MB`, and wat-array can find the k-th largest value in `A[sp, ep]` for any `k, sp, ep` in less than 1 micro second (i.e. >1000000 qps).

# QuickStart #

Currently, wat-array is supported in Ubuntu Linux

Download the latest tarball of wat-array from [Downloads](http://code.google.com/p/wat-array/downloads/list).

Finally type the following commands.
```
> tar xvjf wat-array-x.x.x.tar.bz2
> cd wat-array-x.x.x
> ./waf configure
> ./waf
sudo ./waf install
```

The library wat-array will be installed.

In your program, you can use a wat-array as follows.
```
#include <wat_array/wat_array.hpp>

....
vector<uint64_t> array;
... // set your array;
wat_array::WatArray wa;
wa.Init(array);

```

You can find sample programs in wat-array/tool/

# API #
Here we list the supported operations in wat-array.
You can find the formal definition in [wat\_array.hpp](http://code.google.com/p/wat-array/source/browse/trunk/src/wat_array.hpp)

| **Method** |  |
|:-----------|:-|
| `Init(A)`  | Create wat-array from the array `A` |
| `length`   | Return the length of `A` |
| `alphabet_num` | Return the number of alphabet (max value in A+1) of `A` |
| `Freq(c)`  | Return the number of `c` in `A`|
| `Lookup(pos) `|  Return `A[pos]` |
| `Rank(c, pos)` |  Return the number of `c` in `A[0...pos)` |
| `Select(c, rank)` | Return the position of the (rank)-th occurence of `c` in `A` |
| `RankLessThan(c, pos)` | Return the number of `c'<c` in `A[0...pos)` |
| `RankMoreThan(c, pos)` | Return the number of `c'>c` in `A[0...pos)` |
| `FreqRange(min_c, max_c, beg_pos, end_pos)` | Return the number of `min_c<=c'<max_c` in `A[beg_pos...end_pos)` |
| `MaxRange(beg_pos, end_pos)` | Return the maximum value and its position in `A[beg_pos...end_pos)` |
| `MinRange(beg_pos, end_pos)` |  Return the minimum value and its position in `A[beg_pos...end_pos)`. |
| `QuantileRange(beg_pos, end_pos, k)` | Return the (k+1)-th smallest value and its position in `A[beg_pos...end_pos]`. |
| `ListModeRange(min_c, max_c, beg_pos, end_pos, num)` | Return the most frequent characters and its frequencies in `A[beg_pos...end_pos)` and `min_c<=c<max_c`|
| `ListMinRange(min_c, max_c, beg_pos, end_pos, num)` | Return the characters in ascending order and its frequencies in `A[beg_pos...end_pos)` and `min_c<=c<max_c` |
| `ListMaxRange(min_c, max_c, beg_pos, end_pos, num)` | Return the characters in descending order and its frequencies in `A[beg_pos...end_pos)` and `min_c<=c<max_c` |
| `Save(os)` | Save the current status to os |
| `Load(is)` | Load the status from is |

For MaxRange, MinRange, and QuantileRange, if there are many such positions, return the left most position.
If an argument is invalid, these functions return wat\_array::NOTFOUND.

You can find sample programs at
performance\_test/performance\_test.cpp
test/wat\_array\_test.cpp

# Example #

```
#include <wat_array/wat_array.hpp>
...
vector<uint64_t> A;
A.push_back(5);
A.push_back(1);
A.push_back(0);
A.push_back(4);
A.push_back(2);
A.push_back(2);
A.push_back(0);
A.push_back(3);

// index    01234567 
// A       = 51042203

wat_array::WatArray wa;
wa.Init(A);

wa.Lookup(3); // =4. Return A[3]

wa.Rank(2, 6); // =2. The num. of 2 in A[0...6)

wa.Select(2, 2); // =5. The second 2 appeared in A[5]

wa.RankLessThan(4, 5); // =3. {1,0,2} appear in A[0...5)

wa.RankMoreThan(4, 5); // =4. {5, 4} appear in A[0...5)

wa.FreqRange(2, 5, 2, 6); // =3. {4, 2, 2} appear in A[2...5)

wa.MaxRange(1, 6); // =(pos=3, val=4). A[3]=4 is the maximum in A[1...6)

wa.QuantileRange(1, 6, 3); // = (pos=4, val=2). Sort A[1...6) = 001224, and take the (3+1)-th value

wa.ListModeRange(1, 5, 1, 6, 3); // = (val=0, freq=2), (val=2, freq=2), (val=1, freq=1)

wa.ListMaxRange(1, 5, 1, 6, 3); // = (val=4, freq=1), (val=2, freq=2), (val=1, freq=1)

```

# Experimental Result #

We compared wat-array with the baseline, naive investigation. For example, in the baseline, `Rank(c, i)` is examined by checking `A[0...i)` explicitly.
The test program is found at [wat-array/test/performance\_test.cpp](http://code.google.com/p/wat-array/source/browse/trunk/test/performance_test.cpp)

The table shows the average time per one query in micro sec (`10^{-6}` sec.). We performed 100 random query.

| **method** | **length** | **alphabet\_num** | **lookup** | | **rank** | **select** | **max\_range** | **quantile\_range** | **freq\_range** | **list\_max\_one** | **list\_max\_ten** | **list\_mode\_one** | **list\_mode\_ten** |
|:-----------|:-----------|:------------------|:-----------|:|:---------|:-----------|:---------------|:--------------------|:----------------|:-------------------|:-------------------|:--------------------|:--------------------|
| wat\_array |100000      |10                 |0.43        | |0.47 |1.14      |1.49        |1.18            |0.99                 |1.18             |1.35                |1.35                |1.41                 |
|wat\_array  |1000000     |10                 |0.76        | |0.56 |1.68      |1.58        |1.48            |1.25                 |1.18             |4.05                |4.24                |4.43                 |
|wat\_array  |10000000    |10                 |0.94        | |0.72 |2.93      |1.63        |1.65            |1.12                 |1.64             |3.44                |3.07                |3.63                 |
|wat\_array  |100000000   |10                 |1.16        | |0.71 |3.89      |1.76        |1.81            |0.88                 |1.29             |1.54                |1.47                |1.56                 |
|            | | | | | | | | | | | | | |
|wat\_array  |100000      |1000               |1.21        | |1.30 |2.87      |3.36        |3.29            |4.55                 |3.44             |7.72                |139.93              |140.97               |
|wat\_array  |1000000     |1000               |2.25        | |1.76 |4.23      |3.63        |3.70            |3.81                 |3.23             |6.49                |9.46                |10.52                |
|wat\_array  |10000000    |1000               |2.85        | |2.29 |6.69      |3.90        |4.19            |4.01                 |2.88             |5.72                |5.79                |6.29                 |
|wat\_array  |100000000   |1000               |3.85        | |2.63 |10.67     |4.90        |5.33            |3.77                 |3.45             |7.53                |259.22              |258.71               |
|            | | | | | | | | | | | | | |
|baseline    |100000      |10                 |0.01        | |52.01 |98.55     |56.30       |6175.32         |193.59               |504.89           |505.88              |505.89              |504.79               |
|baseline    |1000000     |10                 |0.02        | |666.03 |1003.07   |159.32      |19096.71        |548.93               |3544.36          |3542.05             |3609.53             |3543.30              |
|baseline    |10000000    |10                 |0.12        | |6731.32 |10213.38  |880.84      |120975.10       |2849.82              |16900.20         |16923.59            |16912.10            |16916.48             |
|baseline    |	100000000  |	10                | | 0.14       | 	68944.74| 	107285.60| 	116583.40 | 	25702300.00   |	111079.40           | 	4383978.00     | 	4430874.00        |	4400354.00         |	4367531.00          |
|            | | | | | | | | | | | | | |
|baseline    |100000      |1000               |0.01        | |57.01 |44.38     |22.73       |2589.96         |97.68                |179.80           |179.76              |179.86              |180.86               |
|baseline    |1000000     |1000               |0.02        | |573.34 |484.05    |123.63      |15491.85        |466.28               |1923.40          |1923.40             |1925.22             |1926.46              |


The results show that wat-array is very efficient even for large alphabet, and almost constant for the lenght of the array.

## Acknowledgement ##
wat-array is developed at the 20% project at Preferred Infrastructure Inc.

wat-array uses the following works
  * Francisco Claude and Gonzalo Navarro. "Practical Rank/Select Queries over Arbitrary Sequences", Proc. SPIRE'08
  * Travis Gagie, Gonzalo Navarro, and Simon Puglisi, "Colored Range Queries and Document Retrieval", Proc. SPIRE'10
  * Shane Culpepper, Gonzalo Navarro, Simon Puglisi, and Andrew Turpin. "Top-k Ranked Document Search in General Text Databases", Proc. ESA'10
  * R.Grossi, A. Gupta, and J. Vitter "High-order entropy-compressed text indexes", Proc. SODA 03
  * Thanks Mr. Tabei for reporting the bug about Log2().
  * Thanks Mr. Tsubosaka for reporting the bug about FreqRange and Rank()