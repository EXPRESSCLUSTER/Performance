# [clpperfchk]コマンド--ミラー統計情報に応じてAlert報告を行う
- 本ページでは、CLUSTERPROのアラートを行うclpperfchk コマンドについて、コマンド引数から実行例、CLUSTERPRO からの実行方法、動作確認までを分かりやすく紹介していきます。
- CLUSTERPRO のミラー統計情報を確認し、アラートの送信などを行う clpperfchk コマンドを提供します。
  - 現時点では、Linux 版のみに対応しています。

### clpperfchk のコマンド引数
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
- ミラーディスクコネクトのハートビート時間 (MDC HB Time, Cur) が 2秒 を連続して 5回 超えたら syslogに記述します。
  ```sh
  # clpperfchk alert "MDC HB Time, Cur" 2 5 60 syslog /opt/nec/clusterpro/perf/disk/nmp1.cur
  ```

### CLUSTERPRO からの実行方法
- CLUSTERPRO でミラーディスク型のクラスタを構築し、EXEC リソースから clpperfchk コマンドを実行します。
- clpperfchk 内でしきい値を超えた場合、clplogcmd コマンドを用いて mail 送信などを行います。
  ```
   CLUSTERPRO
    |
    +-- exec (Async mode)
         |
         +-- clpperfchk
              | 
              +-- clplogcmd
  ```

### クラスタ構成例
- サーバ
  - サーバ名: sv1, sv2
- フェイルオーバグループ
  - failover
    - 全てのサーバでフェイルオーバ可能
      - リソース
        - md (ミラーディスクリソース)
        - その他業務に必要なリソース
  - perfchk-sv1
    - sv1 のみで起動可能
      - リソース
        - exec-perfchk-sv1 (EXEC リソース)
  - perfchk-sv2
    - sv2 のみで起動可能
      - リソース
        - exec-perfchk-sv2 (EXEC リソース)
- モニタリソース
  - pidw-perfchk-sv1
  - pidw-perfchk-sv2
<!--
- サーバ
  - sv1, sv2
- フェイルオーバグループ
  - failover: 業務用のグループ
    - md
  - perfchk-sv1
    - 
  - perfchk-sv2
-->


### 設定手順
1. 以下のマニュアルを参考に、ミラーディスク型のクラスタを構築してください。
   - https://www.manuals.nec.co.jp/contents/system/files/nec_manuals/node/497/L42_IG_JP/index.html
1. clpperfchk をそれぞれのクラスタサーバの /opt/nec/clusterpro/bin に保存してください。
1. Cluster WebUI を起動し、[設定モード] に切り替えてください。
1. それぞれのサーバでのみ起動するフェイルオーバグループを作成してください。
   - [起動サーバ] タブにて、[全てのサーバでフェイルオーバ可能] のチェックを外し、いずれかのサーバを追加してください。
1. 手順4で加えたフェイルオーバーグループに EXEC リソースを追加し、以下を設定してください。
   - [詳細] タブ
     - [調整]ボタン [開始スクリプト] : 非同期
   - Start Script
       ```sh
       #! /bin/sh
       #***************************************
       #*              start.sh               *
       #***************************************
       
       PERFCHKCMD="/opt/nec/clusterpro/bin/clpperfchk"
       LABEL="MDC HB Time, Cur"
       THRESHOLD="2"
       TIMES="5"
       INTERVAL="60"
       METHOD="syslog"
       CHKPATH="/opt/nec/clusterpro/perf/disk/nmp1.cur"

       $PERFCHKCMD alert "$LABEL" $THRESHOLD $TIMES $INTERVAL $METHOD $CHKPATH
       exit 0
       ```
   - [詳細] タブ
     - [調整]ボタン [停止スクリプト] : 同期
   - Stop Script   
       ```sh
       #! /bin/sh
       #***************************************
       #*               stop.sh               *
       #***************************************
       
       kill -9 `ps ax |grep clpperfchk | grep nmp1 | awk '{print $1}'`
       
       exit 0
       ```
1. PID モニタリソースを2つ追加し、それぞれ以下のように設定してください。
   - [監視 (共通) ] タブ
     - 対象リソース: exec-perfchk-sv1 または exec-perfchk-sv2
   - [回復動作] タブ
     - カスタム設定
     - 回復対象: exec-perfchk-sv1 または exec-perfchk-sv2
     - 最大再活性回数: 任意の値
     - 最大フェイルオーバ回数: 0


### 動作確認
1. スクリプトを利用してミラーディスクリソースに 60 秒ごとにファイルの作成、削除を行う。
1. 第6引数を syslog に指定してclpperfchkコマンドを実行する。
1. alertが送信されたら、/var/log/messergeで確認する。
