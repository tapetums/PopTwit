【ソフト名】 PopTrend
【著作権者】 Copyright(C) 2014 tapetums
【種　　別】 Win32/64 アプリケーション
【動作環境】 Windows Vista/7/8/8.1 32/64-bit
【開発環境】 Visual C++ 2013 Express Edition

【概要】
ツイッターのトレンド情報を取得するアプリです。

作者の環境では Windows 7 64-bit で動作確認を行っています。
Windows XP では起動しません。


【依存関係】
Microsoft Visual C++ 2008 および 2013 ランタイムが必要です。
また、以下のライブラリを使用しています。

・OpenSSL
http://slproweb.com/products/Win32OpenSSL.html

・libssh2
http://www.libssh2.org/download/

・libcurl
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
 %LOCALAPPDATA%\PopTrend に一時ファイルを保存しています。
 ( Windows 7 であれば C:\Users\アカウント名\AppData\Local\PopTrend\ )
お手数ですが手動での削除をお願いします。
レジストリは使用していません。


【使い方】
初回起動時にダイアログが表示されるので、ご自身の Twitter ユーザー名と
パスワードを入力してください。以後は自動的に情報を取得します。

PopTrend.ini をテキストエディタで編集することで、以下の設定が可能です:
・enabled
　0 にすると PopTrend.exe は起動しません。
・woeid
　トレンドを取得する地域のIDを指定します。
　　(例: 23424856 = 日本全域、1118370 = 東京、15015370 = 大阪)
　デフォルトでは 23424856 になっています。
　ご希望の地域IDをお探しの場合は "WOEID" で検索してみてください。
・no_hashtags
　1 にすると、ハッシュタグをトレンドワードに含めません。
・interval
　更新間隔を分単位で指定します。0 にすると手動更新のみとなります。
　Twitter 社の制約により、15分間に15回以上の更新はできません(2014/10/14 現在)。
・username
　Twitter にログインする際のユーザー名です。

ini ファイルの編集は、PopTrend.exe を終了させた状態で行ってください。


【ライセンス】
NYSL です。このプラグラムを使用したことによる一切の影響は関知しません。
http://www.kmonos.net/nysl/

なお、各ライブラリはそれぞれのライセンスに従います。


【更新履歴】
2014.10.14 v0.1.0.0
・初版作成


【文責】
tapetums
https://github.com/tapetums
