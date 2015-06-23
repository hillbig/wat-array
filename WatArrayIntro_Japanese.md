# はじめに #

wat-array は様々な配列上の操作を行うことができるwavelet treeを利用したC++ライブラリです。

wat-arrayは位置範囲と文字範囲をした上で次のような操作を行うことができます `count` /   `mode` /   `max` /  `min` /  `list`.
例えばそれらのうちの一つの操作を用いて、任意の`sp`, `ep`, `k`について`A[sp...ep)`中で`k`番目に大きい要素を探すことができます。
これらの全ての操作は入力長に依存せず、アルファベット数が`s`の時、`O(log s)`時間で求められます(`ListModeRange`を除く).

wat-arrayのもう一つの特徴が、作業領域量が小さいことです。wat-arrayは元の入力配列を必要とせず、さらに元の入力配列の情報を復元することもできます（いわゆるself-index）、そのため、ユーザーは元の入力配列の代わりにwat-arrayを利用することができます。正確には`A[0...n)`, `0<=A[i]<s`を入力配列としたときwat-arrayの作業領域量は `n log s` bitです（logの底は2とする）

例えば, `n=10^8`、`k=10^3`の時、wat-arrayの作業領域量は`10^8 * log(10^3) = 1Gbit = 125MB`であり、先程のk番目に大きい値を求める操作はどのようなクエリであっても1マイクロ秒未満（100万qps以上）で行うことができます。

# ライセンス #

wat-arrayはフリーソフトウェアであり、修正BSDライセンスに基づいて利用できます．

# クイックスタート #

wat-arrayは現在Ubuntu Linuxでの動作をサポートしています。多くのLinuxで動作することを想定しています。

