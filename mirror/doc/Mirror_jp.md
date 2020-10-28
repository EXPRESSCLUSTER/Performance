# ミラー統計情報
- CLUSTERPRO のミラー統計情報を確認し、アラートの送信などを行う clpperfchk コマンドを提供します。
  - 現時点では、Linux 版のみに対応しています。

## アラートの送信
```sh
# clpperfchk alert [label] [threshold] [times] [interval] [method] [path]
```
- 第 1 引数: alert
  - **alert** を指定してください。
- 第 2 引数: label
  - ミラー統計情報の **統計値名** を指定してください。統計値名については以下を参照してください。
    - https://www.manuals.nec.co.jp/contents/system/files/nec_manuals/node/497/L42_MG_JP/L_MG.html#mirror-statistics-information-collection-function
- 第 3 引数: threshold
  - **しきい値** を指定してください。
- 第 4 引数: times
  - しきい値が連続して **何回超えたか** を指定してください。
- 第 5 引数: interval
  - ミラー統計情報を確認する間隔を指定してください。単位は 秒 です。例えば、60 を指定すると、60秒ごとにミラー統計情報ファイルのチェックをおこないます。
- 第 6 引数: method
  - CLUSTERPRO の clplogcmd コマンドに指定するアラートの送信方法 (syslog, alert, mail, trap)を指定してください。各送信方法の詳細については以下を参照してください。
    - https://www.manuals.nec.co.jp/contents/system/files/nec_manuals/node/497/L42_RG_JP/L_RG_08.html#clplogcmd
- 第 7 引数: path
  - ミラー統計情報のファイルパスを指定してください。既定値は以下になります。
    - /opt/nec/clusterpro/perf/disk/nmp{*}.cur
      - {*} にはミラーディスクの番号が入ります。

### コマンド実行例
- ミラーディスクコネクトのハートビート時間 (MDC HB Time, Max2) が 20秒 を連続して 5回 超えたら mail を送信する
  ```sh
  # clpperfchk alert "MDC HB Time, Max2" 20 5 60 mail /opt/nec/clusterpro/perf/disk/nmp1.cur
  ```

### クラスタの構成
- CLUSTERPRO でミラーディスク型のクラスタを構築し、カスタムモニタリソースから clpperfchk コマンドを実行します。
- clpperfchk 内でしきい値を超えた場合、clplogcmd コマンドを用いて mail 送信などを行います。
  ```
   CLUSTERPRO
    |
    +-- genw (Async mode)
         |
         +-- clpperfchk
              | 
              +-- clplogcmd
  ```

## ソースコード案 (いずれ削除予定)
```c
// mdperf.h
#define LABEL_LEN 128
#define ROW_LEN 2048
#define PATH_LEN 1024

#deinfe ERR_INVALID_PARAM 1

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
        /* define variables */
        char label[LABEL_LEN];
        char method[LABEL_LEN];
        char path[PATH_LEN];
        int times;
        int interval;
        long threshold;        

        /* initialize variables */
        /*
         * Expamle:
         * times = 0;
         * threshold = 0;
         */

　　　  /* check option */
        if (!strcmp(argv[1], "alert"))
        {
                sendalert(label, threshold, times, interval, method, path);
        }
        else
        {
                printf("%d: Invalid parameter.\n", __LINE__);
                return ERR_INVALID_PARAM;
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
        char *token = NULL;
        char tmp[ROW_LEN];
        int column;

    /* snip */

        /* find the column */
        fp = fopen(path, "r");
        fgets(tmp, sizeof(tmp), fp);
        token = strtok(tmp, "\"")
        i = 0;
        /* Find the column */
        while (token != NULL)
        {
                if (!strcmp(token, label))
                {
                    /* Bingo! */
                    column = i;
                    printf("column: %d\n", column);
                    break;
                }
                token = strtok(NULL, "\"");
                i++;
        }
        /* Find the last row */
        while (1)
        {
            /* find the column on the last row */
            sleep(interval);
        }
    return 0;
}        
```
