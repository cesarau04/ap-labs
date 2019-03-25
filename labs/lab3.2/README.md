Lab 3.2 - Progress Notifier with Signals
========================================
This program encode a given text in Base64 (code taken from [wikibooks](https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64)) the objective is to handle signals SIGINT & SIGINFO (only in BSD, see [issue](https://github.com/golang/go/issues/1653)).

Encoding
--------
```
./base64 --encode input.txt
```
Your program should generate a new `encoded.txt` file with the result.

Decoding
--------
```
./base64 --decode encoded.txt
```
Your program will generate a `decoded.txt` file witg the result.

How to test?
------------
- `Ctrl + C` this way is the easiest.
- Get process ID
```
ps aux | grep base64
```

- Send the signals
```
kill -SIGINT <PID>
kill -SIGINFO <PID>
```

Test files
----------
- [big.txt](https://github.com/cesarau04/ap-labs/blob/master/labs/lab3.2/big.txt)