はじめに、最新のtarball を[Downloads](http://code.google.com/p/wat-array/downloads/list)からダウンロードしてください。

次のようなコマンドでwat-arrayがインストールされます
```
> tar xvjf wat-array-x.x.x.tar.bz2
> cd wat-array-x.x.x
> ./waf configure
> ./waf
sudo ./waf install
```

プログラム中で利用する場合は次のようにしてwat-arrayに配列をセットして使います
```
#include <wat_array/wat_array.hpp>

....
vector<uint64_t> array;
... // set your array;
wat_array::WatArray wa;
wa.Init(array);

```

wat-array/performance\_test/performance\_test.cppに、実際に利用したサンプルプログラムがあります。
wat-array/test/wat\_array\_test.cppには想定される動作を記述したサンプルプログラムがあります。

なお、wat-array/tool/以下は現在メンテナンス中で利用することはできません。

# API #
wat-arrayがサポートしている操作について以下にまとめます。APIの正確な詳細については [wat\_array.hpp](http://code.google.com/p/wat-array/source/browse/trunk/src/wat_array.hpp)を参照してください。

| **Method** |  |
|:-----------|:-|
| `Init(A)`  | 入力配列 `A`に対するwat-arrayを構築する |
| `length()` | `A`の長さを返す |
| `alphabet_num()` | `A`中のアルファベット数 (A中の最大値+1) を返す |
| `Freq(c)`  | `A`中の文字`c`の出現回数を返す|
| `Lookup(pos) `|  `A[pos]`を返す |
| `Rank(c, pos)` |  `A[0...pos)`中の文字`c`の数を返す |
| `Select(c, rank)` | `A`の文字`c`の(rank)番目の出現位置を返す |
| `RankLessThan(c, pos)` | `A[0...pos)`中の`c'<c`の出現回数を返す  |
| `RankMoreThan(c, pos)` | `A[0...pos)`中の`c'>c` の出現回数を返す  |
| `FreqRange(min_c, max_c, beg_pos, end_pos)` |  `A[beg_pos...end_pos)`中の `min_c<=c'<max_c`の出現回数を返す |
| `MaxRange(beg_pos, end_pos)` | `A[beg_pos...end_pos)`中の最大値とその出現位置を返す  |
| `MinRange(beg_pos, end_pos)` |  `A[beg_pos...end_pos)`中の最小値とその出現位置を返す |
| `QuantileRange(beg_pos, end_pos, k)` | `A[beg_pos...end_pos)`中のk番目の最小値とその出現位置を返す |
| `ListModeRange(min_c, max_c, beg_pos, end_pos, num)` |`A[beg_pos...end_pos)`中に出現した文字の中で`min_c<=c<max_c`を満たす文字の中で出現数が多い順に文字とその出現数を最大num個返す|
| `ListMinRange(min_c, max_c, beg_pos, end_pos, num)` | `A[beg_pos...end_pos)`中に出現した文字の中で`min_c<=c<max_c`を満たす文字の中で、小さい順に文字とその出現数を最大num個返す |
| `ListMaxRange(min_c, max_c, beg_pos, end_pos, num)` | `A[beg_pos...end_pos)`中に出現した文字の中で`min_c<=c<max_c`を満たす文字の中で、大きい順に文字とその出現数を最大num個返す|
| `Save(os)` | 現在の状態（索引）をosに保存する|
| `Load(is)` | 状態（索引）をisから読み込む|

MaxRange, MinRange, QuantileRangeについては、もし該当するような位置が複数存在する場合、最左の位置を返す。
引数が有効で無い場合はwat\_array::NOTFOUNDを返す。
Rankの場合はc>=alphabet\_numの時、Selectの時はc>=alphabet\_numまたは、rank>Freq(c)の時、

# 例 #

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

# 実験結果 #

wat-arrayとbaselineの性能を人工データの元で比較した。baselineは索引を用いない場合での結果であり、例えば`Rank(c, i)`は`A[0...i)`を全てチェックすることで調べている
これらのテストプログラムは次の場所にある [wat-array/test/performance\_test.cpp](http://code.google.com/p/wat-array/source/browse/trunk/test/performance_test.cpp)

この表では、1クエリあたりの平均実行時間を示す。単位はマイクロ秒 (`10^{-6}`秒)である. 100回の有効なランダムクエリの結果の平均をとった

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


これらの結果により次のような考察を得られます。

  * wat-arrayの実行時間は入力サイズに対してはほぼ一定（大きいサイズでの増加はキャッシュミスによるものと思われる）
  * wat-arrayの実行時間はアルファベットサイズに対してはマイルドな増加

また、同じくランダムに生成された配列に対するwat-arrayの構築時間については次の通りでした。

| **length** | **alpha\_num** | **total time(sec)** | **time per input bit (10^-9 sec)** |
|:-----------|:---------------|:--------------------|:-----------------------------------|
|1000        |10              |0.00                 |15.36                               |
|1000        |1000            |0.00                 |12.34                               |
|10000       |10              |0.00                 |12.70                               |
|10000       |1000            |0.00                 |11.14                               |
|100000      |10              |0.00                 |12.47                               |
|100000      |1000            |0.01                 |11.67                               |
|100000      |100000          |0.02                 |13.24                               |
|1000000     |10              |0.04                 |12.84                               |
|1000000     |1000            |0.12                 |11.65                               |
|1000000     |100000          |0.25                 |14.80                               |
|10000000    |10              |0.43                 |12.86                               |
|10000000    |1000            |1.24                 |12.48                               |
|10000000    |100000          |2.74                 |16.51                               |
|10000000    |10000000        |12.88                |55.39                               |
|100000000   |10              |4.25                 |12.80                               |
|100000000   |1000            |12.80                |12.85                               |
|100000000   |100000          |32.87                |19.79                               |
|100000000   |10000000        |135.98               |58.48                               |

入力1bitあたり、大体数十ナノ秒、秒間で約10MB程度の入力を処理できる計算になります。

構築時間に関しても入力長に対して線形、アルファベット数に対しては対数の計算量を必要とします。

## TODO ##
  * Wavelet Treeの解説
  * 作業領域量を少なくする
  * Geometry Search, 2次元サーチ
  * AND検索、ゲノム配列におけるPaired-End (PE) マッピング

## 謝辞 ##
wat-arrayはプリファードインフラストラクチャーの20%プロジェクトで作られました．

wat-arrayは多くの研究成果を参考にして作られています。
  * Francisco Claude and Gonzalo Navarro. "Practical Rank/Select Queries over Arbitrary Sequences", Proc. SPIRE'08
> Wavelet Treeでアルファベット数分ビット配列を持たずに、つなげて保持する方法について
  * Travis Gagie, Gonzalo Navarro, and Simon Puglisi, "Colored Range Queries and Document Retrieval", Proc. SPIRE'10
> wat-array/tool中のdoc\_searchについて
  * Shane Culpepper, Gonzalo Navarro, Simon Puglisi, and Andrew Turpin. "Top-k Ranked Document Search in General Text Databases"
Proc. ESA'10
> List系の操作、特にListModeRange
  * R.Grossi, A. Gupta, and J. Vitter "High-order entropy-compressed text indexes", Proc. SODA 03
> Wavelet Treeの元論文
  * tabeiさんにLog2()の問題を指摘していただきました。ありがとうございます。
  * tsubosakaさんにFreqRange, Rankの問題を指摘していただきました。ありがとうございます。
