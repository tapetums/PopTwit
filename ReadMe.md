#PopTwit / PopTrend

**[twitcurl](https://code.google.com/p/twitcurl/)** を 利用した Twitter クライアントアプリです。  
詳細は各プロジェクトの README.TXT をご覧ください。

##開発環境

Windows 7 64-bit  
Microsoft Visual C++ 2013 Update 3


##動作環境
　Windows Vista/7/8/8.1 32/64-bit  
　Microsoft Visual C++ 2008 および 2013 ランタイムが必要です。


##依存関係
- [OpenSSL](http://slproweb.com/products/Win32OpenSSL.html)
 - libeay32.dll
 - ssleay32.dll
- [libssh2](http://www.libssh2.org/download/)
  - libssh2.dll
- [libcurl](http://curl.haxx.se/download.html)
  - libcurl.dll


##ソースコードをご利用の方へ
ConsumerKey.h ファイルはレポジトリに含まれていません。  
ご自身でファイルをご用意いただき、コンシューマーキーを定義してください。  
例：
'''c
#define CONSUMER_KEY        "XXXXXXXXXXXXXXXXXXXXXXXXX"
#define CONSUMER_KEY_SECRET "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
'''


##ライセンス
NYSL です。  
[http://www.kmonos.net/nysl/](http://www.kmonos.net/nysl/)  

ただし、各ライブラリはそれぞれのライセンスに従います。

---

以上です。

####文責
tapetums
