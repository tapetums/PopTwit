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
- twitcurl


##使い方
　上記 4 つの dll ファイルを入手して、exe ファイルと同じ階層のフォルダに配置します。PopTwit.ini を開き、[account] の 01 に 自分の Twitter のログインアカウントを書き込みます。初回投稿時にパスワードを入力すると、以後は取得したアクセストークンだけを使って書き込めるようになります。  
　アクセストークンは %LOCALAPPDATA%PopTwit にあります。( Win7 であれば C:\Users\アカウント名\AppData\Local\PopTwit\ )

---

　まだ使い難いので改良予定


##ライセンス
NYSL です。  
[http://www.kmonos.net/nysl/](http://www.kmonos.net/nysl/)  

ただし、各ライブラリはそれぞれのライセンスに従います。

---

以上です。

####文責
tapetums
