#PopTwit

ツイッターに投稿するだけのアプリです。マルチアカウント／マルチユーザー対応。

##開発環境

Windows 7 64-bit  
Microsoft Visual C++ 2013 SP1


##依存関係
- OpenSSL
 - libeay32.dll
 - ssleay32.dll
- libcurl
  - libcurl.dll
  - libssh2.dll


##動作環境
　Windows Vista/7/8/8.1 32/64-bit


##使い方
1. 上記 4 つの dll ファイルを入手して、exe ファイルと同じ階層のフォルダに配置します。
2. ウィンドウ右上にあるボタンをクリックするか、Ctrl+U でユーザーリスト編集画面を開き、自分の Twitter のユーザー名を入力します。
3. 初回投稿時にパスワードを入力します。以後は取得したアクセストークンだけを使って書き込めるようになります。アクセストークンは %LOCALAPPDATA%PopTwit にあります。( Win7 であれば C:\Users\アカウント名\AppData\Local\PopTwit\ )
4. Ctrl+↑ / Ctrl+↓ でユーザーを切り替えます。最大 8 アカウントまでに対応しています。


##ライセンス
NYSL です。  
[http://www.kmonos.net/nysl/](http://www.kmonos.net/nysl/)  

ただし、各ライブラリはそれぞれのライセンスに従います。

---

以上です。

####文責
tapetums
