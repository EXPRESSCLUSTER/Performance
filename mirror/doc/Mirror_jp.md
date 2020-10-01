# ミラー統計情報
- CLUSTERPRO のミラー統計情報を確認し、アラートの送信などを行う clpperfchk コマンドを提供します。
  - 現時点では、Linux 版のみに対応しています。

## アラートの送信
```sh
# clpperfchk alert [label] [threshold] [times] [method] [file path]
```
- 第 1 引数: **alert** を指定してください。
- 第 2 引数: ミラー統計情報の **統計値名** を指定してください。統計値名については以下を参照してください。
  - https://www.manuals.nec.co.jp/contents/system/files/nec_manuals/node/497/L42_MG_JP/L_MG.html#mirror-statistics-information-collection-function
- 第 3 引数: **しきい値** を指定してください。
- 第 4 引数: しきい値が連続して **何回超えたか** を指定してください。
- 第 5 引数: CLUSTERPRO の clplogcmd コマンドに指定するアラートの送信方法 (syslog, alert, mail, trap)を指定してください。各送信方法の詳細については以下を参照してください。
  - https://www.manuals.nec.co.jp/contents/system/files/nec_manuals/node/497/L42_RG_JP/L_RG_08.html#clplogcmd
- 第 6 引数: ミラー統計情報のファイルパスを指定してください。既定値は以下になります。
  - /opt/nec/clusterpro/perf/disk/nmp{*}.cur
    - {*} にはミラーディスクの番号が入ります。

### コマンド実行例
- ミラーディスクコネクトのハートビート時間が 20秒 を連続して 5回 超えたら mail を送信する
  ```sh
  # clpperfchk alert "MDC HB Time, Max2" 20 5 /opt/nec/clusterpro/perf/disk/nmp1.log.cur
  ```

### クラスタの構成
- CLUSTERPRO でミラーディスク型のクラスタを構築し、EXECリソースから clpperfchk コマンドを実行します。
  ```
  +---------------------------------------------+
  | CLUSTERPRO                                  |
  |  |                                          |
  |  +-- EXEC リソース (非同期モードで実行)     |
  |       |                                     |
  |       +-- clpperfchk                        |
  |            | (しきい値を指定回数超えた場合) |
  |            +-- clplogcmd                    |
  +---------------------------------------------+
  ```

## ソースコード案 (いずれ削除予定)
```c
// mdperf.h
#define LABEL_LEN 128
#define ROW_LEN 2048
#define PATH_LEN 1024

int sendalert(char *label, int threshold, int times, int interval, char *method, char *path);
```
```c
// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdperf.h"

int main
(
        int argc,
        char **argv
)
{
    (省略)
        if (argv[1], "alert")
        {
                sendalert(label, threshold, times, interval, method, path);
        }
        else
        {
                printf("%d: Invalid parameter.\n", __LINE__);
                return 1;
        }
    return 0;
}


int
sendalert
(
        char *label,
        int threshold,
        int times,
        int interval,
        char *method,
        char *path
)
{
    (省略)
        /* find the column */
        fp = fopen(path, "r");
    (省略)
        while (1)
        {
            (該当の行を探す)
            sleep(interval);
        }
    return 0;
}        
```
