【ソフト名】 PopTwit
【著作権者】 Copyright(C) 2014 tapetums
【種　　別】 Win32/64 アプリケーション
【動作環境】 Windows Vista/7/8/8.1 32/64-bit
【開発環境】 Visual C++ 2013 Express Edition

【概要】
ツイッターに投稿するだけのアプリです。マルチアカウント／マルチユーザーに対応しています。

作者の環境では Windows 7 64-bit で動作確認を行っています。
Windows XP では起動しません。


【依存関係】
Microsoft Visual C++ 2008 および 2013 ランタイムが必要です。
また、以下のライブラリを使用しています。

<OpenSSL>
http://slproweb.com/products/Win32OpenSSL.html

<libssh2>
http://www.libssh2.org/download/

<cURL>
http://curl.haxx.se/download.html


【インストール】
お好きなフォルダに展開してお使いください。
32ビット版 は x86 フォルダ一式を、
64ビット版 は x64 フォルダ一式を それぞれお使いください。

本体 exe ファイルの他に、以下の 4 つの dll ファイルが必要です。
exe ファイルと同じ階層のフォルダに配置してください。
・libeay32.dll
・ssleay32.dll
・libssh2.dll
・libcurl.dll
いずれもアーカイブに含まれています。


【アンインストール】
 %LOCALAPPDATA%\PopTwit に一時ファイルを保存しています。
 ( Windows 7 であれば C:\Users\アカウント名\AppData\Local\PopTwit\ )
お手数ですが手動での削除をお願いします。
レジストリは使用していません。


【使い方】
1. ウィンドウ右上にあるボタンをクリックするか、Ctrl+U でユーザーリスト編集画面を開き、
   自分の Twitter のユーザー名を入力します。
2. 初回投稿時にパスワードを入力します。
   以後は取得したアクセストークンだけを使って書き込めるようになります。
3. Ctrl+↑ / Ctrl+↓ でユーザーを切り替えます。
   最大 8 アカウントまでに対応しています。


【既知の不具合】
・ 文字列を選択状態にすると透明になってしまう
・ Ctrl + A で 文字列の全選択ができない
・ Ctrl+Enter から送信した場合、キャンセルすると改行が入力されてしまう


【ライセンス】
NYSL です。このプラグラムを使用したことによる一切の影響は関知しません。
http://www.kmonos.net/nysl/

なお、各ライブラリはそれぞれのライセンスに従います。


【更新履歴】
2014.10.12 v0.1.1.0
・%LOCALAPPDATA%\PopTwit が存在しない場合認証情報を保存できないバグを修正
・32ビット版を ANSI エンコードに変更
・二重起動を防止するようにした

2014.10.10 v0.1.1.0
・初版作成


【文責】
tapetums
https://github.com/tapetums
