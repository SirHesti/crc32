
## create crc32 and/or md5
Builds a simple crc32 and md5 hash. Very Oldschool but sometimes the old one is really good and enough
and there is no compare cmd. 


Usage:
```
crc32 V1.18.0 Linuxe X86
-?  This Help
-5  build md5 too
-o  do not build crc32
-h  [File] only one File one CRC and display only the CRC !!
-d  [Dir]  crc32 for the complete Dir - if no one specified, then the current dir is taken
    [File] only the one CRC; if more then one is given then crc [tab] filename is printed
```

Like:
```ts
crc32 -h /usr/bin/ls
6850d96c
```

or like this one:
```ts
crc32 -d /hs/tmp/seterror
ae469fa1	/hs/tmp/seterror/README.md
00000000	/hs/tmp/seterror/index.html
```

compare:
```ts
crc32 -d /hs/tmp/seterror | diff - compare.crc
[ $? -eq 0 ] || echo error
```
